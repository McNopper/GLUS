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

#ifndef GLUS_IMAGE_TGA_H_
#define GLUS_IMAGE_TGA_H_

/**
 * Creates a TGA image.
 *
 * @param tgaimage	The structure to fill the TGA data.
 * @param width 	Width of the image.
 * @param height 	Height of the image.
 * @param depth 	Depth of the image.
 * @param format 	Format of the image.
 *
 * @return GLUS_TRUE, if creating succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageCreateTga(GLUStgaimage* tgaimage, GLUSint width, GLUSint height, GLUSint depth, GLUSenum format);

/**
 * Loads a TGA file.
 *
 * @param filename The name of the file to load.
 * @param tgaimage The structure to fill the TGA data.
 *
 * @return GLUS_TRUE, if loading succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageLoadTga(const GLUSchar* filename, GLUStgaimage* tgaimage);

/**
 * Saves a TGA file.
 *
 * @param filename The name of the file to save.
 * @param tgaimage The structure with the TGA data.
 *
 * @return GLUS_TRUE, if saving succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageSaveTga(const GLUSchar* filename, const GLUStgaimage* tgaimage);

/**
 * Destroys the content of a TGA structure. Has to be called for freeing the resources.
 *
 * @param tgaimage The TGA file structure.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusImageDestroyTga(GLUStgaimage* tgaimage);

/**
 * Samples a RGBA color value from a TGA 2D image.
 * Sampling uses a bilinear filter.
 *
 * @param rgba 		The resulting, sampled RGBA color value.
 * @param tgaimage 	The TGA image structure, containing the 2D texel data.
 * @param st		Texture coordinate, where to sample the 2D texture.
 *
 * @return GLUS_TRUE, if sampling succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageSampleTga2D(GLUSubyte rgba[4], const GLUStgaimage* tgaimage, const GLUSfloat st[2]);

/**
 * Converts a TGA image into another color format.
 * Source and target can not be the same.
 *
 * @param targetImage  The TGA image structure, containing the converted image.
 * @param sourceImage  The TGA image structure, which will be converted.
 * @param targetFormat The target format of the image.
 *
 * @return GLUS_TRUE, if conversion succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageConvertTga(GLUStgaimage* targetImage, const GLUStgaimage* sourceImage, const GLUSenum targetFormat);

/**
 * Converts a TGA image into a premultiplied TGA image.
 * Source has to have GLUS_RGBA as format.
 *
 * @param targetImage  The TGA image structure, containing the premultiplied image.
 * @param sourceImage  The TGA image structure, which will be premultiplied.
 *
 * @return GLUS_TRUE, if premultiplying succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageToPremultiplyTga(GLUStgaimage* targetImage, const GLUStgaimage* sourceImage);

#endif /* GLUS_IMAGE_TGA_H_ */
