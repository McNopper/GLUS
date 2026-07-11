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

static GLUSvoid glusLineInitf(GLUSline* line)
{
    if (!line)
    {
        return;
    }

    memset(line, 0, sizeof(GLUSline));

    line->mode = GLUS_LINE_LOOP;
}

static GLUSboolean glusLineCheckf(GLUSline* line)
{
    if (!line)
    {
        return GLUS_FALSE;
    }

    return line->vertices && line->numberIndices;
}

GLUSboolean GLUSAPIENTRY glusLineCreateLinef(GLUSline* line, const GLUSfloat point0[4], const GLUSfloat point1[4])
{
    if (!line)
    {
        return GLUS_FALSE;
    }
    glusLineInitf(line);

    line->numberVertices = 2;
    line->numberIndices  = 2;

    line->vertices = (GLUSfloat*)glusMemoryMalloc(4 * line->numberVertices * sizeof(GLUSfloat));
    line->indices  = (GLUSindex*)glusMemoryMalloc(line->numberIndices * sizeof(GLUSindex));

    line->mode = GLUS_LINES;

    if (!glusLineCheckf(line))
    {
        glusLineDestroyf(line);

        return GLUS_FALSE;
    }

    memcpy(line->vertices, point0, 4 * sizeof(GLUSfloat));
    memcpy(line->vertices + 4, point1, 4 * sizeof(GLUSfloat));

    line->indices[0] = 0;
    line->indices[1] = 1;

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusLineCreateSquaref(GLUSline* line, const GLUSfloat halfExtend)
{
    if (!line)
    {
        return GLUS_FALSE;
    }
    glusLineInitf(line);

    line->numberVertices = 4;
    line->numberIndices  = 4;

    line->vertices = (GLUSfloat*)glusMemoryMalloc(4 * line->numberVertices * sizeof(GLUSfloat));
    line->indices  = (GLUSindex*)glusMemoryMalloc(line->numberIndices * sizeof(GLUSindex));

    line->mode = GLUS_LINE_LOOP;

    if (!glusLineCheckf(line))
    {
        glusLineDestroyf(line);

        return GLUS_FALSE;
    }

    line->vertices[0] = halfExtend;
    line->vertices[1] = -halfExtend;
    line->vertices[2] = 0.0f;
    line->vertices[3] = 1.0f;

    line->vertices[4] = halfExtend;
    line->vertices[5] = halfExtend;
    line->vertices[6] = 0.0f;
    line->vertices[7] = 1.0f;

    line->vertices[8]  = -halfExtend;
    line->vertices[9]  = halfExtend;
    line->vertices[10] = 0.0f;
    line->vertices[11] = 1.0f;

    line->vertices[12] = -halfExtend;
    line->vertices[13] = -halfExtend;
    line->vertices[14] = 0.0f;
    line->vertices[15] = 1.0f;

    line->indices[0] = 0;
    line->indices[1] = 1;
    line->indices[2] = 2;
    line->indices[3] = 3;

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusLineCreateRectangularGridf(GLUSline* line, const GLUSfloat horizontalExtend, const GLUSfloat verticalExtend, const GLUSuint rows, const GLUSuint columns)
{
    GLUSuint i, offset;
    GLUSuint numberVertices = (rows + 1) * 2 + (columns + 1) * 2;
    GLUSuint numberIndices  = numberVertices;
    float    rowStep, columnStep;

    if (!line || rows < 1 || columns < 1 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }
    glusLineInitf(line);

    line->numberVertices = numberVertices;
    line->numberIndices  = numberIndices;

    line->vertices = (GLUSfloat*)glusMemoryMalloc(4 * line->numberVertices * sizeof(GLUSfloat));
    line->indices  = (GLUSindex*)glusMemoryMalloc(line->numberIndices * sizeof(GLUSindex));

    line->mode = GLUS_LINES;

    if (!glusLineCheckf(line))
    {
        glusLineDestroyf(line);

        return GLUS_FALSE;
    }

    rowStep = verticalExtend / (float)rows;
    for (i = 0; i <= rows; i++)
    {
        line->vertices[0 + i * 4 * 2] = -horizontalExtend * 0.5f;
        line->vertices[1 + i * 4 * 2] = -verticalExtend * 0.5f + rowStep * (float)i;
        line->vertices[2 + i * 4 * 2] = 0.0f;
        line->vertices[3 + i * 4 * 2] = 1.0f;

        line->indices[i * 2] = i * 2;

        //

        line->vertices[0 + i * 4 * 2 + 4] = horizontalExtend * 0.5f;
        line->vertices[1 + i * 4 * 2 + 4] = -verticalExtend * 0.5f + rowStep * (float)i;
        line->vertices[2 + i * 4 * 2 + 4] = 0.0f;
        line->vertices[3 + i * 4 * 2 + 4] = 1.0f;

        line->indices[i * 2 + 1] = i * 2 + 1;
    }

    offset = (rows + 1) * 2;

    columnStep = horizontalExtend / (float)columns;
    for (i = 0; i <= columns; i++)
    {
        line->vertices[0 + i * 4 * 2 + offset * 4] = -horizontalExtend * 0.5f + columnStep * (float)i;
        line->vertices[1 + i * 4 * 2 + offset * 4] = -verticalExtend * 0.5f;
        line->vertices[2 + i * 4 * 2 + offset * 4] = 0.0f;
        line->vertices[3 + i * 4 * 2 + offset * 4] = 1.0f;

        line->indices[i * 2 + offset] = i * 2 + offset;

        //

        line->vertices[0 + i * 4 * 2 + 4 + offset * 4] = -horizontalExtend * 0.5f + columnStep * (float)i;
        line->vertices[1 + i * 4 * 2 + 4 + offset * 4] = verticalExtend * 0.5f;
        line->vertices[2 + i * 4 * 2 + 4 + offset * 4] = 0.0f;
        line->vertices[3 + i * 4 * 2 + 4 + offset * 4] = 1.0f;

        line->indices[i * 2 + 1 + offset] = i * 2 + 1 + offset;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusLineCreateCirclef(GLUSline* line, const GLUSfloat radius, const GLUSuint numberSectors)
{
    GLUSuint i;
    float    singleStep;
    float    totalStep;

    GLUSuint numberVertices = numberSectors;
    GLUSuint numberIndices  = numberSectors;

    if (!line || numberSectors < 3 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }
    glusLineInitf(line);

    line->numberVertices = numberSectors;
    line->numberIndices  = numberSectors;

    line->vertices = (GLUSfloat*)glusMemoryMalloc(4 * line->numberVertices * sizeof(GLUSfloat));
    line->indices  = (GLUSindex*)glusMemoryMalloc(line->numberIndices * sizeof(GLUSindex));

    line->mode = GLUS_LINE_LOOP;

    if (!glusLineCheckf(line))
    {
        glusLineDestroyf(line);

        return GLUS_FALSE;
    }

    singleStep = 2.0f * GLUS_PI / (float)numberSectors;
    totalStep  = 0.0f;
    for (i = 0; i < numberSectors; i++)
    {
        line->vertices[0 + i * 4] = radius * cosf(totalStep);
        line->vertices[1 + i * 4] = radius * sinf(totalStep);
        line->vertices[2 + i * 4] = 0.0f;
        line->vertices[3 + i * 4] = 1.0f;

        line->indices[i] = i;

        totalStep += singleStep;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusLineCopyf(GLUSline* line, const GLUSline* source)
{
    if (!line || !source)
    {
        return GLUS_FALSE;
    }

    glusLineInitf(line);

    line->numberVertices = source->numberVertices;
    line->numberIndices  = source->numberIndices;

    if (source->numberVertices)
    {
        line->vertices = (GLUSfloat*)glusMemoryMalloc(4 * source->numberVertices * sizeof(GLUSfloat));
        if (!line->vertices)
        {
            glusLineDestroyf(line);

            return GLUS_FALSE;
        }
        memcpy(line->vertices, source->vertices, 4 * source->numberVertices * sizeof(GLUSfloat));
    }

    if (source->indices)
    {
        line->indices = (GLUSindex*)glusMemoryMalloc(source->numberIndices * sizeof(GLUSindex));
        if (!line->indices)
        {
            glusLineDestroyf(line);

            return GLUS_FALSE;
        }
        memcpy(line->indices, source->indices, source->numberIndices * sizeof(GLUSindex));
    }

    line->mode = source->mode;

    return GLUS_TRUE;
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusLineDestroyf(GLUSline* line)
{
    if (!line)
    {
        return;
    }

    if (line->vertices)
    {
        glusMemoryFree(line->vertices);

        line->vertices = 0;
    }

    if (line->indices)
    {
        glusMemoryFree(line->indices);

        line->indices = 0;
    }

    line->numberVertices = 0;
    line->numberIndices  = 0;
    line->mode           = 0;
}
