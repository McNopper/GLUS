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

#include "GL/glus.h"

#define GLUS_MAX_DIMENSION 16384

extern GLUSvoid _glusImageGatherSamplePoints(GLUSint sampleIndex[4], GLUSfloat sampleWeight[2], const GLUSfloat st[2], GLUSint width, GLUSint height, GLUSint stride);

extern GLUSboolean _glusFileCheckRead(FILE* f, size_t actualRead, size_t expectedRead);
extern GLUSboolean _glusFileCheckWrite(FILE* f, size_t actualWrite, size_t expectedWrite);

static GLUSvoid glusImageSwapColorChannel(GLUSint width, GLUSint height, GLUSenum format, GLUSubyte* data)
{
    GLUSint   i;
    GLUSubyte temp;
    GLUSint   bytesPerPixel = 3;

    if (!data)
    {
        return;
    }

    if (format == GLUS_RGBA)
    {
        bytesPerPixel = 4;
    }

    // swap the R and B values to get RGB since the bitmap color format is in BGR
    // Bound in 64 bits: width * height * bytesPerPixel is GLUSint arithmetic and
    // overflows for large caller-built images, after which the loop bound wraps
    // and the BGR swap is silently skipped.
    for (i = 0; (GLUSint64)i < (GLUSint64)width * (GLUSint64)height * bytesPerPixel; i += bytesPerPixel)
    {
        temp        = data[i];
        data[i]     = data[i + 2];
        data[i + 2] = temp;
    }
}

