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

#ifndef GLUS_OS_LINUX_EVDEV_H
#define GLUS_OS_LINUX_EVDEV_H

// Direct evdev input, shared by the fbdev and GBM OS backends: both read
// /dev/input/event* themselves (the Wayland backend receives its input
// through wl_seat instead). Exactly one OS backend is compiled per build.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include <sys/ioctl.h>

#include "GLUS/glus_os_linux_keymap.h"

// input_event.value semantics for EV_KEY (input.h documents them but defines
// no names); the same press/release pair drives the touch and PowerMate
// buttons.
#define EV_KEY_RELEASED 0
#define EV_KEY_PRESSED  1

// The touch panel reports 15-bit absolute coordinates.
#define TOUCH_AXIS_MAX (32767)

// EVIOCGBIT(0, ...) event-type signatures the device probe matches against:
// keyboard, PowerMate and touch display, in that order.
#define KEYBOARD_EVENT_BITS  ((uint32_t)((1 << EV_SYN) | (1 << EV_KEY) | (1 << EV_MSC) | (1 << EV_LED) | (1 << EV_REP)))
#define POWERMATE_EVENT_BITS ((uint32_t)((1 << EV_SYN) | (1 << EV_KEY) | (1 << EV_REL) | (1 << EV_MSC)))
#define TOUCH_EVENT_BITS     ((uint32_t)((1 << EV_SYN) | (1 << EV_KEY) | (1 << EV_ABS)))

extern GLUSvoid _glusWindowInternalClose(GLUSvoid);

extern GLUSvoid _glusWindowInternalKey(GLUSint key, GLUSint state);

extern GLUSvoid _glusWindowInternalMouse(GLUSint button, GLUSint action);

extern GLUSvoid _glusWindowInternalMouseWheel(GLUSint pos);

extern GLUSvoid _glusWindowInternalMouseMove(GLUSint x, GLUSint y);

//

static int g_keyFileDescriptor = -1;

static int g_touchFileDescriptor = -1;

static int g_powermateFileDescriptor = -1;

static int g_displayWidth = -1;

static int g_displayHeight = -1;

static int g_currentX = -1;

static int g_currentY = -1;

static int g_pressed = 0;

// The touch coordinates are scaled to the display size of the owning
// backend: fbdev takes them from fbGetDisplayGeometry, GBM from the DRM
// mode in use.
static GLUSvoid glusOsEvdevSetDisplayGeometry(const GLUSint width, const GLUSint height)
{
    g_displayWidth = width;
    g_displayHeight = height;
}

static GLUSvoid glusOsEvdevInit()
{
    char eventFilename[256];
    int  eventNumber;

    int fileDescriptor;

    uint32_t eventBits;


    strcpy(eventFilename, "/dev/input/event0");

    for (eventNumber = 0; eventNumber < 10; eventNumber++)
    {
        eventFilename[16] = '0' + (char)eventNumber;

        fileDescriptor = open(eventFilename, O_RDONLY | O_NONBLOCK);

        if (fileDescriptor < 0)
        {
            continue;
        }

        eventBits = 0;
        if (ioctl(fileDescriptor, EVIOCGBIT(0, EV_MAX), &eventBits) < 0)
        {
            close(fileDescriptor);

            continue;
        }

        if (eventBits == KEYBOARD_EVENT_BITS && g_keyFileDescriptor < 0)
        {
            glusLogPrint(GLUS_LOG_INFO, "Found keyboard on '%s'", eventFilename);

            g_keyFileDescriptor = fileDescriptor;

            // Disable echo.
            system("stty -echo");
        }
        else if (eventBits == POWERMATE_EVENT_BITS && g_powermateFileDescriptor < 0)
        {
            glusLogPrint(GLUS_LOG_INFO, "Found power mate on '%s'", eventFilename);

            g_powermateFileDescriptor = fileDescriptor;
        }
        else if (eventBits == TOUCH_EVENT_BITS && g_touchFileDescriptor < 0)
        {
            glusLogPrint(GLUS_LOG_INFO, "Found touch display on '%s'", eventFilename);

            g_touchFileDescriptor = fileDescriptor;
        }
        else
        {
            close(fileDescriptor);
        }
    }

}

