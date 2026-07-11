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

GLUSfloat GLUSAPIENTRY glusComplexAbsolutec(const GLUScomplex* complex)
{
    return sqrtf(complex->real * complex->real + complex->imaginary * complex->imaginary);
}

GLUSvoid GLUSAPIENTRY glusComplexConjugatec(GLUScomplex* result, const GLUScomplex* complex)
{
    result->real      = complex->real;
    result->imaginary = -complex->imaginary;
}

GLUSvoid GLUSAPIENTRY glusComplexAddComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1)
{
    result->real      = complex0->real + complex1->real;
    result->imaginary = complex0->imaginary + complex1->imaginary;
}

GLUSvoid GLUSAPIENTRY glusComplexSubtractComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1)
{
    result->real      = complex0->real - complex1->real;
    result->imaginary = complex0->imaginary - complex1->imaginary;
}

GLUSvoid GLUSAPIENTRY glusComplexMultiplyComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1)
{
    GLUScomplex temp;

    temp.real      = complex0->real * complex1->real - complex0->imaginary * complex1->imaginary;
    temp.imaginary = complex0->real * complex1->imaginary + complex1->real * complex0->imaginary;

    result->real      = temp.real;
    result->imaginary = temp.imaginary;
}

GLUSvoid GLUSAPIENTRY glusComplexDivideComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1)
{
    GLUScomplex temp;

    GLUSfloat divisor = complex1->real * complex1->real + complex1->imaginary * complex1->imaginary;

    temp.real      = (complex0->real * complex1->real + complex0->imaginary * complex1->imaginary) / divisor;
    temp.imaginary = (complex1->real * complex0->imaginary - complex0->real * complex1->imaginary) / divisor;

    result->real      = temp.real;
    result->imaginary = temp.imaginary;
}

GLUSvoid GLUSAPIENTRY glusComplexMultiplyScalarc(GLUScomplex* result, const GLUScomplex* complex, const GLUSfloat scalar)
{
    result->real      = complex->real * scalar;
    result->imaginary = complex->imaginary * scalar;
}

GLUSvoid GLUSAPIENTRY glusComplexExpComplexc(GLUScomplex* result, const GLUScomplex* complex)
{
    GLUScomplex temp;

    temp.real      = expf(complex->real) * cosf(complex->imaginary);
    temp.imaginary = expf(complex->real) * sinf(complex->imaginary);

    result->real      = temp.real;
    result->imaginary = temp.imaginary;
}

GLUSvoid glusComplexRootOfUnityc(GLUScomplex* result, const GLUSint n, const GLUSint k, const GLUSfloat dir)
{
    result->real      = cosf(2.0f * GLUS_PI * (GLUSfloat)k / (GLUSfloat)n);
    result->imaginary = dir * sinf(2.0f * GLUS_PI * (GLUSfloat)k / (GLUSfloat)n);
}
