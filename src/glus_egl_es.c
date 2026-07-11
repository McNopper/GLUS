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

#include "GL/glus.h"

EGLBoolean GLUSAPIENTRY glusEGLCreateContext(EGLNativeDisplayType eglNativeDisplayType, EGLDisplay* eglDisplay, EGLConfig* eglConfig, EGLContext* eglContext, const EGLint configAttribList[], const EGLint contextAttribList[])
{
    EGLint     numConfigs;
    EGLint     majorVersion;
    EGLint     minorVersion;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig  config  = 0;
    EGLContext context = EGL_NO_CONTEXT;

    if (!eglDisplay || !eglConfig || !eglContext)
    {
        glusLogPrint(GLUS_LOG_ERROR, "No eglDisplay, eglConfig or eglContext passed");

        return EGL_FALSE;
    }

    // Get Display
    display = eglGetDisplay((EGLNativeDisplayType)eglNativeDisplayType);
    if (display == EGL_NO_DISPLAY)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not get EGL display");

        return EGL_FALSE;
    }

    // Initialize EGL
    if (!eglInitialize(display, &majorVersion, &minorVersion))
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not initialize EGL");

        glusEGLTerminate(&display, 0, 0);

        return EGL_FALSE;
    }

    glusLogPrint(GLUS_LOG_INFO, "EGL version: %d.%d", majorVersion, minorVersion);

    if (!eglBindAPI(GLUS_EGL_API))
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not bind API");

        glusEGLTerminate(&display, 0, 0);

        return EGL_FALSE;
    }

    // Choose config
    if (!eglChooseConfig(display, configAttribList, &config, 1, &numConfigs))
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not choose EGL configuration");

        glusEGLTerminate(&display, 0, 0);

        return EGL_FALSE;
    }

    if (numConfigs == 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "No EGL configuration returned");

        glusEGLTerminate(&display, 0, 0);

        return EGL_FALSE;
    }

    // Create a GL ES or VG context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribList);
    if (context == EGL_NO_CONTEXT)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not create EGL context");

        glusEGLTerminate(&display, 0, 0);

        return EGL_FALSE;
    }

    *eglDisplay = display;
    *eglConfig  = config;
    *eglContext = context;

    return EGL_TRUE;
}

EGLBoolean GLUSAPIENTRY glusEGLCreateWindowSurfaceMakeCurrent(EGLNativeWindowType eglNativeWindowType, EGLDisplay* eglDisplay, EGLConfig* eglConfig, EGLContext* eglContext, EGLSurface* eglSurface, const EGLint* surfaceAttribList)
{
    EGLDisplay surface = EGL_NO_SURFACE;

    if (!eglDisplay || !eglConfig || !eglContext || !eglSurface)
    {
        glusLogPrint(GLUS_LOG_ERROR, "No eglDisplay, eglConfig, eglContext or eglSurface passed");

        return EGL_FALSE;
    }

    // Create a surface
    surface = eglCreateWindowSurface(*eglDisplay, *eglConfig, (EGLNativeWindowType)eglNativeWindowType, surfaceAttribList);
    if (surface == EGL_NO_SURFACE)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not create EGL window surface");

        glusEGLTerminate(eglDisplay, eglContext, 0);

        return EGL_FALSE;
    }

    // Make the context current
    if (!eglMakeCurrent(*eglDisplay, surface, surface, *eglContext))
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not set EGL context as current");

        glusEGLTerminate(eglDisplay, eglContext, 0);

        return EGL_FALSE;
    }

    *eglSurface = surface;

    return EGL_TRUE;
}

GLUSvoid GLUSAPIENTRY glusEGLTerminate(EGLDisplay* eglDisplay, EGLContext* eglContext, EGLSurface* eglSurface)
{
    if (!eglDisplay)
    {
        glusLogPrint(GLUS_LOG_ERROR, "No eglDisplay passed");

        return;
    }

    if (*eglDisplay)
    {
        eglMakeCurrent(*eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (eglSurface && *eglSurface)
        {
            eglDestroySurface(*eglDisplay, *eglSurface);

            *eglSurface = EGL_NO_SURFACE;
        }

        if (eglContext && *eglContext)
        {
            eglDestroyContext(*eglDisplay, *eglContext);

            *eglContext = EGL_NO_CONTEXT;
        }

        eglTerminate(*eglDisplay);

        *eglDisplay = EGL_NO_DISPLAY;
    }
}
