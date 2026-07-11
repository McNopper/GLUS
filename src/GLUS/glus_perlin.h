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

#ifndef GLUS_PERLIN_H_
#define GLUS_PERLIN_H_

/**
 * Creates a 1D perlin noise texture. See OpenGL Programming Guide 4.3, p.460ff
 *
 * @param image The perlin noise texture will be stored into this image.
 * @param width Width of the texture.
 * @param seed Random seed number.
 * @param frequency Frequency of the noise.
 * @param amplitude Amplitude of the noise.
 * @param persistence Persistence of the noise.
 * @param octaves Octaves of the noise.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusPerlinCreateNoise1D(GLUStgaimage* image, const GLUSint width, const GLUSint seed, const GLUSfloat frequency, const GLUSfloat amplitude, const GLUSfloat persistence, const GLUSint octaves);

/**
 * Creates a 2D perlin noise texture. See OpenGL Programming Guide 4.3, p.460ff
 *
 * @param image The perlin noise texture will be stored into this image.
 * @param width Width of the texture.
 * @param height Height of the texture
 * @param seed Random seed number.
 * @param frequency Frequency of the noise.
 * @param amplitude Amplitude of the noise.
 * @param persistence Persistence of the noise.
 * @param octaves Octaves of the noise.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusPerlinCreateNoise2D(GLUStgaimage* image, const GLUSint width, const GLUSint height, const GLUSint seed, const GLUSfloat frequency, const GLUSfloat amplitude, const GLUSfloat persistence, const GLUSint octaves);

/**
 * Creates a 3D perlin noise texture. See OpenGL Programming Guide 4.3, p.460ff
 *
 * @param image The perlin noise texture will be stored into this image.
 * @param width Width of the texture.
 * @param height Height of the texture
 * @param depth Depth of the texture
 * @param seed Random seed number.
 * @param frequency Frequency of the noise.
 * @param amplitude Amplitude of the noise.
 * @param persistence Persistence of the noise.
 * @param octaves Octaves of the noise.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusPerlinCreateNoise3D(GLUStgaimage* image, const GLUSint width, const GLUSint height, const GLUSint depth, const GLUSint seed, const GLUSfloat frequency, const GLUSfloat amplitude, const GLUSfloat persistence, const GLUSint octaves);

#endif /* GLUS_PERLIN_H_ */
