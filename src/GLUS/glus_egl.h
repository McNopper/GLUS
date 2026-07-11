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

#ifndef GLUS_EGL_H_
#define GLUS_EGL_H_

/**
 * Creates the context and all needed EGL elements.
 *
 * @param eglNativeDisplayType		Native display type.
 * @param eglDisplay  				EGL display.
 * @param eglConfig  				EGL configuration.
 * @param eglContext 				EGL context.
 * @param configAttribList 			EGL configuration attribute list.
 * @param contextAttribList 		EGL context attribute list.
 *
 * @return EGL_TRUE, when creation of context succeeded.
 */
GLUSAPI EGLBoolean GLUSAPIENTRY glusEGLCreateContext(EGLNativeDisplayType eglNativeDisplayType, EGLDisplay* eglDisplay, EGLConfig* eglConfig, EGLContext* eglContext, const EGLint configAttribList[], const EGLint contextAttribList[]);

/**
 * Creates a window surface and sets it as current.
 *
 * @param eglNativeWindowType   	Native window type.
 * @param eglDisplay 				EGL display.
 * @param eglConfig  				EGL configuration.
 * @param eglContext 				EGL context.
 * @param eglSurface 				EGL surface.
 *
 * @return EGL_TRUE, when creation of window and setting of context succeeded.
 */
GLUSAPI EGLBoolean GLUSAPIENTRY glusEGLCreateWindowSurfaceMakeCurrent(EGLNativeWindowType eglNativeWindowType, EGLDisplay* eglDisplay, EGLConfig* eglConfig, EGLContext* eglContext, EGLSurface* eglSurface, const EGLint* surfaceAttribList);

/**
 * Returns the created default EGL display.
 *
 * @return the EGL display.
 */
GLUSAPI EGLDisplay GLUSAPIENTRY glusEGLGetDefaultDisplay(GLUSvoid);

/**
 * Returns the created default EGL surface.
 *
 * @return the EGL surface.
 */
GLUSAPI EGLSurface GLUSAPIENTRY glusEGLGetDefaultSurface(GLUSvoid);

/**
 * Returns the created default EGL context.
 *
 * @return the EGL context.
 */
GLUSAPI EGLContext GLUSAPIENTRY glusEGLGetDefaultContext(GLUSvoid);

/**
 * Terminates EGL an frees all resources.
 *
 * @param eglDisplay EGL display.
 * @param eglContext EGL context.
 * @param eglSurface EGL surface.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusEGLTerminate(EGLDisplay* eglDisplay, EGLContext* eglContext, EGLSurface* eglSurface);

#endif /* GLUS_EGL_H_ */
