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

#ifndef GLUS_FOURIER_H_
#define GLUS_FOURIER_H_

/**
 * Performs a direct fourier transform on a given vector with N elements.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if transform succeeded.
 */
GLUSAPI GLUSboolean glusFourierDFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

/**
 * Performs an inverse direct fourier transform on a given vector with N elements.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if transform succeeded.
 */
GLUSAPI GLUSboolean glusFourierInverseDFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

/**
 * Performs a fast fourier transform on a given vector with N elements, using a recursive algorithm.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if transform succeeded.
 */
GLUSAPI GLUSboolean glusFourierRecursiveFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

/**
 * Performs an inverse fast fourier transform on a given vector with N elements, using a recursive algorithm.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if transform succeeded.
 */
GLUSAPI GLUSboolean glusFourierRecursiveInverseFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

/**
 * Shuffles a vector with N elements, that it can be used for a FFT butterfly algorithm.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if shuffle succeeded.
 */
GLUSAPI GLUSboolean glusFourierButterflyShuffleFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

/**
 * Shuffles a index vector with N elements, that the indices can be used for a FFT butterfly algorithm.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if shuffle succeeded.
 */
GLUSAPI GLUSboolean glusFourierButterflyShuffleFFTi(GLUSint* result, const GLUSint* vector, const GLUSint n);

/**
 * Performs a fast fourier transform on a given vector with N elements, using a butterfly algorithm.
 * Shuffling of the elements is done in this function.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if transform succeeded.
 */
GLUSAPI GLUSboolean glusFourierButterflyFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

/**
 * Performs an inverse fast fourier transform on a given vector with N elements, using a butterfly algorithm.
 * Shuffling of the elements is done in this function.
 *
 * @param result The transformed vector.
 * @param vector The source vector.
 * @param n 	 The number of elements.
 *
 * @return GLUS_TRUE, if transform succeeded.
 */
GLUSAPI GLUSboolean glusFourierButterflyInverseFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n);

#endif /* GLUS_FOURIER_H_ */
