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

#define GLUS_POINT_TOLERANCE 0.001f

static GLUSboolean glusShapeCheckCompletef(GLUSshape* shape)
{
    if (!shape)
    {
        return GLUS_FALSE;
    }

    return shape->vertices && shape->normals && shape->tangents && shape->bitangents && shape->texCoords && shape->allAttributes && shape->indices;
}

static GLUSboolean glusShapeFindIndexByIndicesf(GLUSuint* adjacentIndex, GLUSuint triangleFirstIndex, GLUSuint edge, const GLUSshape* shape)
{
    GLUSuint i, k, m;

    GLUSuint equalIndices;

    GLUSuint edgeIndices[2];

    if (!adjacentIndex || !shape)
    {
        return GLUS_FALSE;
    }

    if (shape->mode != GLUS_TRIANGLES)
    {
        return GLUS_FALSE;
    }

    for (i = 0; i < shape->numberIndices / 3; i++)
    {
        // Skip same triangle
        if (triangleFirstIndex == i * 3)
        {
            continue;
        }

        equalIndices = 0;

        for (k = 0; k < 3; k++)
        {
            for (m = 0 + edge; m < 2 + edge; m++)
            {
                if (shape->indices[triangleFirstIndex + (m % 3)] == shape->indices[i * 3 + k])
                {
                    equalIndices++;

                    edgeIndices[m - edge] = shape->indices[i * 3 + k];

                    break;
                }
            }
        }

        if (equalIndices == 2)
        {
            for (k = 0; k < 3; k++)
            {
                if (shape->indices[i * 3 + k] != edgeIndices[0] && shape->indices[i * 3 + k] != edgeIndices[1])
                {
                    *adjacentIndex = shape->indices[i * 3 + k];

                    break;
                }
            }

            return GLUS_TRUE;
        }
    }

    return GLUS_FALSE;
}

static GLUSboolean glusShapeFindIndexByVerticesf(GLUSuint* adjacentIndex, GLUSuint triangleIndex, GLUSuint edge, const GLUSshape* shape)
{
    GLUSuint i, k, m;

    GLUSuint equalVertices, walkerIndex, searchIndex;

    GLUSuint edgeIndices[2];

    if (!adjacentIndex || !shape)
    {
        return GLUS_FALSE;
    }

    if (shape->mode != GLUS_TRIANGLES)
    {
        return GLUS_FALSE;
    }

    for (i = 0; i < shape->numberIndices / 3; i++)
    {
        // Skip same triangle
        if (triangleIndex == i * 3)
        {
            continue;
        }

        equalVertices = 0;

        for (k = 0; k < 3; k++)
        {
            walkerIndex = shape->indices[i * 3 + k];

            for (m = 0 + edge; m < 2 + edge; m++)
            {
                searchIndex = shape->indices[triangleIndex + (m % 3)];

                if (shape->vertices[4 * searchIndex + 0] >= shape->vertices[4 * walkerIndex + 0] - GLUS_POINT_TOLERANCE && shape->vertices[4 * searchIndex + 0] <= shape->vertices[4 * walkerIndex + 0] + GLUS_POINT_TOLERANCE &&
                    shape->vertices[4 * searchIndex + 1] >= shape->vertices[4 * walkerIndex + 1] - GLUS_POINT_TOLERANCE && shape->vertices[4 * searchIndex + 1] <= shape->vertices[4 * walkerIndex + 1] + GLUS_POINT_TOLERANCE &&
                    shape->vertices[4 * searchIndex + 2] >= shape->vertices[4 * walkerIndex + 2] - GLUS_POINT_TOLERANCE && shape->vertices[4 * searchIndex + 2] <= shape->vertices[4 * walkerIndex + 2] + GLUS_POINT_TOLERANCE)
                {
                    equalVertices++;

                    edgeIndices[m - edge] = walkerIndex;

                    break;
                }
            }
        }

        if (equalVertices == 2)
        {
            for (k = 0; k < 3; k++)
            {
                if (shape->indices[i * 3 + k] != edgeIndices[0] && shape->indices[i * 3 + k] != edgeIndices[1])
                {
                    *adjacentIndex = shape->indices[i * 3 + k];

                    break;
                }
            }

            return GLUS_TRUE;
        }
    }

    return GLUS_FALSE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateAdjacencyIndicesf(GLUSshape* adjacencyShape, const GLUSshape* sourceShape)
{
    GLUSuint i;

    GLUSuint numberIndices;

    GLUSuint adjacentIndex, edge;

    if (!adjacencyShape || !sourceShape)
    {
        return GLUS_FALSE;
    }

    if (sourceShape->mode != GLUS_TRIANGLES)
    {
        return GLUS_FALSE;
    }

    numberIndices = sourceShape->numberIndices * 2;

    if (numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!glusShapeCopyf(adjacencyShape, sourceShape))
    {
        glusShapeDestroyf(adjacencyShape);

        return GLUS_FALSE;
    }

    adjacencyShape->numberIndices = numberIndices;

    glusMemoryFree(adjacencyShape->indices);
    adjacencyShape->indices = (GLUSuint*)glusMemoryMalloc(numberIndices * sizeof(GLUSuint));

    adjacencyShape->mode = GLUS_TRIANGLES_ADJACENCY;

    if (!glusShapeCheckCompletef(adjacencyShape))
    {
        glusShapeDestroyf(adjacencyShape);

        return GLUS_FALSE;
    }

    // Process all triangles
    for (i = 0; i < sourceShape->numberIndices / 3; i++)
    {
        // For now, all adjacent triangles are degenerated.
        adjacencyShape->indices[6 * i + 0] = sourceShape->indices[3 * i + 0];
        adjacencyShape->indices[6 * i + 1] = sourceShape->indices[3 * i + 0];
        adjacencyShape->indices[6 * i + 2] = sourceShape->indices[3 * i + 1];
        adjacencyShape->indices[6 * i + 3] = sourceShape->indices[3 * i + 1];
        adjacencyShape->indices[6 * i + 4] = sourceShape->indices[3 * i + 2];
        adjacencyShape->indices[6 * i + 5] = sourceShape->indices[3 * i + 2];

        // Skip degenerated triangles
        if (sourceShape->indices[i * 3 + 0] == sourceShape->indices[i * 3 + 1] || sourceShape->indices[i * 3 + 0] == sourceShape->indices[i * 3 + 2] || sourceShape->indices[i * 3 + 1] == sourceShape->indices[i * 3 + 2])
        {
            continue;
        }

        // Find adjacent indices for each edge of the triangle ...
        for (edge = 0; edge < 3; edge++)
        {
            adjacentIndex = 0;

            // ... by scanning the indices ...
            if (glusShapeFindIndexByIndicesf(&adjacentIndex, 3 * i, edge, sourceShape))
            {
                adjacencyShape->indices[6 * i + edge * 2 + 1] = adjacentIndex;

                continue;
            }

            // ... and if not found, compare the vertices.
            if (glusShapeFindIndexByVerticesf(&adjacentIndex, 3 * i, edge, sourceShape))
            {
                adjacencyShape->indices[6 * i + edge * 2 + 1] = adjacentIndex;

                continue;
            }

            glusLogPrint(GLUS_LOG_WARNING, "Triangle %d with edge %d: No adjacent index found!", i, edge);
        }
    }

    return GLUS_TRUE;
}
