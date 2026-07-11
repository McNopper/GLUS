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

#ifndef GLUS_PLANE_H_
#define GLUS_PLANE_H_

/**
 * Copies a plane.
 *
 * @param result Destination plane.
 * @param plane Source plane.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPlaneCopyf(GLUSfloat result[4], const GLUSfloat plane[4]);

/**
 * Creates a plane. The formula is Ax + By + Cz + D = 0, where A,B,C and D are stored in the resulting array.
 * @see Mathematics For 3D Game Programming & Computer Graphics, Second Edition, Page105
 *
 * @param result The calculated plane.
 * @param point Any point on the plane.
 * @param normal The normal vector of the plane. The vector will be normalized during calculation.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPlaneCreatef(GLUSfloat result[4], const GLUSfloat point[4], const GLUSfloat normal[3]);

/**
 * Calculates the signed distance from a plane to a point. If the value is positive, the point is on the side the normal is directing to.
 *
 * @param plane The used plane.
 * @param point The used point.
 *
 * @return The signed distance.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusPlaneDistancePoint4f(const GLUSfloat plane[4], const GLUSfloat point[4]);

/**
 * Calculates a point on the given plane, located around the origin.
 *
 * @param point The resulting point.
 * @param plane The used plane.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPlaneGetPoint4f(GLUSfloat point[4], const GLUSfloat plane[4]);

#endif /* GLUS_PLANE_H_ */
