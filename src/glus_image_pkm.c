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

GLUSboolean GLUSAPIENTRY glusImageLoadPkm(const GLUSchar* filename, GLUSpkmimage* pkmimage)
{
    GLUSbinaryfile binaryfile;

    GLUSubyte* buffer;

    GLUSubyte type;

    // check, if we have a valid pointer
    if (!filename || !pkmimage)
    {
        return GLUS_FALSE;
    }

    if (!glusFileLoadBinary(filename, &binaryfile))
    {
        return GLUS_FALSE;
    }

    buffer = binaryfile.binary;
    if (!(buffer[0] == 'P' && buffer[1] == 'K' && buffer[2] == 'M' && buffer[3] == ' '))
    {
        glusFileDestroyBinary(&binaryfile);

        return GLUS_FALSE;
    }
    buffer += 7;

    pkmimage->depth = 1;

    pkmimage->imageSize = binaryfile.length - 16;
    if (pkmimage->imageSize <= 0)
    {
        glusFileDestroyBinary(&binaryfile);

        return GLUS_FALSE;
    }

    pkmimage->data = (GLUSubyte*)glusMemoryMalloc(pkmimage->imageSize * sizeof(GLUSubyte));
    if (!pkmimage->data)
    {
        glusFileDestroyBinary(&binaryfile);

        return GLUS_FALSE;
    }

    type = *buffer;
    switch (type)
    {
    case 1:
        pkmimage->internalformat = GLUS_COMPRESSED_RGB8_ETC2;
        break;
    case 3:
        pkmimage->internalformat = GLUS_COMPRESSED_RGBA8_ETC2_EAC;
        break;
    case 4:
        pkmimage->internalformat = GLUS_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        break;
    case 5:
        pkmimage->internalformat = GLUS_COMPRESSED_R11_EAC;
        break;
    case 6:
        pkmimage->internalformat = GLUS_COMPRESSED_RG11_EAC;
        break;
    case 7:
        pkmimage->internalformat = GLUS_COMPRESSED_SIGNED_R11_EAC;
        break;
    case 8:
        pkmimage->internalformat = GLUS_COMPRESSED_SIGNED_RG11_EAC;
        break;
    default:
    {
        glusImageDestroyPkm(pkmimage);

        glusFileDestroyBinary(&binaryfile);

        return GLUS_FALSE;
    }
    break;
    }
    buffer += 5;

    pkmimage->width = (GLUSushort)(*buffer) * 256;
    buffer += 1;
    pkmimage->width += (GLUSushort)(*buffer);
    buffer += 1;

    pkmimage->height = (GLUSushort)(*buffer) * 256;
    buffer += 1;
    pkmimage->height += (GLUSushort)(*buffer);
    buffer += 1;

    memcpy(pkmimage->data, buffer, pkmimage->imageSize);

    glusFileDestroyBinary(&binaryfile);

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusImageDestroyPkm(GLUSpkmimage* pkmimage)
{
    if (!pkmimage)
    {
        return;
    }

    if (pkmimage->data)
    {
        glusMemoryFree(pkmimage->data);

        pkmimage->data = 0;
    }

    pkmimage->width = 0;

    pkmimage->height = 0;

    pkmimage->depth = 0;

    pkmimage->internalformat = 0;

    pkmimage->imageSize = 0;
}
