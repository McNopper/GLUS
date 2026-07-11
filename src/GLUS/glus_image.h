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

#ifndef GLUS_IMAGE_H_
#define GLUS_IMAGE_H_

/**
 * Structure used for Targa Image File loading.
 */
typedef struct _GLUStgaimage
{
    /**
     * Width of the TGA image.
     */
    GLUSushort width;

    /**
     * Height of the TGA image.
     */
    GLUSushort height;

    /**
     * Depth of the image.
     */
    GLUSushort depth;

    /**
     * Pixel data.
     */
    GLUSubyte* data;

    /**
     * Format of the TGA image. Can be:
     *
     * GLUS_RGB
     * GLUS_RGBA
     * GLUS_LUMINANCE
     *
     * Last entry can also be interpreted as GLUS_ALPHA.
     */
    GLUSenum format;

} GLUStgaimage;

/**
 * Structure used for HDR Image File loading.
 */
typedef struct _GLUShdrimage
{
    /**
     * Width of the HDR image.
     */
    GLUSushort width;

    /**
     * Height of the HDR image.
     */
    GLUSushort height;

    /**
     * Depth of the image.
     */
    GLUSushort depth;

    /**
     * Pixel data.
     */
    GLUSfloat* data;

    /**
     * Format of the HDR image.
     */
    GLUSenum format;

} GLUShdrimage;

/**
 * Structure used for PKM Image File loading.
 */
typedef struct _GLUSpkmimage
{
    /**
     * Width of the PKM image.
     */
    GLUSushort width;

    /**
     * Height of the PKM image.
     */
    GLUSushort height;

    /**
     * Depth of the image.
     */
    GLUSushort depth;

    /**
     * Pixel data.
     */
    GLUSubyte* data;

    /**
     * The image size in bytes.
     */
    GLUSint imageSize;

    /**
     * Internal format of the PKM image.
     */
    GLUSenum internalformat;

} GLUSpkmimage;

#endif /* GLUS_IMAGE_H_ */
