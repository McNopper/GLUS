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

#ifndef GLUS_SCREENSHOT_H_
#define GLUS_SCREENSHOT_H_

/**
 * Creates a screenshot of the current back buffer and uses the given image and its dimension.
 *
 * @param x The lower left x coordinate from the screenshot rectangle.
 * @param y The lower left y coordinate from the screenshot rectangle.
 * @param screenshot The image structure to store the pixel data.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusScreenshotUseTga(GLUSint x, GLUSint y, const GLUStgaimage* screenshot);

/**
 * Creates a screenshot of the current back buffer and creates the image automatically.
 *
 * @param x The lower left x coordinate from the screenshot rectangle.
 * @param y The lower left y coordinate from the screenshot rectangle.
 * @param width The width of the screenshot rectangle.
 * @param height The height of the screenshot rectangle.
 * @param screenshot The image structure to store the pixel data.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusScreenshotCreateTga(GLUSint x, GLUSint y, GLUSsizei width, GLUSsizei height, GLUStgaimage* screenshot);

#endif /* GLUS_SCREENSHOT_H_ */
