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

GLUSboolean GLUSAPIENTRY glusShapeTexGenByAxesf(GLUSshape* shape, const GLUSfloat sSizeX, const GLUSfloat sSizeZ, const GLUSfloat tSizeY, const GLUSfloat tSizeZ, const GLUSfloat sOffset, const GLUSfloat tOffset)
{
    GLUSuint i;

    // vertex, normal, tangent, bitangent, texCoords
    GLUSuint stride = 4 + 3 + 3 + 3 + 2;

    if (!shape || !shape->vertices)
    {
        return GLUS_FALSE;
    }

    if (shape->texCoords)
    {
        glusMemoryFree(shape->texCoords);

        shape->texCoords = 0;
    }

    shape->texCoords = (GLUSfloat*)glusMemoryMalloc((size_t)2 * shape->numberVertices * sizeof(GLUSfloat));

    if (!shape->texCoords)
    {
        return GLUS_FALSE;
    }

    for (i = 0; i < shape->numberVertices; i++)
    {
        shape->texCoords[2 * i + 0] = shape->vertices[4 * i + 0] * sSizeX + shape->vertices[4 * i + 2] * sSizeZ + sOffset;
        shape->texCoords[2 * i + 1] = shape->vertices[4 * i + 1] * tSizeY + shape->vertices[4 * i + 2] * tSizeZ + tOffset;
    }

    // The interleaved buffer is the one uploaded to the GPU, so it has to be refreshed as well.
    if (shape->allAttributes)
    {
        for (i = 0; i < shape->numberVertices; i++)
        {
            shape->allAttributes[i * stride + 13] = shape->texCoords[i * 2 + 0];
            shape->allAttributes[i * stride + 14] = shape->texCoords[i * 2 + 1];
        }
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeTexGenByPlanesf(GLUSshape* shape, const GLUSfloat sPlane[4], const GLUSfloat tPlane[4], const float sSize, const float tSize, const float sOffset, const float tOffset)
{
    GLUSuint i;

    // vertex, normal, tangent, bitangent, texCoords
    GLUSuint stride = 4 + 3 + 3 + 3 + 2;

    if (!shape || !shape->vertices)
    {
        return GLUS_FALSE;
    }

    if (shape->texCoords)
    {
        glusMemoryFree(shape->texCoords);

        shape->texCoords = 0;
    }

    shape->texCoords = (GLUSfloat*)glusMemoryMalloc((size_t)2 * shape->numberVertices * sizeof(GLUSfloat));

    if (!shape->texCoords)
    {
        return GLUS_FALSE;
    }

    for (i = 0; i < shape->numberVertices; i++)
    {
        shape->texCoords[2 * i + 0] = glusPlaneDistancePoint4f(sPlane, &shape->vertices[(size_t)4 * i]) * sSize + sOffset;
        shape->texCoords[2 * i + 1] = glusPlaneDistancePoint4f(tPlane, &shape->vertices[(size_t)4 * i]) * tSize + tOffset;
    }

    // The interleaved buffer is the one uploaded to the GPU, so it has to be refreshed as well.
    if (shape->allAttributes)
    {
        for (i = 0; i < shape->numberVertices; i++)
        {
            shape->allAttributes[i * stride + 13] = shape->texCoords[i * 2 + 0];
            shape->allAttributes[i * stride + 14] = shape->texCoords[i * 2 + 1];
        }
    }

    return GLUS_TRUE;
}
