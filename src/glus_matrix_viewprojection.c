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

GLUSboolean GLUSAPIENTRY glusMatrix4x4Orthof(GLUSfloat result[16], const GLUSfloat left, const GLUSfloat right, const GLUSfloat bottom, const GLUSfloat top, const GLUSfloat nearVal, const GLUSfloat farVal)
{
    if ((right - left) == 0.0f || (top - bottom) == 0.0f || (farVal - nearVal) == 0.0f)
    {
        return GLUS_FALSE;
    }

    result[0]  = 2.0f / (right - left);
    result[1]  = 0.0f;
    result[2]  = 0.0f;
    result[3]  = 0.0f;
    result[4]  = 0.0f;
    result[5]  = 2.0f / (top - bottom);
    result[6]  = 0.0f;
    result[7]  = 0.0f;
    result[8]  = 0.0f;
    result[9]  = 0.0f;
    result[10] = -2.0f / (farVal - nearVal);
    result[11] = 0.0f;
    result[12] = -(right + left) / (right - left);
    result[13] = -(top + bottom) / (top - bottom);
    result[14] = -(farVal + nearVal) / (farVal - nearVal);
    result[15] = 1.0f;

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusMatrix4x4Frustumf(GLUSfloat result[16], const GLUSfloat left, const GLUSfloat right, const GLUSfloat bottom, const GLUSfloat top, const GLUSfloat nearVal, const GLUSfloat farVal)
{
    if ((right - left) == 0.0f || (top - bottom) == 0.0f || (farVal - nearVal) == 0.0f)
    {
        return GLUS_FALSE;
    }

    result[0]  = 2.0f * nearVal / (right - left);
    result[1]  = 0.0f;
    result[2]  = 0.0f;
    result[3]  = 0.0f;
    result[4]  = 0.0f;
    result[5]  = 2.0f * nearVal / (top - bottom);
    result[6]  = 0.0f;
    result[7]  = 0.0f;
    result[8]  = (right + left) / (right - left);
    result[9]  = (top + bottom) / (top - bottom);
    result[10] = -(farVal + nearVal) / (farVal - nearVal);
    result[11] = -1.0f;
    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = -(2.0f * farVal * nearVal) / (farVal - nearVal);
    result[15] = 0.0f;

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusMatrix4x4Perspectivef(GLUSfloat result[16], const GLUSfloat fovy, const GLUSfloat aspect, const GLUSfloat zNear, const GLUSfloat zFar)
{
    GLUSfloat xmin, xmax, ymin, ymax;

    if (fovy <= 0.0f || fovy >= 180.0f)
    {
        return GLUS_FALSE;
    }

    ymax = zNear * tanf(fovy * GLUS_PI / 360.0f);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    return glusMatrix4x4Frustumf(result, xmin, xmax, ymin, ymax, zNear, zFar);
}

GLUSvoid GLUSAPIENTRY glusMatrix4x4LookAtf(GLUSfloat result[16], const GLUSfloat eyeX, const GLUSfloat eyeY, const GLUSfloat eyeZ, const GLUSfloat centerX, const GLUSfloat centerY, const GLUSfloat centerZ, const GLUSfloat upX, const GLUSfloat upY, const GLUSfloat upZ)
{
    GLUSfloat forward[3], side[3], up[3];

    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;

    glusVector3Normalizef(forward);

    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;

    glusVector3Crossf(side, forward, up);
    glusVector3Normalizef(side);

    glusVector3Crossf(up, side, forward);

    result[0]  = side[0];
    result[1]  = up[0];
    result[2]  = -forward[0];
    result[3]  = 0.0f;
    result[4]  = side[1];
    result[5]  = up[1];
    result[6]  = -forward[1];
    result[7]  = 0.0f;
    result[8]  = side[2];
    result[9]  = up[2];
    result[10] = -forward[2];
    result[11] = 0.0f;
    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = 0.0f;
    result[15] = 1.0f;

    glusMatrix4x4Translatef(result, -eyeX, -eyeY, -eyeZ);
}
