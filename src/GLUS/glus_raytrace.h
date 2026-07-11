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

#ifndef GLUS_RAYTRACE_H_
#define GLUS_RAYTRACE_H_

/**
 * Creates normals in a buffer for ray traced perspective projection. Directions are pointing to -Z direction.
 *
 * @param directionBuffer	The resulting direction buffer.
 * @param padding			Amount of padding bytes.
 * @param fovy				Field of view.
 * @param width				Width of the buffer.
 * @param height			Height of the buffer.
 *
 * @return GLUS_TRUE, if creation was successful.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusRaytracePerspectivef(GLUSfloat* directionBuffer, const GLUSubyte padding, const GLUSfloat fovy, const GLUSint width, const GLUSint height);

/**
 * Creates the positions and directions in buffers needed for ray tracing.
 *
 * @param positionBuffer		The resulting position buffer. Positions are in homogeneous coordinates.
 * @param directionBuffer		The resulting direction buffer.
 * @param originDirectionBuffer	The direction buffer, pointing to -Z direction.
 * @param padding				Amount of padding bytes.
 * @param width 				The width of the buffers.
 * @param height 				The height of the buffers.
 * @param eyeX 					Eye / camera X position.
 * @param eyeY 					Eye / camera Y position.
 * @param eyeZ 					Eye / camera Z position.
 * @param centerX 				X Position, where the view / camera points to.
 * @param centerY 				Y Position, where the view / camera points to.
 * @param centerZ 				Z Position, where the view / camera points to.
 * @param upX 					Eye / camera X component from up vector.
 * @param upY 					Eye / camera Y component from up vector.
 * @param upZ 					Eye / camera Z component from up vector.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusRaytraceLookAtf(GLUSfloat* positionBuffer, GLUSfloat* directionBuffer, const GLUSfloat* originDirectionBuffer, const GLUSubyte padding, const GLUSint width, const GLUSint height, const GLUSfloat eyeX, const GLUSfloat eyeY, const GLUSfloat eyeZ, const GLUSfloat centerX, const GLUSfloat centerY, const GLUSfloat centerZ, const GLUSfloat upX, const GLUSfloat upY, const GLUSfloat upZ);

#endif /* GLUS_RAYTRACE_H_ */
