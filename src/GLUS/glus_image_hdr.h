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

#ifndef GLUS_IMAGE_HDR_H_
#define GLUS_IMAGE_HDR_H_

/**
 * Creates a HDR image.
 *
 * @param hdrimage	The structure to fill the HDR data.
 * @param width 	Width of the image.
 * @param height 	Height of the image.
 * @param depth 	Depth of the image.
 * @param format 	Format of the image.
 *
 * @return GLUS_TRUE, if creating succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageCreateHdr(GLUShdrimage* hdrimage, GLUSint width, GLUSint height, GLUSint depth, GLUSenum format);

/**
 * Loads a HDR file.
 *
 * @param filename The name of the file to load.
 * @param hdrimage The structure to fill the HDR data.
 *
 * @return GLUS_TRUE, if loading succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageLoadHdr(const GLUSchar* filename, GLUShdrimage* hdrimage);

/**
 * Saves a HDR file.
 *
 * @param filename The name of the file to save.
 * @param hdrimage The structure with the HDR data.
 *
 * @return GLUS_TRUE, if saving succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageSaveHdr(const GLUSchar* filename, const GLUShdrimage* hdrimage);

/**
 * Destroys the content of a HDR structure. Has to be called for freeing the resources.
 *
 * @param hdrimage The HDR file structure.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusImageDestroyHdr(GLUShdrimage* hdrimage);

/**
 * Samples a RGB color value from a HDR 2D image.
 * Sampling uses a bilinear filter.
 *
 * @param rgb 		The resulting, sampled RGB color value.
 * @param hdrimage 	The HDR image structure, containing the 2D texel data.
 * @param st		Texture coordinate, where to sample the 2D texture.
 *
 * @return GLUS_TRUE, if sampling succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusImageSampleHdr2D(GLUSfloat rgb[3], const GLUShdrimage* hdrimage, const GLUSfloat st[2]);

#endif /* GLUS_IMAGE_HDR_H_ */
