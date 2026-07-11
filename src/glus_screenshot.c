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

GLUSboolean GLUSAPIENTRY glusScreenshotUseTga(GLUSint x, GLUSint y, const GLUStgaimage* screenshot)
{
    if (!screenshot)
    {
        return GLUS_FALSE;
    }

    if (x < 0 || y < 0 || screenshot->width < 1 || screenshot->height < 1 || screenshot->depth != 1 || screenshot->format != GLUS_RGBA || screenshot->data == 0)
    {
        return GLUS_FALSE;
    }

    glBindFramebuffer(GLUS_FRAMEBUFFER, 0);

    glFlush();

    glPixelStorei(GLUS_PACK_ALIGNMENT, 1);
    glPixelStorei(GLUS_UNPACK_ALIGNMENT, 1);

    glReadPixels(x, y, screenshot->width, screenshot->height, GLUS_RGBA, GLUS_UNSIGNED_BYTE, screenshot->data);

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusScreenshotCreateTga(GLUSint x, GLUSint y, GLUSsizei width, GLUSsizei height, GLUStgaimage* screenshot)
{
    if (!screenshot)
    {
        return GLUS_FALSE;
    }

    screenshot->data = (GLUSubyte*)glusMemoryMalloc(width * height * 4);
    if (!screenshot->data)
    {
        return GLUS_FALSE;
    }
    screenshot->format = GLUS_RGBA;
    screenshot->width  = width;
    screenshot->height = height;
    screenshot->depth  = 1;

    return glusScreenshotUseTga(x, y, screenshot);
}
