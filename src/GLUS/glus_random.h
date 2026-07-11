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

#ifndef GLUS_RANDOM_H_
#define GLUS_RANDOM_H_

/**
 * Initializes the random generator.
 *
 * @param seed Number for initializing the pseudo-random number generator.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusRandomSetSeed(const GLUSuint seed);

/**
 * Returns a uniform distributed random floating point value in the given range.
 *
 * @param start Smallest possible generated value (inclusive).
 * @param end Largest possible generated value (inclusive).
 *
 * @return The random value.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusRandomUniformf(const GLUSfloat start, const GLUSfloat end);

/**
 * Returns a normal distributed random floating point value.
 *
 * @param mean 				Mean.
 * @param standardDeviation Standard deviation.
 *
 * @return The random value.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusRandomNormalf(const GLUSfloat mean, const GLUSfloat standardDeviation);

/**
 * Samples two floating point values from a Hammersley point set.
 *
 * @param result 	The resulting random values.
 * @param sample	The sample to take. Has to be in the range 0 <= sample < 2^m.
 * @param m			Order m, which allows 2^m samples. Has to be in the range 0 < m <= 32.
 *
 * @return GLUS_TRUE, if sampling was successful.
 **/
GLUSAPI GLUSboolean GLUSAPIENTRY glusRandomHammersleyf(GLUSfloat result[2], const GLUSuint sample, const GLUSubyte m);

#endif /* GLUS_RANDOM_H_ */
