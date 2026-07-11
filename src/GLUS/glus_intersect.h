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

#ifndef GLUS_INTERSECT_H_
#define GLUS_INTERSECT_H_

/**
 * Intersecting ray against sphere.
 *
 * @param tNear			t of near intersection point if number intersections greater than zero.
 * @param tFar			t of far intersection point if number intersections greater than zero.
 * @param insideSphere	Set to GLUS_TRUE, if ray starts inside sphere.
 * @param rayStart		Point, where the ray starts.
 * @param rayDirection	Ray direction vector. Has to be normalized.
 * @param sphereCenter	Center of the sphere given as a point.
 * @param radius		Radius of the sphere.
 *
 * @return Number of intersection points.
 */
GLUSAPI GLUSint GLUSAPIENTRY glusIntersectRaySpheref(GLUSfloat* tNear, GLUSfloat* tFar, GLUSboolean* insideSphere, const GLUSfloat rayStart[4], const GLUSfloat rayDirection[3], const GLUSfloat sphereCenter[4], const GLUSfloat radius);

#endif /* GLUS_INTERSECT_H_ */
