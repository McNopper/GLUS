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

#ifndef GLUS_QUATERNION_H_
#define GLUS_QUATERNION_H_

/**
 * Creates the identity Quaternion.
 *
 * @param quaternion The resulting identity Quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionIdentityf(GLUSfloat quaternion[4]);

/**
 * Copies a Quaternion.
 *
 * @param result The destination Quaternion.
 * @param quaternion The source Quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionCopyf(GLUSfloat result[4], const GLUSfloat quaternion[4]);

/**
 * Calculates the norm of a Quaternion.
 *
 * @param quaternion The Quaternion to calculate the norm from.
 *
 * @return The norm of the Quaternion.
 */
GLUSAPI GLUSfloat GLUSAPIENTRY glusQuaternionNormf(const GLUSfloat quaternion[4]);

/**
 * Normalized a Quaternion.
 *
 * @param quaternion The Quaternion to normalize.
 *
 * @return GLUS_TRUE, if normalization succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionNormalizef(GLUSfloat quaternion[4]);

/**
 * Adds two Quaternions.
 *
 * @param result The sum of both Quaternions.
 * @param quaternion0 The first Quaternion.
 * @param quaternion1 The second Quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionAddQuaternionf(GLUSfloat result[4], const GLUSfloat quaternion0[4], const GLUSfloat quaternion1[4]);

/**
 * Subtracts two Quaternions: quaternion0 - quaternion1
 *
 * @param result The difference of both Quaternions.
 * @param quaternion0 The first Quaternion.
 * @param quaternion1 The second Quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionSubQuaternionf(GLUSfloat result[4], const GLUSfloat quaternion0[4], const GLUSfloat quaternion1[4]);

/**
 * Multiplies two Quaternions: quaternion0 * quaternion1
 *
 * @param result The multiplied Quaternion.
 * @param quaternion0 The first Quaternion.
 * @param quaternion1 The second Quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionMultiplyQuaternionf(GLUSfloat result[4], const GLUSfloat quaternion0[4], const GLUSfloat quaternion1[4]);

/**
 * Calculates the conjugate of a Quaternion.
 *
 * @param quaternion The Quaternion to conjugate.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionConjugatef(GLUSfloat quaternion[4]);

/**
 * Calculates the inverse of a Quaternion.
 *
 * @param quaternion The Quaternion to invert.
 *
 * @return GLUS_TRUE, if the inverse could be calculated.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionInversef(GLUSfloat quaternion[4]);

/**
 * Calculates the inverse of a unit Quaternion.
 *
 * @param quaternion The Quaternion to invert.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionInverseUnitf(GLUSfloat quaternion[4]);

/**
 * Creates a rotation Quaternion around a given axis.
 *
 * @param quaternion The final quaternion.
 * @param angle The rotation angle in degree.
 * @param x The x coordinate of the axis.
 * @param y The y coordinate of the axis.
 * @param z The z coordinate of the axis.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionRotatef(GLUSfloat quaternion[4], const GLUSfloat angle, const GLUSfloat x, const GLUSfloat y, const GLUSfloat z);

/**
 * Creates a rotation Quaternion around a the x axis.
 *
 * @param quaternion The final quaternion.
 * @param angle The rotation angle in degree.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionRotateRxf(GLUSfloat quaternion[4], const GLUSfloat angle);

/**
 * Creates a rotation Quaternion around a the y axis.
 *
 * @param quaternion The final quaternion.
 * @param angle The rotation angle in degree.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionRotateRyf(GLUSfloat quaternion[4], const GLUSfloat angle);

/**
 * Creates a rotation Quaternion around a the z axis.
 *
 * @param quaternion The final quaternion.
 * @param angle The rotation angle in degree.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionRotateRzf(GLUSfloat quaternion[4], const GLUSfloat angle);

/**
 * Creates a rotation Quaternion out of Euler angels: Rz * Rx * Ry
 *
 * @param quaternion The final quaternion.
 * @param anglez The rotation z angle in degree.
 * @param anglex The rotation x angle in degree.
 * @param angley The rotation y angle in degree.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionRotateRzRxRyf(GLUSfloat quaternion[4], const GLUSfloat anglez, const GLUSfloat anglex, const GLUSfloat angley);

/**
 * Creates a rotation Quaternion out of Euler angels: Rz * Ry * Rx
 *
 * @param quaternion The final quaternion.
 * @param anglez The rotation z angle in degree.
 * @param angley The rotation y angle in degree.
 * @param anglex The rotation x angle in degree.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionRotateRzRyRxf(GLUSfloat quaternion[4], const GLUSfloat anglez, const GLUSfloat angley, const GLUSfloat anglex);

/**
 * Creates a 4x4 matrix out of a Quaternion.
 *
 * @param matrix The resulting matrix.
 * @param quaternion The final quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionGetMatrix4x4f(GLUSfloat matrix[16], const GLUSfloat quaternion[4]);

/**
 * Creates a 3x3 matrix out of a Quaternion.
 *
 * @param matrix The resulting matrix.
 * @param quaternion The final Quaternion.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionGetMatrix3x3f(GLUSfloat matrix[9], const GLUSfloat quaternion[4]);

/**
 * Creates a 3D Point, given as homogeneous coordinates, out of a Quaternion.
 *
 * @param point The resulting point.
 * @param quaternion The used Quaternion.
 *
 * @return GLUS_TRUE, if a point could be created.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionGetPoint4f(GLUSfloat point[4], const GLUSfloat quaternion[4]);

/**
 * Creates a 3D Vector, out of a Quaternion.
 *
 * @param vector The resulting vecotor.
 * @param quaternion The used Quaternion.
 *
 * @return GLUS_TRUE, if a vector could be created.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionGetVector3f(GLUSfloat vector[3], const GLUSfloat quaternion[4]);

/**
 * Extracts the Euler angles from a Quaternion, created by Rz * Rx * Ry.
 *
 * @param angles Resulting anglex (Pitch), angley (Yaw) and anglez (Roll) in degrees.
 * @param quaternion The used Quaternion.
 *
 * @return GLUS_TRUE, if a extraction could be done.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionGetEulerRzRxRyf(GLUSfloat angles[3], const GLUSfloat quaternion[4]);

/**
 * Extracts the Euler angles from a Quaternion, created by Rz * Ry * Rx.
 *
 * @param angles Resulting anglex (Pitch), angley (Yaw) and anglez (Roll) in degrees.
 * @param quaternion The used Quaternion.
 *
 * @return GLUS_TRUE, if a extraction could be done.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionGetEulerRzRyRxf(GLUSfloat angles[3], const GLUSfloat quaternion[4]);

/**
 * Linear interpolation of two Quaternions.
 *
 * @param result The interpolated Quaternion.
 * @param quaternion0 The first Quaternion.
 * @param quaternion1 The second Quaternion.
 * @param t The fraction of both Quaternions.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusQuaternionLerpf(GLUSfloat result[4], const GLUSfloat quaternion0[4], const GLUSfloat quaternion1[4], const GLUSfloat t);

/**
 * Normalized linear interpolation of two Quaternions.
 *
 * @param result The interpolated and normalized Quaternion.
 * @param quaternion0 The first Quaternion.
 * @param quaternion1 The second Quaternion.
 * @param t The fraction of both Quaternions.
 *
 * @return GLUS_TRUE, if normalization succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionNlerpf(GLUSfloat result[4], const GLUSfloat quaternion0[4], const GLUSfloat quaternion1[4], const GLUSfloat t);

/**
 * Spherical interpolation of two Quaternions. Falls back to nlerp when the quaternions are nearly parallel.
 *
 * @param result The interpolated Quaternion.
 * @param quaternion0 The first Quaternion.
 * @param quaternion1 The second Quaternion.
 * @param t The fraction of both Quaternions.
 *
 * @return GLUS_TRUE, if a slerp could be done.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionSlerpf(GLUSfloat result[4], const GLUSfloat quaternion0[4], const GLUSfloat quaternion1[4], const GLUSfloat t);

/**
 * Creates a Quaternion representing the rotation between two normalized vectors.
 *
 * @param result The resulting rotation Quaternion.
 * @param vector0 The first normalized vector.
 * @param vector1 The second normalized vector.
 *
 * @return GLUS_TRUE, if the Quaternion could be created.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusQuaternionRotationBetweenVectorsf(GLUSfloat result[4], const GLUSfloat vector0[3], const GLUSfloat vector1[3]);

#endif /* GLUS_QUATERNION_H_ */
