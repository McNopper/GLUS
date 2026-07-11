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

#ifndef GLUS_IBL_H_
#define GLUS_IBL_H_

/**
 * Set the directory in which the GLUS IBL shaders reside.
 * The default path is "../GLUS/shader/".
 *
 * @param path  Null-terminated path string including trailing slash/backslash.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusIblSetShaderPath(const GLUSchar* path);

/**
 * Convert a latitude-longitude HDR panorama texture to a plain cubemap.
 * The result is suitable for use as a scene background.
 *
 * @param backgroundTexture  [out] GL texture name of the created GL_TEXTURE_CUBE_MAP (GL_RGB32F).
 * @param panoramaTexture    Bound GL_TEXTURE_2D containing the equirectangular panorama.
 * @param size               Edge length (pixels) of each cubemap face.
 * @return GLUS_TRUE on success, GLUS_FALSE on failure.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusIblBuildBackgroundCubemap(GLUSuint* backgroundTexture, GLUSuint panoramaTexture, GLUSint size);

/**
 * Pre-filter the environment map for specular IBL into a cubemap-array texture
 * where layer i*6+face contains the face image at roughness level i.
 *
 * @param specularTexture   [out] GL texture name of the created GL_TEXTURE_CUBE_MAP_ARRAY (GL_RGB32F).
 * @param panoramaTexture   Bound GL_TEXTURE_2D containing the equirectangular panorama.
 * @param size              Edge length (pixels) of the mip-0 face.
 * @param numberRoughness   Number of roughness levels (layers = numberRoughness * 6).
 * @return GLUS_TRUE on success, GLUS_FALSE on failure.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusIblBuildSpecularEnvironmentMap(GLUSuint* specularTexture, GLUSuint panoramaTexture, GLUSint size, GLUSint numberRoughness);

/**
 * Convolve the environment map with a cosine lobe to produce a diffuse
 * irradiance cubemap.
 *
 * @param diffuseTexture   [out] GL texture name of the created GL_TEXTURE_CUBE_MAP (GL_RGB32F).
 * @param panoramaTexture  Bound GL_TEXTURE_2D containing the equirectangular panorama.
 * @param size             Edge length (pixels) of each cubemap face.
 * @return GLUS_TRUE on success, GLUS_FALSE on failure.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusIblBuildDiffuseEnvironmentMap(GLUSuint* diffuseTexture, GLUSuint panoramaTexture, GLUSint size);

/**
 * Pre-integrate the split-sum BRDF look-up table.
 *
 * @param brdfLutTexture   [out] GL texture name of the created GL_TEXTURE_2D (GL_RG32F).
 * @param size             Width/height of the square LUT texture.
 * @return GLUS_TRUE on success, GLUS_FALSE on failure.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusIblBuildBrdfLookupTable(GLUSuint* brdfLutTexture, GLUSint size);

#endif /* GLUS_IBL_H_ */
