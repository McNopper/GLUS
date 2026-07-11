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

#ifndef GLUS_MATH_H_
#define GLUS_MATH_H_

/**
 * Calculates the maximum of two values.
 *
 * @param value0 The first value.
 * @param value1 The second value.
 *
 * @return The maximum of the two values.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathMaxf(const GLUSfloat value0, const GLUSfloat value1);

/**
 * Calculates the minimum of two values.
 *
 * @param value0 The first value.
 * @param value1 The second value.
 *
 * @return The minimum of the two values.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathMinf(const GLUSfloat value0, const GLUSfloat value1);

/**
 * Converts radians to degrees.
 *
 * @param radians The angle in radians.
 *
 * @return The angle in degrees.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathRadToDegf(const GLUSfloat radians);

/**
 * Converts degrees to radians.
 *
 * @param degrees The angle in degrees.
 *
 * @return The angle in radians.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathDegToRadf(const GLUSfloat degrees);

/**
 * Linear interpolation of two values. If t = 0, value0 is returned. If t = 1, value1 is returned.
 *
 * @param value0 First value.
 * @param value1 Second value.
 * @param t The fraction.
 *
 * @return The interpolated value.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathMixf(const GLUSfloat value0, const GLUSfloat value1, const GLUSfloat t);

/**
 * Clamps a given value between a minimum and maximum value. If the value is between min and max, the value is returned.
 *
 * @param value The value to be clamped.
 * @param min The minimum border. If the value is lower than min, min is returned.
 * @param max The maximum border. If the value is greater than max, max is returned.
 *
 * @return The clamped value.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathClampf(const GLUSfloat value, const GLUSfloat min, const GLUSfloat max);

/**
 * Calculates the length of a vector or the distance of a point to the origin by providing the x, y and z coordinates.
 *
 * @param x The x coordinate.
 * @param y The y coordinate.
 * @param z The z coordinate.
 *
 * @return The calculated length.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathLengthf(const GLUSfloat x, const GLUSfloat y, const GLUSfloat z);

/**
 * Cubic Hermite spline interpolation for a single float value.
 *
 * Given two endpoints p0 and p1 and their associated tangent vectors m0 and m1
 * (each pre-scaled by the interval length delta = t1 - t0), evaluates the
 * Hermite basis at parameter t in [0, 1]:
 *
 *   p(t) = (2t^3 - 3t^2 + 1)*p0  +  (t^3 - 2t^2 + t)*m0
 *        + (-2t^3 + 3t^2)*p1      +  (t^3 - t^2)*m1
 *
 * @param p0 The value at the start of the interval.
 * @param m0 The outgoing tangent at p0, scaled by the interval length.
 * @param p1 The value at the end of the interval.
 * @param m1 The incoming tangent at p1, scaled by the interval length.
 * @param t  The interpolation parameter in [0, 1].
 *
 * @return The interpolated value.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusMathCubicHermitef(const GLUSfloat p0, const GLUSfloat m0, const GLUSfloat p1, const GLUSfloat m1, const GLUSfloat t);

#endif /* GLUS_MATH_H_ */
