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

#ifndef GLUS_POINT_H_
#define GLUS_POINT_H_

/**
 * Copies a 3D point, given as homogeneous coordinates.
 *
 * @param result The destination point.
 * @param point The source point.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint4Copyf(GLUSfloat result[4], const GLUSfloat point[4]);

/**
 * Copies a 2D point, given as homogeneous coordinates.
 *
 * @param result The destination point.
 * @param point The source point.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint3Copyf(GLUSfloat result[3], const GLUSfloat point[3]);

/**
 * Subtracts a 3D point, given as homogeneous coordinates, from another and calculates a 3D vector.
 *
 * @param result The resulting vector.
 * @param point0 The point subtracted by point1.
 * @param point1 The point subtracted from point0.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint4SubtractPoint4f(GLUSfloat result[3], const GLUSfloat point0[4], const GLUSfloat point1[4]);

/**
 * Subtracts a 2D point, given as homogeneous coordinates, from another and calculates a 2D vector.
 *
 * @param result The resulting vector.
 * @param point0 The point subtracted by point1.
 * @param point1 The point subtracted from point0.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint3SubtractPoint3f(GLUSfloat result[2], const GLUSfloat point0[3], const GLUSfloat point1[3]);

/**
 * Adds a vector to a 3D point, given as homogeneous coordinates. Result is the new point.
 *
 * @param result The resulting point.
 * @param point The point.
 * @param vector The vector, which is added to the point.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint4AddVector3f(GLUSfloat result[4], const GLUSfloat point[4], const GLUSfloat vector[3]);

/**
 * Adds a vector to a 2D point, given as homogeneous coordinates. Result is the new point.
 *
 * @param result The resulting point.
 * @param point The point.
 * @param vector The vector, which is added to the point.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint3AddVector2f(GLUSfloat result[3], const GLUSfloat point[3], const GLUSfloat vector[2]);

/**
 * Subtracts a vector from a 3D point, given as homogeneous coordinates. Result is the new point.
 *
 * @param result The resulting point.
 * @param point The point.
 * @param vector The vector, which is subtracted from the point.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint4SubtractVector3f(GLUSfloat result[4], const GLUSfloat point[4], const GLUSfloat vector[3]);

/**
 * Subtracts a vector from a 2D point, given as homogeneous coordinates. Result is the new point.
 *
 * @param result The resulting point.
 * @param point The point.
 * @param vector The vector, which is subtracted from the point.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint3SubtractVector2f(GLUSfloat result[3], const GLUSfloat point[3], const GLUSfloat vector[2]);

/**
 * Converts a 3D point, given as homogeneous coordinates, to a quaternion.
 *
 * @param result The resulting quaternion.
 * @param point The point, which is converted.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint4GetQuaternionf(GLUSfloat result[4], const GLUSfloat point[4]);

/**
 * Converts a 3D point, given as homogeneous coordinates, to a vector.
 *
 * @param result The resulting vector.
 * @param point The point, which is converted.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint4GetVector3f(GLUSfloat result[3], const GLUSfloat point[4]);

/**
 * Converts a 2D point, given as homogeneous coordinates, to a vector.
 *
 * @param result The resulting vector.
 * @param point The point, which is converted.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusPoint3GetVector2f(GLUSfloat result[2], const GLUSfloat point[3]);

/**
 * Calculates the distance of two 3D points, given as homogeneous coordinates.
 *
 * @param point0 The first point.
 * @param point1 The second point.
 *
 * @return The distance of the two points.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusPoint4Distancef(const GLUSfloat point0[4], const GLUSfloat point1[4]);

/**
 * Calculates the distance of two 2D points, given as homogeneous coordinates.
 *
 * @param point0 The first point.
 * @param point1 The second point.
 *
 * @return The distance of the two points.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusPoint3Distancef(const GLUSfloat point0[3], const GLUSfloat point1[3]);

#endif /* GLUS_POINT_H_ */
