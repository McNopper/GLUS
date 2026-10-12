/*
 * GLUS - Modern OpenGL, OpenGL ES and OpenVG Utilities. Copyright (C) since 2010 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// Wayland windowing backend: the window is an xdg-shell toplevel, the EGL
// native window the matching wl_egl_window of libwayland-egl, and input
// arrives through wl_seat. The windowing is GPU independent - every EGL stack
// with Wayland support understands wl_egl_window - which is why selecting it
// needs no GPU.

#include "GL/glus.h"

#include <wayland-client.h>
#include <wayland-egl.h>

#include "xdg-shell-client-protocol.h"

#include "GLUS/glus_os_linux_keymap.h"

#include <string.h>

#include <unistd.h>

#include <time.h>

extern GLUSvoid _glusWindowInternalClose(GLUSvoid);

extern GLUSvoid _glusWindowInternalKey(GLUSint key, GLUSint state);

extern GLUSvoid _glusWindowInternalMouse(GLUSint button, GLUSint action);

extern GLUSvoid _glusWindowInternalMouseWheel(GLUSint pos);

extern GLUSvoid _glusWindowInternalMouseMove(GLUSint x, GLUSint y);

//

static struct wl_display* g_display = NULL;

static struct wl_registry* g_registry = NULL;

static struct wl_compositor* g_compositor = NULL;

static struct xdg_wm_base* g_wmBase = NULL;

static struct wl_seat* g_seat = NULL;

static struct wl_keyboard* g_keyboard = NULL;

static struct wl_pointer* g_pointer = NULL;

static struct wl_touch* g_touch = NULL;

static struct wl_surface* g_surface = NULL;

static struct xdg_surface* g_xdgSurface = NULL;

static struct xdg_toplevel* g_toplevel = NULL;

static struct wl_egl_window* g_eglWindow = NULL;

static GLUSint g_width = -1;

static GLUSint g_height = -1;

// A compositor resize arrives as an xdg_toplevel configure followed by an
// xdg_surface configure; the size is applied when the surface configure
// acknowledges it.
static int g_pendingWidth = 0;

static int g_pendingHeight = 0;

static int g_configured = 0;

static void glusOsXdgWmBasePing(void* data, struct xdg_wm_base* xdgWmBase, uint32_t serial)
{
    (void)data;

    xdg_wm_base_pong(xdgWmBase, serial);
}

static const struct xdg_wm_base_listener g_xdgWmBaseListener = {glusOsXdgWmBasePing};

static void glusOsXdgSurfaceConfigure(void* data, struct xdg_surface* xdgSurface, uint32_t serial)
{
    (void)data;

    xdg_surface_ack_configure(xdgSurface, serial);

    if (g_eglWindow != NULL && g_pendingWidth > 0 && g_pendingHeight > 0 && (g_pendingWidth != g_width || g_pendingHeight != g_height))
    {
        wl_egl_window_resize(g_eglWindow, g_pendingWidth, g_pendingHeight, 0, 0);

        g_width = g_pendingWidth;

        g_height = g_pendingHeight;
    }

    g_configured = 1;
}

static const struct xdg_surface_listener g_xdgSurfaceListener = {glusOsXdgSurfaceConfigure};

static void glusOsXdgToplevelConfigure(void* data, struct xdg_toplevel* xdgToplevel, int32_t width, int32_t height, struct wl_array* states)
{
    (void)xdgToplevel;
    (void)states;

    // Zero dimensions mean "choose your own size"; the requested one stays.
    if (width > 0 && height > 0)
    {
        g_pendingWidth = (int)width;

        g_pendingHeight = (int)height;
    }
}

static void glusOsXdgToplevelClose(void* data, struct xdg_toplevel* xdgToplevel)
{
    (void)xdgToplevel;

    _glusWindowInternalClose();
}

static const struct xdg_toplevel_listener g_xdgToplevelListener = {glusOsXdgToplevelConfigure, glusOsXdgToplevelClose};

static void glusOsKeyboardKeymap(void* data, struct wl_keyboard* wlKeyboard, uint32_t format, int32_t fd, uint32_t size)
{
    (void)data;
    (void)wlKeyboard;
    (void)format;
    (void)size;

    // Raw key codes are translated without a key map, but the descriptor
    // belongs to the client and must be closed either way.
    close(fd);
}

static void glusOsKeyboardEnter(void* data, struct wl_keyboard* wlKeyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys)
{
    (void)data;
    (void)wlKeyboard;
    (void)serial;
    (void)surface;
    (void)keys;
}

static void glusOsKeyboardLeave(void* data, struct wl_keyboard* wlKeyboard, uint32_t serial, struct wl_surface* surface)
{
    (void)data;
    (void)wlKeyboard;
    (void)serial;
    (void)surface;
}

static void glusOsKeyboardKey(void* data, struct wl_keyboard* wlKeyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    (void)data;
    (void)wlKeyboard;
    (void)serial;
    (void)time;

    // The key is an evdev key code, as read from /dev/input/event*.
    _glusWindowInternalKey(glusOsTranslateKey((int)key), state == WL_KEYBOARD_KEY_STATE_PRESSED ? GLFW_PRESS : GLFW_RELEASE);
}

static void glusOsKeyboardModifiers(void* data, struct wl_keyboard* wlKeyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
{
    (void)data;
    (void)wlKeyboard;
    (void)serial;
    (void)modsDepressed;
    (void)modsLatched;
    (void)modsLocked;
    (void)group;
}

static const struct wl_keyboard_listener g_keyboardListener = {
    glusOsKeyboardKeymap, glusOsKeyboardEnter, glusOsKeyboardLeave, glusOsKeyboardKey, glusOsKeyboardModifiers};

static void glusOsPointerEnter(void* data, struct wl_pointer* wlPointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t surfaceX, wl_fixed_t surfaceY)
{
    (void)data;
    (void)wlPointer;
    (void)serial;
    (void)surface;

    _glusWindowInternalMouseMove(wl_fixed_to_int(surfaceX), wl_fixed_to_int(surfaceY));
}

static void glusOsPointerLeave(void* data, struct wl_pointer* wlPointer, uint32_t serial, struct wl_surface* surface)
{
    (void)data;
    (void)wlPointer;
    (void)serial;
    (void)surface;
}

static void glusOsPointerMotion(void* data, struct wl_pointer* wlPointer, uint32_t time, wl_fixed_t surfaceX, wl_fixed_t surfaceY)
{
    (void)data;
    (void)wlPointer;
    (void)time;

    _glusWindowInternalMouseMove(wl_fixed_to_int(surfaceX), wl_fixed_to_int(surfaceY));
}

static void glusOsPointerButton(void* data, struct wl_pointer* wlPointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    GLUSint glusButton = -1;

    (void)data;
    (void)wlPointer;
    (void)serial;
    (void)time;

    switch (button)
    {
        case BTN_LEFT:
            glusButton = GLFW_MOUSE_BUTTON_LEFT;
            break;
        case BTN_RIGHT:
            glusButton = GLFW_MOUSE_BUTTON_RIGHT;
            break;
        case BTN_MIDDLE:
            glusButton = GLFW_MOUSE_BUTTON_MIDDLE;
            break;
    }

    if (glusButton >= 0)
    {
        _glusWindowInternalMouse(glusButton, state == WL_POINTER_BUTTON_STATE_PRESSED ? GLFW_PRESS : GLFW_RELEASE);
    }
}

static void glusOsPointerAxis(void* data, struct wl_pointer* wlPointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    (void)data;
    (void)wlPointer;
    (void)time;

    if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
    {
        _glusWindowInternalMouseWheel(wl_fixed_to_int(value) > 0 ? 1 : -1);
    }
}

static const struct wl_pointer_listener g_pointerListener = {
    glusOsPointerEnter, glusOsPointerLeave, glusOsPointerMotion, glusOsPointerButton, glusOsPointerAxis};

// The touch panel drives the left mouse button, as on the fbdev backend.
static void glusOsTouchDown(void* data, struct wl_touch* wlTouch, uint32_t serial, uint32_t time, struct wl_surface* surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    (void)data;
    (void)wlTouch;
    (void)serial;
    (void)time;
    (void)surface;
    (void)id;

    _glusWindowInternalMouseMove(wl_fixed_to_int(x), wl_fixed_to_int(y));

    _glusWindowInternalMouse(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
}

static void glusOsTouchUp(void* data, struct wl_touch* wlTouch, uint32_t serial, uint32_t time, int32_t id)
{
    (void)data;
    (void)wlTouch;
    (void)serial;
    (void)time;
    (void)id;

    _glusWindowInternalMouse(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
}

static void glusOsTouchMotion(void* data, struct wl_touch* wlTouch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    (void)data;
    (void)wlTouch;
    (void)time;
    (void)id;

    _glusWindowInternalMouseMove(wl_fixed_to_int(x), wl_fixed_to_int(y));
}

static void glusOsTouchFrame(void* data, struct wl_touch* wlTouch)
{
    (void)data;
    (void)wlTouch;
}

static void glusOsTouchCancel(void* data, struct wl_touch* wlTouch)
{
    (void)data;
    (void)wlTouch;
}

static const struct wl_touch_listener g_touchListener = {
    glusOsTouchDown, glusOsTouchUp, glusOsTouchMotion, glusOsTouchFrame, glusOsTouchCancel};

static void glusOsSeatCapabilities(void* data, struct wl_seat* wlSeat, uint32_t capabilities)
{
    (void)data;

    if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) != 0 && g_keyboard == NULL)
    {
        g_keyboard = wl_seat_get_keyboard(wlSeat);

        wl_keyboard_add_listener(g_keyboard, &g_keyboardListener, NULL);
    }

    if ((capabilities & WL_SEAT_CAPABILITY_POINTER) != 0 && g_pointer == NULL)
    {
        g_pointer = wl_seat_get_pointer(wlSeat);

        wl_pointer_add_listener(g_pointer, &g_pointerListener, NULL);
    }

    if ((capabilities & WL_SEAT_CAPABILITY_TOUCH) != 0 && g_touch == NULL)
    {
        g_touch = wl_seat_get_touch(wlSeat);

        wl_touch_add_listener(g_touch, &g_touchListener, NULL);
    }
}

static void glusOsSeatName(void* data, struct wl_seat* wlSeat, const char* name)
{
    (void)data;
    (void)wlSeat;
    (void)name;
}

static const struct wl_seat_listener g_seatListener = {glusOsSeatCapabilities, glusOsSeatName};

static void glusOsRegistryGlobal(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
{
    (void)data;
    (void)version;

    if (strcmp(interface, "wl_compositor") == 0)
    {
        g_compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    }
    else if (strcmp(interface, "xdg_wm_base") == 0)
    {
        g_wmBase = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);

        xdg_wm_base_add_listener(g_wmBase, &g_xdgWmBaseListener, NULL);
    }
    else if (strcmp(interface, "wl_seat") == 0)
    {
        g_seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);

        wl_seat_add_listener(g_seat, &g_seatListener, NULL);
    }
}

static void glusOsRegistryGlobalRemove(void* data, struct wl_registry* registry, uint32_t name)
{
    (void)data;
    (void)registry;
    (void)name;
}

static const struct wl_registry_listener g_registryListener = {glusOsRegistryGlobal, glusOsRegistryGlobalRemove};

static void glusOsDestroyWindow()
{
    if (g_eglWindow != NULL)
    {
        wl_egl_window_destroy(g_eglWindow);

        g_eglWindow = NULL;
    }

    if (g_toplevel != NULL)
    {
        xdg_toplevel_destroy(g_toplevel);

        g_toplevel = NULL;
    }

    if (g_xdgSurface != NULL)
    {
        xdg_surface_destroy(g_xdgSurface);

        g_xdgSurface = NULL;
    }

    if (g_surface != NULL)
    {
        wl_surface_destroy(g_surface);

        g_surface = NULL;
    }

    g_width = -1;

    g_height = -1;

    g_pendingWidth = 0;

    g_pendingHeight = 0;

    g_configured = 0;
}

GLUSvoid _glusOsPollEvents()
{
    if (g_display == NULL)
    {
        return;
    }

    if (wl_display_dispatch_pending(g_display) == -1)
    {
        _glusWindowInternalClose();

        return;
    }

    wl_display_flush(g_display);
}

EGLNativeDisplayType _glusOsGetNativeDisplayType()
{
    if (g_display != NULL)
    {
        return g_display;
    }

    g_display = wl_display_connect(NULL);

    if (g_display == NULL)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not connect to a Wayland compositor");

        return 0;
    }

    g_registry = wl_display_get_registry(g_display);

    wl_registry_add_listener(g_registry, &g_registryListener, NULL);

    // The globals are only bound after a roundtrip.
    if (wl_display_roundtrip(g_display) == -1 || g_compositor == NULL || g_wmBase == NULL)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Wayland compositor does not provide wl_compositor and xdg_wm_base");

        wl_registry_destroy(g_registry);

        g_registry = NULL;

        wl_display_disconnect(g_display);

        g_display = NULL;

        return 0;
    }

    return g_display;
}

EGLNativeWindowType _glusOsCreateNativeWindowType(const char* title, const GLUSint width, const GLUSint height, const GLUSboolean fullscreen, const GLUSboolean noResize, EGLint eglNativeVisualID)
{
    (void)eglNativeVisualID;

    if (g_display == NULL || g_compositor == NULL || g_wmBase == NULL)
    {
        return 0;
    }

    g_surface = wl_compositor_create_surface(g_compositor);

    g_xdgSurface = xdg_wm_base_get_xdg_surface(g_wmBase, g_surface);

    xdg_surface_add_listener(g_xdgSurface, &g_xdgSurfaceListener, NULL);

    g_toplevel = xdg_surface_get_toplevel(g_xdgSurface);

    xdg_toplevel_add_listener(g_toplevel, &g_xdgToplevelListener, NULL);

    xdg_toplevel_set_title(g_toplevel, title != NULL ? title : "");

    xdg_toplevel_set_app_id(g_toplevel, "glus");

    if (noResize)
    {
        xdg_toplevel_set_min_size(g_toplevel, width, height);

        xdg_toplevel_set_max_size(g_toplevel, width, height);
    }

    g_width = width;

    g_height = height;

    wl_surface_commit(g_surface);

    // The toplevel is only mapped after the initial configure.
    while (!g_configured)
    {
        if (wl_display_roundtrip(g_display) == -1)
        {
            glusLogPrint(GLUS_LOG_ERROR, "Wayland compositor did not configure the window");

            glusOsDestroyWindow();

            return 0;
        }
    }

    if (g_pendingWidth > 0 && g_pendingHeight > 0)
    {
        g_width = g_pendingWidth;

        g_height = g_pendingHeight;
    }

    if (fullscreen)
    {
        xdg_toplevel_set_fullscreen(g_toplevel, NULL);
    }

    g_eglWindow = wl_egl_window_create(g_surface, g_width, g_height);

    if (g_eglWindow == NULL)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not create native window");

        glusOsDestroyWindow();

        return 0;
    }

    return g_eglWindow;
}

GLUSvoid _glusOsDestroyNativeWindowDisplay()
{
    glusOsDestroyWindow();

    if (g_keyboard != NULL)
    {
        wl_keyboard_destroy(g_keyboard);

        g_keyboard = NULL;
    }

    if (g_pointer != NULL)
    {
        wl_pointer_destroy(g_pointer);

        g_pointer = NULL;
    }

    if (g_touch != NULL)
    {
        wl_touch_destroy(g_touch);

        g_touch = NULL;
    }

    if (g_seat != NULL)
    {
        wl_seat_destroy(g_seat);

        g_seat = NULL;
    }

    if (g_wmBase != NULL)
    {
        xdg_wm_base_destroy(g_wmBase);

        g_wmBase = NULL;
    }

    if (g_compositor != NULL)
    {
        wl_compositor_destroy(g_compositor);

        g_compositor = NULL;
    }

    if (g_registry != NULL)
    {
        wl_registry_destroy(g_registry);

        g_registry = NULL;
    }

    if (g_display != NULL)
    {
        wl_display_disconnect(g_display);

        g_display = NULL;
    }
}

double _glusOsGetRawTime()
{
    struct timespec currentTime;

    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000.0;
}

GLUSvoid _glusOsGetWindowSize(GLUSint* width, GLUSint* height)
{
    if (width)
    {
        *width = g_width;
    }

    if (height)
    {
        *height = g_height;
    }
}
