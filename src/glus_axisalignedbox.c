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

GLUSvoid GLUSAPIENTRY glusAxisAlignedBoxCopyf(GLUSfloat resultCenter[4], GLUSfloat resultHalfExtend[3], const GLUSfloat center[4], const GLUSfloat halfExtend[3])
{
    resultCenter[0] = center[0];
    resultCenter[1] = center[1];
    resultCenter[2] = center[2];
    resultCenter[3] = center[3];

    resultHalfExtend[0] = halfExtend[0];
    resultHalfExtend[1] = halfExtend[1];
    resultHalfExtend[2] = halfExtend[2];
}

GLUSfloat GLUSAPIENTRY glusAxisAlignedBoxDistancePoint4f(const GLUSfloat center[4], const GLUSfloat halfExtend[3], const GLUSfloat point[4])
{
    GLUSfloat vector[3];

    GLUSfloat insideDistance;
    GLUSfloat outsideDistance;

    glusPoint4SubtractPoint4f(vector, point, center);

    vector[0] = fabsf(vector[0]) - halfExtend[0];
    vector[1] = fabsf(vector[1]) - halfExtend[1];
    vector[2] = fabsf(vector[2]) - halfExtend[2];

    insideDistance = glusMathMinf(glusMathMaxf(vector[0], glusMathMaxf(vector[1], vector[2])), 0.0f);

    vector[0] = glusMathMaxf(vector[0], 0.0f);
    vector[1] = glusMathMaxf(vector[1], 0.0f);
    vector[2] = glusMathMaxf(vector[2], 0.0f);

    outsideDistance = glusVector3Lengthf(vector);

    return insideDistance + outsideDistance;
}
