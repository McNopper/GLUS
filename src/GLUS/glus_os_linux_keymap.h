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

#ifndef GLUS_OS_LINUX_KEYMAP_H
#define GLUS_OS_LINUX_KEYMAP_H

#include <linux/input.h>

#include "GLUS/glus_glfw_es.h"

// Maps Linux evdev key codes to the GLUS/GLFW key codes. Shared by the fbdev
// and Wayland OS backends: both receive evdev key codes - the fbdev backend
// reads them from /dev/input/event*, Wayland delivers them in the
// wl_keyboard::key events - and exactly one OS backend is compiled per build,
// so the static function exists once in every binary.

static int glusOsTranslateKey(int key)
{
    switch (key)
    {
    case KEY_SPACE:
        return GLFW_KEY_SPACE;
    case KEY_APOSTROPHE:
        return GLFW_KEY_APOSTROPHE;
    case KEY_COMMA:
        return GLFW_KEY_COMMA;
    case KEY_MINUS:
        return GLFW_KEY_MINUS;
    case KEY_DOT:
        return GLFW_KEY_PERIOD;
    case KEY_SLASH:
        return GLFW_KEY_SLASH;
    case KEY_0:
        return GLFW_KEY_0;
    case KEY_1:
        return GLFW_KEY_1;
    case KEY_2:
        return GLFW_KEY_2;
    case KEY_3:
        return GLFW_KEY_3;
    case KEY_4:
        return GLFW_KEY_4;
    case KEY_5:
        return GLFW_KEY_5;
    case KEY_6:
        return GLFW_KEY_6;
    case KEY_7:
        return GLFW_KEY_7;
    case KEY_8:
        return GLFW_KEY_8;
    case KEY_9:
        return GLFW_KEY_9;
    case KEY_SEMICOLON:
        return GLFW_KEY_SEMICOLON;
    case KEY_EQUAL:
        return GLFW_KEY_EQUAL;
    case KEY_A:
        return GLFW_KEY_A;
    case KEY_B:
        return GLFW_KEY_B;
    case KEY_C:
        return GLFW_KEY_C;
    case KEY_D:
        return GLFW_KEY_D;
    case KEY_E:
        return GLFW_KEY_E;
    case KEY_F:
        return GLFW_KEY_F;
    case KEY_G:
        return GLFW_KEY_G;
    case KEY_H:
        return GLFW_KEY_H;
    case KEY_I:
        return GLFW_KEY_I;
    case KEY_J:
        return GLFW_KEY_J;
    case KEY_K:
        return GLFW_KEY_K;
    case KEY_L:
        return GLFW_KEY_L;
    case KEY_M:
        return GLFW_KEY_M;
    case KEY_N:
        return GLFW_KEY_N;
    case KEY_O:
        return GLFW_KEY_O;
    case KEY_P:
        return GLFW_KEY_P;
    case KEY_Q:
        return GLFW_KEY_Q;
    case KEY_R:
        return GLFW_KEY_R;
    case KEY_S:
        return GLFW_KEY_S;
    case KEY_T:
        return GLFW_KEY_T;
    case KEY_U:
        return GLFW_KEY_U;
    case KEY_V:
        return GLFW_KEY_V;
    case KEY_W:
        return GLFW_KEY_W;
    case KEY_X:
        return GLFW_KEY_X;
    case KEY_Y:
        return GLFW_KEY_Y;
    case KEY_Z:
        return GLFW_KEY_Z;
    case KEY_LEFTBRACE:
        return GLFW_KEY_LEFT_BRACKET;
    case KEY_BACKSLASH:
        return GLFW_KEY_BACKSLASH;
    case KEY_RIGHTBRACE:
        return GLFW_KEY_RIGHT_BRACKET;
    case KEY_GRAVE:
        return GLFW_KEY_GRAVE_ACCENT;
    case KEY_ESC:
        return GLFW_KEY_ESCAPE;
    case KEY_F1:
        return GLFW_KEY_F1;
    case KEY_F2:
        return GLFW_KEY_F2;
    case KEY_F3:
        return GLFW_KEY_F3;
    case KEY_F4:
        return GLFW_KEY_F4;
    case KEY_F5:
        return GLFW_KEY_F5;
    case KEY_F6:
        return GLFW_KEY_F6;
    case KEY_F7:
        return GLFW_KEY_F7;
    case KEY_F8:
        return GLFW_KEY_F8;
    case KEY_F9:
        return GLFW_KEY_F9;
    case KEY_F10:
        return GLFW_KEY_F10;
    case KEY_F11:
        return GLFW_KEY_F11;
    case KEY_F12:
        return GLFW_KEY_F12;
    case KEY_F13:
        return GLFW_KEY_F13;
    case KEY_F14:
        return GLFW_KEY_F14;
    case KEY_F15:
        return GLFW_KEY_F15;
    case KEY_UP:
        return GLFW_KEY_UP;
    case KEY_DOWN:
        return GLFW_KEY_DOWN;
    case KEY_LEFT:
        return GLFW_KEY_LEFT;
    case KEY_RIGHT:
        return GLFW_KEY_RIGHT;
    case KEY_LEFTSHIFT:
        return GLFW_KEY_LEFT_SHIFT;
    case KEY_RIGHTSHIFT:
        return GLFW_KEY_RIGHT_SHIFT;
    case KEY_LEFTCTRL:
        return GLFW_KEY_LEFT_CONTROL;
    case KEY_RIGHTCTRL:
        return GLFW_KEY_RIGHT_CONTROL;
    case KEY_LEFTALT:
        return GLFW_KEY_LEFT_ALT;
    case KEY_RIGHTALT:
        return GLFW_KEY_RIGHT_ALT;
    case KEY_TAB:
        return GLFW_KEY_TAB;
    case KEY_ENTER:
        return GLFW_KEY_ENTER;
    case KEY_BACKSPACE:
        return GLFW_KEY_BACKSPACE;
    case KEY_INSERT:
        return GLFW_KEY_INSERT;
    case KEY_DELETE:
        return GLFW_KEY_DELETE;
    case KEY_PAGEUP:
        return GLFW_KEY_PAGE_UP;
    case KEY_PAGEDOWN:
        return GLFW_KEY_PAGE_DOWN;
    case KEY_HOME:
        return GLFW_KEY_HOME;
    case KEY_END:
        return GLFW_KEY_END;
    case KEY_KP0:
        return GLFW_KEY_KP_0;
    case KEY_KP1:
        return GLFW_KEY_KP_1;
    case KEY_KP2:
        return GLFW_KEY_KP_2;
    case KEY_KP3:
        return GLFW_KEY_KP_3;
    case KEY_KP4:
        return GLFW_KEY_KP_4;
    case KEY_KP5:
        return GLFW_KEY_KP_5;
    case KEY_KP6:
        return GLFW_KEY_KP_6;
    case KEY_KP7:
        return GLFW_KEY_KP_7;
    case KEY_KP8:
        return GLFW_KEY_KP_8;
    case KEY_KP9:
        return GLFW_KEY_KP_9;
    case KEY_KPSLASH:
        return GLFW_KEY_KP_DIVIDE;
    case KEY_KPASTERISK:
        return GLFW_KEY_KP_MULTIPLY;
    case KEY_KPMINUS:
        return GLFW_KEY_KP_SUBTRACT;
    case KEY_KPPLUS:
        return GLFW_KEY_KP_ADD;
    case KEY_KPDOT:
        return GLFW_KEY_KP_DECIMAL;
    case KEY_KPEQUAL:
        return GLFW_KEY_KP_EQUAL;
    case KEY_KPENTER:
        return GLFW_KEY_KP_ENTER;
    case KEY_NUMLOCK:
        return GLFW_KEY_NUM_LOCK;
    case KEY_CAPSLOCK:
        return GLFW_KEY_CAPS_LOCK;
    case KEY_SCROLLLOCK:
        return GLFW_KEY_SCROLL_LOCK;
    case KEY_PAUSE:
        return GLFW_KEY_PAUSE;
    case KEY_LEFTMETA:
        return GLFW_KEY_LEFT_SUPER;
    case KEY_RIGHTMETA:
        return GLFW_KEY_RIGHT_SUPER;
    case KEY_MENU:
        return GLFW_KEY_MENU;
    default:
        return key;
    }

    return GLFW_KEY_UNKNOWN;
}

#endif /* GLUS_OS_LINUX_KEYMAP_H */
