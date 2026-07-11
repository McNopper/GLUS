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

#ifndef GLUS_COMPLEX_H_
#define GLUS_COMPLEX_H_

/**
 * Calculates the absolute of a complex number.
 *
 * @param complex   The complex number from which the absolute is calculated.
 *
 * @result The resulting value.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusComplexAbsolutec(const GLUScomplex* complex);

/**
 * Calculates the complex conjugate.
 *
 * @param result	The resulting complex conjugate.
 * @param complex   The complex number from which the conjugate is calculated.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexConjugatec(GLUScomplex* result, const GLUScomplex* complex);

/**
 * Calculates the sum of two complex numbers.
 *
 * @param result	The resulting complex number.
 * @param complex0  First complex number.
 * @param complex1  Second complex number.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexAddComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1);

/**
 * Calculates the difference of two complex numbers.
 *
 * @param result	The resulting complex number.
 * @param complex0  First complex number.
 * @param complex1  Second complex number.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexSubtractComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1);

/**
 * Multiplies two complex numbers.
 *
 * @param result	The resulting complex number.
 * @param complex0  First complex number.
 * @param complex1  Second complex number.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexMultiplyComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1);

/**
 * Divides two complex numbers.
 *
 * @param result	The resulting complex number.
 * @param complex0  First complex number.
 * @param complex1  Second complex number.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexDivideComplexc(GLUScomplex* result, const GLUScomplex* complex0, const GLUScomplex* complex1);

/**
 * Multiplies a complex number with a real number.
 *
 * @param result	The resulting complex number.
 * @param complex	Complex number.
 * @param scalar  	The scalar to multiply with.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexMultiplyScalarc(GLUScomplex* result, const GLUScomplex* complex, const GLUSfloat scalar);

/**
 * Calculates the exponential of complex number.
 *
 * @param result	The resulting complex number.
 * @param complex	Complex number.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusComplexExpComplexc(GLUScomplex* result, const GLUScomplex* complex);

/**
 * Calculates the root of unity.
 *
 * @param result The root of unity.
 * @param n		 The nth root of unity.
 * @param k		 k element. Maybe negative.
 * @param dir 	 The direction. Negative is clockwise.
 */
GLUSAPI GLUSvoid glusComplexRootOfUnityc(GLUScomplex* result, const GLUSint n, const GLUSint k, const GLUSfloat dir);

#endif /* GLUS_COMPLEX_H_ */
