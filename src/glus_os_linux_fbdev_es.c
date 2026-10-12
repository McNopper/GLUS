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

// The fbdev windowing entry points (fbGetDisplayByIndex, fbCreateWindow, ...)
// are declared in EGL/eglvivante.h; the header is part of the platform EGL
// package, not of a driver - only the windowing approach is generic.
#include <EGL/eglvivante.h>

#include "GL/glus.h"

// Evdev key codes, shared with the Wayland OS backend.
#include "GLUS/glus_os_linux_evdev.h"

#include <time.h>





//

static EGLNativeDisplayType g_nativeDisplay = 0;

static EGLNativeWindowType g_nativeWindow = 0;

static GLUSint g_width = -1;

static GLUSint g_height = -1;


GLUSvoid _glusOsPollEvents()
{
    glusOsEvdevPoll();
}

EGLNativeDisplayType _glusOsGetNativeDisplayType()
{
    if (g_nativeDisplay != 0)
    {
        return g_nativeDisplay;
    }

    g_nativeDisplay = fbGetDisplayByIndex(0);

    if (g_nativeDisplay == 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not get native display");

        return 0;
    }

    int displayWidth = -1;
    int displayHeight = -1;

    fbGetDisplayGeometry(g_nativeDisplay, &displayWidth, &displayHeight);

    glusOsEvdevSetDisplayGeometry(displayWidth, displayHeight);

    return g_nativeDisplay;
}

EGLNativeWindowType _glusOsCreateNativeWindowType(const char* title, const GLUSint width, const GLUSint height, const GLUSboolean fullscreen, const GLUSboolean noResize, EGLint eglNativeVisualID)
{
    glusLogPrint(GLUS_LOG_INFO, "Parameters 'title', 'fullscreen' and 'noResize' are not used");
    glusLogPrint(GLUS_LOG_INFO, "Key events are mapped to US keyboard");
    glusLogPrint(GLUS_LOG_INFO, "Touch display is used for mouse events and are limited");

    glusOsEvdevInit();


    g_nativeWindow = fbCreateWindow(g_nativeDisplay, 0, 0, width, height);

    if (g_nativeWindow == 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not create native window");

        return 0;
    }

    g_width  = width;
    g_height = height;

    return g_nativeWindow;
}

GLUSvoid _glusOsDestroyNativeWindowDisplay()
{
    if (g_nativeWindow)
    {
        fbDestroyWindow(g_nativeWindow);

        g_nativeWindow = 0;
    }

    if (g_nativeDisplay)
    {
        fbDestroyDisplay(g_nativeDisplay);

        g_nativeDisplay = 0;
    }

    glusOsEvdevTerminate();
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
