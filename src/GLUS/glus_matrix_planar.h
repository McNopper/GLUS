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

#ifndef GLUS_MATRIX_PLANAR_H_
#define GLUS_MATRIX_PLANAR_H_

/**
 * Creates the projection matrix to simulate a planar shadow coming from a point / spot light.
 *
 * @param matrix The matrix, which is set to the projection matrix.
 * @param shadowPlane The shadow receiver plane.
 * @param lightPoint The origin of the light.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusMatrix4x4PlanarShadowPointLightf(GLUSfloat matrix[16], const GLUSfloat shadowPlane[4], const GLUSfloat lightPoint[4]);

/**
 * Creates the projection matrix to simulate a planar shadow coming from a directional light.
 *
 * @param matrix The matrix, which is set to the projection matrix.
 * @param shadowPlane The shadow receiver plane.
 * @param lightDirection The direction of the light, pointing towards the light source.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusMatrix4x4PlanarShadowDirectionalLightf(GLUSfloat matrix[16], const GLUSfloat shadowPlane[4], const GLUSfloat lightDirection[3]);

/**
 * Creates the projection matrix to simulate a planar reflection.
 *
 * @param matrix The matrix, which is set to the projection matrix.
 * @param reflectionPlane The reflecting plane.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusMatrix4x4PlanarReflectionf(GLUSfloat matrix[16], const GLUSfloat reflectionPlane[4]);

#endif /* GLUS_MATRIX_PLANAR_H_ */
