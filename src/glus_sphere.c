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

GLUSvoid GLUSAPIENTRY glusSphereCopyf(GLUSfloat resultCenter[4], GLUSfloat* resultRadius, const GLUSfloat center[4], const GLUSfloat radius)
{
    if (!resultCenter || !resultRadius || !center)
    {
        return;
    }

    resultCenter[0] = center[0];
    resultCenter[1] = center[1];
    resultCenter[2] = center[2];
    resultCenter[3] = center[3];

    *resultRadius = radius;
}

GLUSfloat GLUSAPIENTRY glusSphereDistancePoint4f(const GLUSfloat center[4], const GLUSfloat radius, const GLUSfloat point[4])
{
    return glusPoint4Distancef(point, center) - radius;
}
