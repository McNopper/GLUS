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

GLUSvoid GLUSAPIENTRY glusPlaneCopyf(GLUSfloat result[4], const GLUSfloat plane[4])
{
    result[0] = plane[0];
    result[1] = plane[1];
    result[2] = plane[2];
    result[3] = plane[3];
}

GLUSvoid GLUSAPIENTRY glusPlaneCreatef(GLUSfloat result[4], const GLUSfloat point[4], const GLUSfloat normal[3])
{
    GLUSfloat normalized[3];

    glusVector3Copyf(normalized, normal);
    glusVector3Normalizef(normalized);

    result[0] = normalized[0];
    result[1] = normalized[1];
    result[2] = normalized[2];

    // D = -N*P
    result[3] = -glusVector3Dotf(normalized, point);
}

GLUSfloat GLUSAPIENTRY glusPlaneDistancePoint4f(const GLUSfloat plane[4], const GLUSfloat point[4])
{
    return glusVector3Dotf(plane, point) + plane[3];
}

GLUSvoid GLUSAPIENTRY glusPlaneGetPoint4f(GLUSfloat point[4], const GLUSfloat plane[4])
{
    point[0] = 0.0f;
    point[1] = 0.0f;
    point[2] = 0.0f;
    point[3] = 1.0f;

    if (plane[0] != 0.0f)
    {
        point[0] = -plane[3] / plane[0];
    }
    else if (plane[1] != 0.0f)
    {
        point[1] = -plane[3] / plane[1];
    }
    else if (plane[2] != 0.0f)
    {
        point[2] = -plane[3] / plane[2];
    }
}
