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

#ifndef GLUS_MATRIX_VIEWPROJECTION_H_
#define GLUS_MATRIX_VIEWPROJECTION_H_

/**
 * Creates a matrix with orthogonal projection.
 * @see http://en.wikipedia.org/wiki/Orthographic_projection_%28geometry%29
 *
 * @param result The resulting matrix.
 * @param left Left corner.
 * @param right Right corner.
 * @param bottom Bottom corner.
 * @param top Top corner.
 * @param nearVal Near corner.
 * @param farVal Far corner.
 *
 * @return GLUS_TRUE, if creation was successful.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusMatrix4x4Orthof(GLUSfloat result[16], const GLUSfloat left, const GLUSfloat right, const GLUSfloat bottom, const GLUSfloat top, const GLUSfloat nearVal, const GLUSfloat farVal);

/**
 * Creates a matrix with perspective projection.
 *
 * @param result The resulting matrix.
 * @param left Left corner.
 * @param right Right corner.
 * @param bottom Bottom corner.
 * @param top Top corner.
 * @param nearVal Near corner.
 * @param farVal Far corner.
 *
 * @return GLUS_TRUE, if creation was successful.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusMatrix4x4Frustumf(GLUSfloat result[16], const GLUSfloat left, const GLUSfloat right, const GLUSfloat bottom, const GLUSfloat top, const GLUSfloat nearVal, const GLUSfloat farVal);

/**
 * Creates a matrix with perspective projection.
 *
 * @param result The resulting matrix.
 * @param fovy Field of view.
 * @param aspect Aspect ratio.
 * @param zNear Near plane.
 * @param zFar Far plane.
 *
 * @return GLUS_TRUE, if creation was successful.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusMatrix4x4Perspectivef(GLUSfloat result[16], const GLUSfloat fovy, const GLUSfloat aspect, const GLUSfloat zNear, const GLUSfloat zFar);

/**
 * Creates a view / camera matrix.
 *
 * @param result The resulting matrix.
 * @param eyeX Eye / camera X position.
 * @param eyeY Eye / camera Y position.
 * @param eyeZ Eye / camera Z position.
 * @param centerX X Position, where the view / camera points to.
 * @param centerY Y Position, where the view / camera points to.
 * @param centerZ Z Position, where the view / camera points to.
 * @param upX Eye / camera X component from up vector.
 * @param upY Eye / camera Y component from up vector.
 * @param upZ Eye / camera Z component from up vector.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusMatrix4x4LookAtf(GLUSfloat result[16], const GLUSfloat eyeX, const GLUSfloat eyeY, const GLUSfloat eyeZ, const GLUSfloat centerX, const GLUSfloat centerY, const GLUSfloat centerZ, const GLUSfloat upX, const GLUSfloat upY, const GLUSfloat upZ);

#endif /* GLUS_MATRIX_VIEWPROJECTION_H_ */
