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

#ifndef GLUS_PROFILE_H_
#define GLUS_PROFILE_H_

/**
 * Reset FPS profiling.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusProfileResetFPSf();

/**
 * Update FPS profiling and log FPS.
 *
 * @param time Passed time per frame.
 * @param frames Stores passed frames. If this is a null pointer, the output is logged.
 *
 * @return GLUS_TRUE, if one second passed.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusProfileUpdateFPSf(GLUSfloat time, GLUSuint* frames);

#endif /* GLUS_PROFILE_H_ */
