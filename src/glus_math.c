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

GLUSfloat GLUSAPIENTRY glusMathMaxf(const GLUSfloat value0, const GLUSfloat value1)
{
    return value0 > value1 ? value0 : value1;
}

GLUSfloat GLUSAPIENTRY glusMathMinf(const GLUSfloat value0, const GLUSfloat value1)
{
    return value0 < value1 ? value0 : value1;
}

GLUSfloat GLUSAPIENTRY glusMathRadToDegf(const GLUSfloat radians)
{
    return radians * 360.0f / (2.0f * GLUS_PI);
}

GLUSfloat GLUSAPIENTRY glusMathDegToRadf(const GLUSfloat degrees)
{
    return degrees * 2.0f * GLUS_PI / 360.0f;
}

GLUSfloat GLUSAPIENTRY glusMathMixf(const GLUSfloat value0, const GLUSfloat value1, const GLUSfloat t)
{
    return value0 * (1.0f - t) + value1 * t;
}

GLUSfloat GLUSAPIENTRY glusMathClampf(const GLUSfloat value, const GLUSfloat min, const GLUSfloat max)
{
    if (value < min)
    {
        return min;
    }
    if (value > max)
    {
        return max;
    }
    return value;
}

GLUSfloat GLUSAPIENTRY glusMathLengthf(const GLUSfloat x, const GLUSfloat y, const GLUSfloat z)
{
    return sqrtf(x * x + y * y + z * z);
}

GLUSfloat GLUSAPIENTRY glusMathCubicHermitef(const GLUSfloat p0, const GLUSfloat m0, const GLUSfloat p1, const GLUSfloat m1, const GLUSfloat t)
{
    GLUSfloat t2 = t * t;
    GLUSfloat t3 = t2 * t;

    return (2.0f * t3 - 3.0f * t2 + 1.0f) * p0 + (t3 - 2.0f * t2 + t) * m0 + (-2.0f * t3 + 3.0f * t2) * p1 + (t3 - t2) * m1;
}