static GLUSvoid glusOsEvdevPoll()
{
    if (g_keyFileDescriptor >= 0)
    {
        static GLUSboolean LEFT_CTRL  = GLUS_FALSE;
        static GLUSboolean RIGHT_CTRL = GLUS_FALSE;

        struct input_event keyEvent;

        ssize_t numBytes;

        do
        {
            numBytes = read(g_keyFileDescriptor, &keyEvent, sizeof(struct input_event));

            if (numBytes > 0 && keyEvent.type == EV_KEY)
            {
                switch (keyEvent.value)
                {
                case EV_KEY_PRESSED: // Pressed
                {
                    // CTRL-C
                    if (keyEvent.code == KEY_C && (LEFT_CTRL || RIGHT_CTRL))
                    {
                        _glusWindowInternalClose();

                        return;
                    }
                    else
                    {
                        if (keyEvent.code == KEY_LEFTCTRL)
                        {
                            LEFT_CTRL = GLUS_TRUE;
                        }
                        else if (keyEvent.code == KEY_RIGHTCTRL)
                        {
                            RIGHT_CTRL = GLUS_TRUE;
                        }

                        _glusWindowInternalKey(glusOsTranslateKey(keyEvent.code), GLFW_PRESS);
                    }
                }
                break;

                case EV_KEY_RELEASED: // Released
                {
                    if (keyEvent.code == KEY_LEFTCTRL)
                    {
                        LEFT_CTRL = GLUS_FALSE;
                    }
                    else if (keyEvent.code == KEY_RIGHTCTRL)
                    {
                        RIGHT_CTRL = GLUS_FALSE;
                    }

                    _glusWindowInternalKey(glusOsTranslateKey(keyEvent.code), GLFW_RELEASE);
                }
                break;
                }
            }
        } while (numBytes > 0);
    }

    if (g_touchFileDescriptor >= 0)
    {
        struct input_event touchEvent;

        ssize_t numBytes;

        int submitPosition = 0;

        do
        {
            numBytes = read(g_touchFileDescriptor, &touchEvent, sizeof(struct input_event));

            if (numBytes > 0)
            {
                if (touchEvent.type == EV_KEY)
                {
                    if (touchEvent.code == BTN_TOUCH)
                    {
                        if (touchEvent.value == EV_KEY_PRESSED)
                        {
                            if (submitPosition)
                            {
                                _glusWindowInternalMouseMove(g_currentX, g_currentY);

                                submitPosition = 0;
                            }

                            _glusWindowInternalMouse(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
                        }
                        else if (touchEvent.value == EV_KEY_RELEASED)
                        {
                            _glusWindowInternalMouse(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
                        }
                    }
                }
                else if (touchEvent.type == EV_ABS)
                {
                    if (touchEvent.code == ABS_MT_POSITION_X)
                    {
                        g_currentX = g_displayWidth * touchEvent.value / TOUCH_AXIS_MAX;

                        submitPosition = 1;
                    }
                    else if (touchEvent.code == ABS_MT_POSITION_Y)
                    {
                        g_currentY = g_displayHeight * touchEvent.value / TOUCH_AXIS_MAX;

                        submitPosition = 1;
                    }
                    else if ((touchEvent.code == ABS_X || touchEvent.code == ABS_Y) && submitPosition)
                    {
                        _glusWindowInternalMouseMove(g_currentX, g_currentY);

                        submitPosition = 0;
                    }
                }
            }
        } while (numBytes > 0);
    }

    if (g_powermateFileDescriptor >= 0)
    {
        struct input_event powermateEvent;

        ssize_t numBytes;

        do
        {
            numBytes = read(g_powermateFileDescriptor, &powermateEvent, sizeof(struct input_event));

            if (numBytes > 0)
            {
                if (powermateEvent.type == EV_KEY)
                {
                    if (powermateEvent.code == BTN_0)
                    {
                        if (powermateEvent.value == EV_KEY_PRESSED)
                        {
                            _glusWindowInternalMouse(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS);
                        }
                        else if (powermateEvent.value == EV_KEY_RELEASED)
                        {
                            _glusWindowInternalMouse(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE);
                        }
                    }
                }
                else if (powermateEvent.type == EV_REL)
                {
                    if (powermateEvent.code == REL_DIAL)
                    {
                        static int mouseWheel = 0;

                        mouseWheel += powermateEvent.value;

                        _glusWindowInternalMouseWheel(mouseWheel);
                    }
                }
            }
        } while (numBytes > 0);
    }
}

static GLUSvoid glusOsEvdevTerminate()
{
    if (g_keyFileDescriptor >= 0)
    {
        close(g_keyFileDescriptor);

        g_keyFileDescriptor = -1;

        // Enable echo.
        system("stty echo");
    }

    if (g_touchFileDescriptor >= 0)
    {
        close(g_touchFileDescriptor);

        g_touchFileDescriptor = -1;

        g_displayWidth = -1;

        g_displayHeight = -1;

        g_currentX = -1;

        g_currentY = -1;

        g_pressed = 0;
    }

    if (g_powermateFileDescriptor >= 0)
    {
        close(g_powermateFileDescriptor);

        g_powermateFileDescriptor = -1;
    }
}

#endif /* GLUS_OS_LINUX_EVDEV_H */
