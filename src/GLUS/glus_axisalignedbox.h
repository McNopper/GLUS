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

#ifndef GLUS_AXIS_ALIGNED_BOX_H_
#define GLUS_AXIS_ALIGNED_BOX_H_

/**
 * Copies an axis aligned box.
 *
 * @param resultCenter	   Destination center.
 * @param resultHalfExtend Destination half extend.
 * @param center	   	   Source center.
 * @param halfExtend	   Source half extend.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusAxisAlignedBoxCopyf(GLUSfloat resultCenter[4], GLUSfloat resultHalfExtend[3], const GLUSfloat center[4], const GLUSfloat halfExtend[3]);

/**
 * Calculates the signed distance from an axis aligned box to a point. If the value is negative, the point is inside the box.
 *
 * @param center	 The center of the box.
 * @param halfExtend The length from the center point to the planes of the box.
 * @param point		 The used point.
 *
 * @return The signed distance.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusAxisAlignedBoxDistancePoint4f(const GLUSfloat center[4], const GLUSfloat halfExtend[3], const GLUSfloat point[4]);

#endif /* GLUS_AXIS_ALIGNED_BOX_H_ */