GLUSboolean GLUSAPIENTRY glusImageCreateTga(GLUStgaimage* tgaimage, GLUSint width, GLUSint height, GLUSint depth, GLUSenum format)
{
    GLUSint stride;

    if (!tgaimage || width < 1 || height < 1 || depth < 1 || width > 65535 || height > 65535 || depth > 65535)
    {
        return GLUS_FALSE;
    }

    if (format == GLUS_ALPHA || format == GLUS_LUMINANCE || format == GLUS_RED)
    {
        stride = 1;
    }
    else if (format == GLUS_RGB)
    {
        stride = 3;
    }
    else if (format == GLUS_RGBA)
    {
        stride = 4;
    }
    else
    {
        return GLUS_FALSE;
    }

    tgaimage->data = (GLUSubyte*)glusMemoryMalloc((size_t)width * (size_t)height * (size_t)depth * (size_t)stride * sizeof(GLUSubyte));
    if (!tgaimage->data)
    {
        return GLUS_FALSE;
    }
    tgaimage->width  = width;
    tgaimage->height = height;
    tgaimage->depth  = depth;
    tgaimage->format = format;

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusImageLoadTga(const GLUSchar* filename, GLUStgaimage* tgaimage)
{
    FILE* file;

    GLUSboolean hasColorMap = GLUS_FALSE;

    GLUSubyte header[18];
    GLUSubyte idLength;
    GLUSubyte imageType;
    GLUSubyte bitsPerPixel;

    GLUSushort firstEntryIndex;
    GLUSushort colorMapLength;
    GLUSubyte  colorMapEntrySize;
    GLUSubyte* colorMap = 0;

    GLUSuint i, k, colorMapIndex;

    size_t elementsRead;

    // check, if we have a valid pointer
    if (!filename || !tgaimage)
    {
        return GLUS_FALSE;
    }

    tgaimage->width  = 0;
    tgaimage->height = 0;
    tgaimage->depth  = 0;
    tgaimage->data   = 0;
    tgaimage->format = 0;

    // open filename in "read binary" mode
    file = glusFileOpen(filename, "rb");

    if (!file)
    {
        return GLUS_FALSE;
    }

    // read the complete tga header
    elementsRead = fread(header, 1, 18, file);

    if (!_glusFileCheckRead(file, elementsRead, 18))
    {
        return GLUS_FALSE;
    }

    idLength = header[0];

    // read the image type
    imageType = header[2];

    // check the type
    if (imageType != 1 && imageType != 2 && imageType != 3 && imageType != 9 && imageType != 10 && imageType != 11)
    {
        glusFileClose(file);

        return GLUS_FALSE;
    }

    if (imageType == 1 || imageType == 9)
    {
        hasColorMap = GLUS_TRUE;
    }

    // Note: All multi byte values are stored little endian, so they are decoded byte by byte.
    firstEntryIndex   = (GLUSushort)((GLUSuint)header[3] + (GLUSuint)header[4] * 256);
    colorMapLength    = (GLUSushort)((GLUSuint)header[5] + (GLUSuint)header[6] * 256);
    colorMapEntrySize = header[7];

    // read the width
    tgaimage->width = (GLUSushort)((GLUSuint)header[12] + (GLUSuint)header[13] * 256);

    if (tgaimage->width < 1 || tgaimage->width > GLUS_MAX_DIMENSION)
    {
        glusFileClose(file);

        glusImageDestroyTga(tgaimage);

        return GLUS_FALSE;
    }

    // read the height
    tgaimage->height = (GLUSushort)((GLUSuint)header[14] + (GLUSuint)header[15] * 256);

    if (tgaimage->height < 1 || tgaimage->height > GLUS_MAX_DIMENSION)
    {
        glusFileClose(file);

        glusImageDestroyTga(tgaimage);

        return GLUS_FALSE;
    }

    tgaimage->depth = 1;

    // read the bits per pixel
    bitsPerPixel = header[16];

    // check the pixel depth
    if (bitsPerPixel != 8 && bitsPerPixel != 24 && bitsPerPixel != 32)
    {
        glusFileClose(file);

        glusImageDestroyTga(tgaimage);

        return GLUS_FALSE;
    }
    else
    {
        tgaimage->format = GLUS_SINGLE_CHANNEL;
        if (bitsPerPixel == 24)
        {
            tgaimage->format = GLUS_RGB;
        }
        else if (bitsPerPixel == 32)
        {
            tgaimage->format = GLUS_RGBA;
        }
    }

    if (hasColorMap)
    {
        // A color mapped image stores exactly one look up index per pixel and needs a usable color map.
        if (bitsPerPixel != 8 || colorMapLength == 0)
        {
            glusFileClose(file);

            glusImageDestroyTga(tgaimage);

            return GLUS_FALSE;
        }

        if (colorMapEntrySize != 8 && colorMapEntrySize != 16 && colorMapEntrySize != 24 && colorMapEntrySize != 32)
        {
            glusFileClose(file);

            glusImageDestroyTga(tgaimage);

            return GLUS_FALSE;
        }
    }

    // skip the image ID field, which follows the header
    if (idLength > 0 && fseek(file, (long)idLength, SEEK_CUR))
    {
        glusFileClose(file);

        glusImageDestroyTga(tgaimage);

        return GLUS_FALSE;
    }

    if (hasColorMap)
    {
        // Create color map space.

        GLUSint bytesPerPixel = colorMapEntrySize / 8;

        colorMap = (GLUSubyte*)glusMemoryMalloc((size_t)colorMapLength * bytesPerPixel * sizeof(GLUSubyte));

        if (!colorMap)
        {
            glusFileClose(file);

            glusImageDestroyTga(tgaimage);

            return GLUS_FALSE;
        }

        // Read in the color map.

        elementsRead = fread(colorMap, 1, (size_t)colorMapLength * bytesPerPixel * sizeof(GLUSubyte), file);

        if (!_glusFileCheckRead(file, elementsRead, (size_t)colorMapLength * bytesPerPixel * sizeof(GLUSubyte)))
        {
            glusImageDestroyTga(tgaimage);

            glusMemoryFree(colorMap);
            colorMap = 0;

            return GLUS_FALSE;
        }

        // swap the color if necessary
        if (colorMapEntrySize == 24 || colorMapEntrySize == 32)
        {
            glusImageSwapColorChannel(colorMapLength, 1, colorMapEntrySize == 24 ? GLUS_RGB : GLUS_RGBA, colorMap);
        }
    }

    // allocate enough memory for the targa  data
    // Divide the pixel size down before multiplying: `(size_t)w * h * bpp / 8`
    // evaluates `w * h * bpp` first, which wraps on a 32-bit size_t for large
    // (but legal) dimensions and then allocates far too little for the RLE
    // path to write into.
    tgaimage->data = (GLUSubyte*)glusMemoryMalloc((size_t)tgaimage->width * (size_t)tgaimage->height * (size_t)(bitsPerPixel / 8));

    // verify memory allocation
    if (!tgaimage->data)
    {
        glusFileClose(file);

        glusImageDestroyTga(tgaimage);

        if (hasColorMap)
        {
            glusMemoryFree(colorMap);
            colorMap = 0;
        }

        return GLUS_FALSE;
    }

    if (imageType == 1 || imageType == 2 || imageType == 3)
    {
        // read in the raw data
        elementsRead = fread(tgaimage->data, 1, (size_t)tgaimage->width * (size_t)tgaimage->height * (size_t)(bitsPerPixel / 8), file);

        if (!_glusFileCheckRead(file, elementsRead, (size_t)tgaimage->width * (size_t)tgaimage->height * (size_t)(bitsPerPixel / 8)))
        {
            glusImageDestroyTga(tgaimage);

            if (hasColorMap)
            {
                glusMemoryFree(colorMap);
                colorMap = 0;
            }

            return GLUS_FALSE;
        }
    }
    else
    {
        // RLE encoded
        GLUSint pixelsRead  = 0;
        GLUSint totalPixels = (GLUSint)tgaimage->width * (GLUSint)tgaimage->height;

        while (pixelsRead < totalPixels)
        {
            GLUSubyte amount;

            elementsRead = fread(&amount, 1, 1, file);

            if (!_glusFileCheckRead(file, elementsRead, 1))
            {
                glusImageDestroyTga(tgaimage);

                if (hasColorMap)
                {
                    glusMemoryFree(colorMap);
                    colorMap = 0;
                }

                return GLUS_FALSE;
            }

            // The packet size is stored biased by one and must not exceed the remaining pixels.
            if ((GLUSint)(amount & 0x7F) + 1 > totalPixels - pixelsRead)
            {
                glusFileClose(file);

                glusImageDestroyTga(tgaimage);

                if (hasColorMap)
                {
                    glusMemoryFree(colorMap);
                    colorMap = 0;
                }

                return GLUS_FALSE;
            }

            if (amount & 0x80)
            {
                GLUSint i;
                GLUSint k;

                amount &= 0x7F;

                amount++;

                // read in the rle data
                // The byte offset is computed in size_t and with the pixel size
                // divided down first: `pixelsRead * bitsPerPixel / 8` is signed
                // 32-bit arithmetic and overflows at 2^26 pixels for 32bpp, after
                // which fread writes attacker-controlled bytes at a wrapped
                // (negative) offset outside the buffer.
                elementsRead = fread(&tgaimage->data[(size_t)pixelsRead * (size_t)(bitsPerPixel / 8)], 1, bitsPerPixel / 8, file);

                if (!_glusFileCheckRead(file, elementsRead, bitsPerPixel / 8))
                {
                    glusImageDestroyTga(tgaimage);

                    if (hasColorMap)
                    {
                        glusMemoryFree(colorMap);
                        colorMap = 0;
                    }

                    return GLUS_FALSE;
                }

                for (i = 1; i < amount; i++)
                {
                    for (k = 0; k < bitsPerPixel / 8; k++)
                    {
                        tgaimage->data[(size_t)(pixelsRead + i) * (size_t)(bitsPerPixel / 8) + k] = tgaimage->data[(size_t)pixelsRead * (size_t)(bitsPerPixel / 8) + k];
                    }
                }
            }
            else
            {
                amount &= 0x7F;

                amount++;

                // read in the raw data
                elementsRead = fread(&tgaimage->data[(size_t)pixelsRead * (size_t)(bitsPerPixel / 8)], 1, (size_t)amount * (size_t)(bitsPerPixel / 8), file);

                if (!_glusFileCheckRead(file, elementsRead, (size_t)amount * (size_t)(bitsPerPixel / 8)))
                {
                    glusImageDestroyTga(tgaimage);

                    if (hasColorMap)
                    {
                        glusMemoryFree(colorMap);
                        colorMap = 0;
                    }

                    return GLUS_FALSE;
                }
            }

            pixelsRead += amount;
        }
    }

    // swap the color if necessary
    if (bitsPerPixel == 24 || bitsPerPixel == 32)
    {
        glusImageSwapColorChannel(tgaimage->width, tgaimage->height, tgaimage->format, tgaimage->data);
    }

    // close the file
    glusFileClose(file);

    if (hasColorMap)
    {
        GLUSubyte* data = tgaimage->data;

        GLUSint bytesPerPixel;

        // Allocating new memory, as current memory is a look up table index and not a color.

        bytesPerPixel = colorMapEntrySize / 8;

        tgaimage->data = (GLUSubyte*)glusMemoryMalloc((size_t)tgaimage->width * tgaimage->height * bytesPerPixel);

        if (!tgaimage->data)
        {
            glusImageDestroyTga(tgaimage);

            glusMemoryFree(data);
            data = 0;

            glusMemoryFree(colorMap);
            colorMap = 0;

            return GLUS_FALSE;
        }

        tgaimage->format = GLUS_SINGLE_CHANNEL;
        if (colorMapEntrySize == 24)
        {
            tgaimage->format = GLUS_RGB;
        }
        else if (colorMapEntrySize == 32)
        {
            tgaimage->format = GLUS_RGBA;
        }

        // Copy color values from the color map into the image data.

        for (i = 0; i < (GLUSuint)tgaimage->width * (GLUSuint)tgaimage->height; i++)
        {
            colorMapIndex = (GLUSuint)firstEntryIndex + (GLUSuint)data[i];

            // The look up index has to be inside the color map.
            if (colorMapIndex >= (GLUSuint)colorMapLength)
            {
                glusImageDestroyTga(tgaimage);

                glusMemoryFree(data);
                data = 0;

                glusMemoryFree(colorMap);
                colorMap = 0;

                return GLUS_FALSE;
            }

            for (k = 0; k < (GLUSuint)bytesPerPixel; k++)
            {
                tgaimage->data[i * bytesPerPixel + k] = colorMap[colorMapIndex * bytesPerPixel + k];
            }
        }

        // Freeing data.

        glusMemoryFree(data);
        data = 0;

        glusMemoryFree(colorMap);
        colorMap = 0;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusImageSaveTga(const GLUSchar* filename, const GLUStgaimage* tgaimage)
{
    FILE*      file;
    GLUSubyte  buffer[12];
    GLUSubyte  bitsPerPixel;
    size_t     elementsWritten;
    size_t     dataSize;
    GLUSubyte* data;

    // check, if we have a valid pointer
    if (!filename || !tgaimage)
    {
        return GLUS_FALSE;
    }

    // open filename in "write binary" mode
    file = glusFileOpen(filename, "wb");

    if (!file)
    {
        return GLUS_FALSE;
    }

    switch (tgaimage->format)
    {
    case GLUS_ALPHA:
    case GLUS_RED:
    case GLUS_LUMINANCE:
        bitsPerPixel = 8;
        break;
    case GLUS_RGB:
        bitsPerPixel = 24;
        break;
    case GLUS_RGBA:
        bitsPerPixel = 32;
        break;
    default:
        glusFileClose(file);
        return GLUS_FALSE;
    }

    if (bitsPerPixel == 8)
    {
        buffer[2] = 3;
    }
    else
    {
        buffer[2] = 2;
    }

    // TGA header
    buffer[0] = 0;
    buffer[1] = 0;

    buffer[3]  = 0;
    buffer[4]  = 0;
    buffer[5]  = 0;
    buffer[6]  = 0;
    buffer[7]  = 0;
    buffer[8]  = 0;
    buffer[9]  = 0;
    buffer[10] = 0;
    buffer[11] = 0;

    elementsWritten = fwrite(buffer, 1, 12, file);

    if (!_glusFileCheckWrite(file, elementsWritten, 12))
    {
        return GLUS_FALSE;
    }

    // Note: All multi byte values are stored little endian, so they are encoded byte by byte.
    buffer[0] = (GLUSubyte)(tgaimage->width & 0xFF);
    buffer[1] = (GLUSubyte)((tgaimage->width >> 8) & 0xFF);

    elementsWritten = fwrite(buffer, 1, 2, file);

    if (!_glusFileCheckWrite(file, elementsWritten, 2))
    {
        return GLUS_FALSE;
    }

    buffer[0] = (GLUSubyte)(tgaimage->height & 0xFF);
    buffer[1] = (GLUSubyte)((tgaimage->height >> 8) & 0xFF);

    elementsWritten = fwrite(buffer, 1, 2, file);

    if (!_glusFileCheckWrite(file, elementsWritten, 2))
    {
        return GLUS_FALSE;
    }

    elementsWritten = fwrite(&bitsPerPixel, sizeof(bitsPerPixel), 1, file);

    if (!_glusFileCheckWrite(file, elementsWritten, 1))
    {
        return GLUS_FALSE;
    }

    buffer[0] = 0;

    elementsWritten = fwrite(buffer, 1, 1, file);

    if (!_glusFileCheckWrite(file, elementsWritten, 1))
    {
        return GLUS_FALSE;
    }

    dataSize = (size_t)tgaimage->width * (size_t)tgaimage->height * (size_t)bitsPerPixel / 8;

    if (!tgaimage->data || dataSize == 0)
    {
        glusFileClose(file);

        return GLUS_FALSE;
    }

    data = glusMemoryMalloc(dataSize);

    if (!data)
    {
        glusFileClose(file);

        return GLUS_FALSE;
    }

    memcpy(data, tgaimage->data, dataSize);

    if (bitsPerPixel >= 24)
    {
        glusImageSwapColorChannel(tgaimage->width, tgaimage->height, tgaimage->format, data);
    }

    elementsWritten = fwrite(data, 1, dataSize, file);

    glusMemoryFree(data);

    if (!_glusFileCheckWrite(file, elementsWritten, dataSize))
    {
        return GLUS_FALSE;
    }

    glusFileClose(file);

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusImageDestroyTga(GLUStgaimage* tgaimage)
{
    if (!tgaimage)
    {
        return;
    }

    if (tgaimage->data)
    {
        glusMemoryFree(tgaimage->data);

        tgaimage->data = 0;
    }

    tgaimage->width = 0;

    tgaimage->height = 0;

    tgaimage->depth = 0;

    tgaimage->format = 0;
}

GLUSboolean GLUSAPIENTRY glusImageSampleTga2D(GLUSubyte rgba[4], const GLUStgaimage* tgaimage, const GLUSfloat st[2])
{
    GLUSfloat tempRGBA[4];

    GLUSint   sampleIndex[4];
    GLUSfloat sampleWeight[2];

    GLUSint i, stride;

    if (!rgba || !tgaimage || !st)
    {
        return GLUS_FALSE;
    }

    stride = 1;
    if (tgaimage->format == GLUS_RGB)
    {
        stride = 3;
    }
    else if (tgaimage->format == GLUS_RGBA)
    {
        stride = 4;
    }

    _glusImageGatherSamplePoints(sampleIndex, sampleWeight, st, tgaimage->width, tgaimage->height, stride);

    for (i = 0; i < stride; i++)
    {
        tempRGBA[i] = (GLUSfloat)tgaimage->data[sampleIndex[0] + i] * sampleWeight[0] * sampleWeight[1];
        tempRGBA[i] += (GLUSfloat)tgaimage->data[sampleIndex[1] + i] * (1.0f - sampleWeight[0]) * sampleWeight[1];
        tempRGBA[i] += (GLUSfloat)tgaimage->data[sampleIndex[2] + i] * sampleWeight[0] * (1.0f - sampleWeight[1]);
        tempRGBA[i] += (GLUSfloat)tgaimage->data[sampleIndex[3] + i] * (1.0f - sampleWeight[0]) * (1.0f - sampleWeight[1]);
    }

    // Resolve

    for (i = 0; i < 4; i++)
    {
        if (i < stride)
        {
            rgba[i] = (GLUSubyte)tempRGBA[i];
        }
        else if (i < 3)
        {
            rgba[i] = rgba[0];
        }
        else
        {
            rgba[3] = 255;
        }
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusImageConvertTga(GLUStgaimage* targetImage, const GLUStgaimage* sourceImage, const GLUSenum targetFormat)
{
    GLUSint targetNumberChannels = 1;
    GLUSint sourceNumberChannels = 1;
    GLUSint x, y, z, c;

    GLUSubyte channels[4] = {0, 0, 0, 255};

    GLUSfloat toLuminace[3] = {0.299f, 0.587f, 0.114f};

    if (!targetImage || !sourceImage)
    {
        return GLUS_FALSE;
    }

    if (sourceImage->format != GLUS_RED && sourceImage->format != GLUS_ALPHA && sourceImage->format != GLUS_LUMINANCE && sourceImage->format != GLUS_RGB && sourceImage->format != GLUS_RGBA)
    {
        return GLUS_FALSE;
    }

    if (targetFormat != GLUS_RED && targetFormat != GLUS_ALPHA && targetFormat != GLUS_LUMINANCE && targetFormat != GLUS_RGB && targetFormat != GLUS_RGBA)
    {
        return GLUS_FALSE;
    }

    if (sourceImage->format == GLUS_RGB)
    {
        sourceNumberChannels = 3;
    }
    else if (sourceImage->format == GLUS_RGBA)
    {
        sourceNumberChannels = 4;
    }

    if (targetFormat == GLUS_RGB)
    {
        targetNumberChannels = 3;
    }
    else if (targetFormat == GLUS_RGBA)
    {
        targetNumberChannels = 4;
    }

    targetImage->data = (GLUSubyte*)glusMemoryMalloc((size_t)targetNumberChannels * (size_t)sourceImage->width * (size_t)sourceImage->height * (size_t)sourceImage->depth * sizeof(GLUSubyte));

    if (!targetImage->data)
    {
        return GLUS_FALSE;
    }
    targetImage->width  = sourceImage->width;
    targetImage->height = sourceImage->height;
    targetImage->depth  = sourceImage->depth;
    targetImage->format = targetFormat;

    for (z = 0; z < targetImage->depth; z++)
    {
        for (y = 0; y < targetImage->height; y++)
        {
            for (x = 0; x < targetImage->width; x++)
            {
                if (sourceImage->format == GLUS_RED)
                {
                    if (targetImage->format == GLUS_RED || targetImage->format == GLUS_RGB || targetImage->format == GLUS_RGBA)
                    {
                        channels[0] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0];

                        if (targetImage->format == GLUS_RGB || targetImage->format == GLUS_RGBA)
                        {
                            channels[1] = 0;
                            channels[2] = 0;

                            if (targetImage->format == GLUS_RGBA)
                            {
                                channels[3] = 255;
                            }
                        }
                    }
                    else if (targetImage->format == GLUS_ALPHA)
                    {
                        channels[0] = 255;
                    }
                    else if (targetImage->format == GLUS_LUMINANCE)
                    {
                        channels[0] = (GLUSubyte)((GLUSfloat)sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0] * toLuminace[0]);
                    }
                }
                else if (sourceImage->format == GLUS_ALPHA)
                {
                    if (targetImage->format == GLUS_LUMINANCE || targetImage->format == GLUS_RED || targetImage->format == GLUS_RGB || targetImage->format == GLUS_RGBA)
                    {
                        channels[0] = 0;

                        if (targetImage->format == GLUS_RGB || targetImage->format == GLUS_RGBA)
                        {
                            channels[1] = 0;
                            channels[2] = 0;

                            if (targetImage->format == GLUS_RGBA)
                            {
                                channels[3] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0];
                            }
                        }
                    }
                    else if (targetImage->format == GLUS_ALPHA)
                    {
                        channels[0] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0];
                    }
                }
                else if (sourceImage->format == GLUS_LUMINANCE)
                {
                    if (targetImage->format == GLUS_RED)
                    {
                        channels[0] = (GLUSubyte)glusMathClampf((GLUSfloat)sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0] / toLuminace[0], 0.0f, 255.0f);
                    }
                    else if (targetImage->format == GLUS_RGB || targetImage->format == GLUS_RGBA)
                    {
                        channels[0] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0];
                        channels[1] = channels[0];
                        channels[2] = channels[0];

                        if (targetImage->format == GLUS_RGBA)
                        {
                            channels[3] = 255;
                        }
                    }
                    else if (targetImage->format == GLUS_ALPHA)
                    {
                        channels[0] = 255;
                    }
                    else if (targetImage->format == GLUS_LUMINANCE)
                    {
                        channels[0] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0];
                    }
                }
                if (sourceImage->format == GLUS_RGB || sourceImage->format == GLUS_RGBA)
                {
                    if (targetImage->format == GLUS_RED)
                    {
                        channels[0] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 0];
                    }
                    else if (targetImage->format == GLUS_ALPHA)
                    {
                        if (sourceImage->format == GLUS_RGB)
                        {
                            channels[0] = 255;
                        }
                        else if (sourceImage->format == GLUS_RGBA)
                        {
                            channels[0] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 3];
                        }
                    }
                    else if (targetImage->format == GLUS_LUMINANCE)
                    {
                        GLUSfloat luminance = 0.0f;

                        for (c = 0; c < 3; c++)
                        {
                            luminance += (GLUSfloat)sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + c] * toLuminace[c];
                        }

                        channels[0] = (GLUSubyte)luminance;
                    }
                    else if (targetImage->format == GLUS_RGB || targetImage->format == GLUS_RGBA)
                    {
                        for (c = 0; c < 3; c++)
                        {
                            channels[c] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + c];
                        }

                        if (targetImage->format == GLUS_RGBA)
                        {
                            if (sourceImage->format == GLUS_RGBA)
                            {
                                channels[3] = sourceImage->data[sourceNumberChannels * z * sourceImage->height * sourceImage->width + sourceNumberChannels * y * sourceImage->width + sourceNumberChannels * x + 3];
                            }
                            else
                            {
                                channels[3] = 255;
                            }
                        }
                    }
                }

                for (c = 0; c < targetNumberChannels; c++)
                {
                    targetImage->data[targetNumberChannels * z * targetImage->height * targetImage->width + targetNumberChannels * y * targetImage->width + targetNumberChannels * x + c] = channels[c];
                }
            }
        }
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusImageToPremultiplyTga(GLUStgaimage* targetImage, const GLUStgaimage* sourceImage)
{
    GLUSint x, y, z, c;

    GLUSfloat alpha;
    GLUSfloat channel;

    if (!targetImage || !sourceImage)
    {
        return GLUS_FALSE;
    }

    if (sourceImage->format != GLUS_RGBA)
    {
        return GLUS_FALSE;
    }

    targetImage->data = (GLUSubyte*)glusMemoryMalloc((size_t)4 * (size_t)sourceImage->width * (size_t)sourceImage->height * (size_t)sourceImage->depth * sizeof(GLUSubyte));

    if (!targetImage->data)
    {
        return GLUS_FALSE;
    }
    targetImage->width  = sourceImage->width;
    targetImage->height = sourceImage->height;
    targetImage->depth  = sourceImage->depth;
    targetImage->format = sourceImage->format;

    for (z = 0; z < targetImage->depth; z++)
    {
        for (y = 0; y < targetImage->height; y++)
        {
            for (x = 0; x < targetImage->width; x++)
            {
                alpha = (GLUSfloat)sourceImage->data[4 * z * targetImage->height * targetImage->width + 4 * y * targetImage->width + 4 * x + 3] / 255.0f;

                for (c = 0; c < 3; c++)
                {
                    channel = (GLUSfloat)sourceImage->data[4 * z * targetImage->height * targetImage->width + 4 * y * targetImage->width + 4 * x + c] / 255.0f;

                    targetImage->data[4 * z * targetImage->height * targetImage->width + 4 * y * targetImage->width + 4 * x + c] = (GLUSubyte)glusMathClampf(channel * alpha * 255.0f, 0.0f, 255.0f);
                }

                // Copy alpha too. The loop above only writes RGB, so the output's
                // alpha bytes were uninitialized heap garbage in every pixel.
                targetImage->data[4 * z * targetImage->height * targetImage->width + 4 * y * targetImage->width + 4 * x + 3] = sourceImage->data[4 * z * targetImage->height * targetImage->width + 4 * y * targetImage->width + 4 * x + 3];
            }
        }
    }

    return GLUS_TRUE;
}
