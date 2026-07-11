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

GLUSvoid GLUSAPIENTRY glusPoint4Copyf(GLUSfloat result[4], const GLUSfloat point[4])
{
    result[0] = point[0];
    result[1] = point[1];
    result[2] = point[2];
    result[3] = point[3];
}

GLUSvoid GLUSAPIENTRY glusPoint3Copyf(GLUSfloat result[3], const GLUSfloat point[3])
{
    result[0] = point[0];
    result[1] = point[1];
    result[2] = point[2];
}

GLUSvoid GLUSAPIENTRY glusPoint4SubtractPoint4f(GLUSfloat result[3], const GLUSfloat point0[4], const GLUSfloat point1[4])
{
    result[0] = point0[0] - point1[0];
    result[1] = point0[1] - point1[1];
    result[2] = point0[2] - point1[2];
}

GLUSvoid GLUSAPIENTRY glusPoint3SubtractPoint3f(GLUSfloat result[2], const GLUSfloat point0[3], const GLUSfloat point1[3])
{
    result[0] = point0[0] - point1[0];
    result[1] = point0[1] - point1[1];
}

GLUSvoid GLUSAPIENTRY glusPoint4AddVector3f(GLUSfloat result[4], const GLUSfloat point[4], const GLUSfloat vector[3])
{
    result[0] = point[0] + vector[0];
    result[1] = point[1] + vector[1];
    result[2] = point[2] + vector[2];
    result[3] = point[3];
}

GLUSvoid GLUSAPIENTRY glusPoint3AddVector2f(GLUSfloat result[3], const GLUSfloat point[3], const GLUSfloat vector[2])
{
    result[0] = point[0] + vector[0];
    result[1] = point[1] + vector[1];
    result[2] = point[2];
}

GLUSvoid GLUSAPIENTRY glusPoint4SubtractVector3f(GLUSfloat result[4], const GLUSfloat point[4], const GLUSfloat vector[3])
{
    result[0] = point[0] - vector[0];
    result[1] = point[1] - vector[1];
    result[2] = point[2] - vector[2];
    result[3] = point[3];
}

GLUSvoid GLUSAPIENTRY glusPoint3SubtractVector2f(GLUSfloat result[3], const GLUSfloat point[3], const GLUSfloat vector[2])
{
    result[0] = point[0] - vector[0];
    result[1] = point[1] - vector[1];
    result[2] = point[2];
}

GLUSvoid GLUSAPIENTRY glusPoint4GetQuaternionf(GLUSfloat result[4], const GLUSfloat point[4])
{
    result[0] = point[0];
    result[1] = point[1];
    result[2] = point[2];
    result[3] = 0.0f;
}

GLUSvoid GLUSAPIENTRY glusPoint4GetVector3f(GLUSfloat result[3], const GLUSfloat point[4])
{
    result[0] = point[0];
    result[1] = point[1];
    result[2] = point[2];
}

GLUSvoid GLUSAPIENTRY glusPoint3GetVector2f(GLUSfloat result[2], const GLUSfloat point[3])
{
    result[0] = point[0];
    result[1] = point[1];
}

GLUSfloat GLUSAPIENTRY glusPoint4Distancef(const GLUSfloat point0[4], const GLUSfloat point1[4])
{
    GLUSfloat vector[3];

    glusPoint4SubtractPoint4f(vector, point0, point1);

    return glusVector3Lengthf(vector);
}

GLUSfloat GLUSAPIENTRY glusPoint3Distancef(const GLUSfloat point0[3], const GLUSfloat point1[3])
{
    GLUSfloat vector[2];

    glusPoint3SubtractPoint3f(vector, point0, point1);

    return glusVector2Lengthf(vector);
}
