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

#ifndef GLUS_SPHERE_H_
#define GLUS_SPHERE_H_

/**
 * Copies a sphere.
 *
 * @param resultCenter Destination center.
 * @param resultRadius Destination radius.
 * @param center	   Source center.
 * @param radius	   Source radius.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusSphereCopyf(GLUSfloat resultCenter[4], GLUSfloat* resultRadius, const GLUSfloat center[4], const GLUSfloat radius);

/**
 * Calculates the signed distance from a sphere to a point. If the value is negative, the point is inside the sphere.
 *
 * @param center The center of the sphere.
 * @param radius The radius of the sphere.
 * @param point	 The used point.
 *
 * @return The signed distance.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusSphereDistancePoint4f(const GLUSfloat center[4], const GLUSfloat radius, const GLUSfloat point[4]);

#endif /* GLUS_SPHERE_H_ */
