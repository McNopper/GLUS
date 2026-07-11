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

#ifndef GLUS_LINE_H_
#define GLUS_LINE_H_

/**
 * Structure for holding line data.
 */
typedef struct _GLUSline
{
    /**
     * Vertices in homogeneous coordinates.
     */
    GLUSfloat* vertices;

    /**
     * Indices.
     */
    GLUSindex* indices;

    /**
     * Number of vertices.
     */
    GLUSuint numberVertices;

    /**
     * Number of indices.
     */
    GLUSuint numberIndices;

    /**
     * Line render mode - could be either:
     *
     * GLUS_LINES
     * GLUS_LINE_STRIP
     * GLUS_LINE_LOOP
     */
    GLUSenum mode;

} GLUSline;

/**
 * Creates a line out of two points.
 *
 * @param line The data is stored into this structure.
 * @param point0 The starting point.
 * @param point1 The ending point.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusLineCreateLinef(GLUSline* line, const GLUSfloat point0[4], const GLUSfloat point1[4]);

/**
 * Creates a square out of lines.
 *
 * @param line The data is stored into this structure.
 * @param halfExtend The length from the center point to edge of the square.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusLineCreateSquaref(GLUSline* line, const GLUSfloat halfExtend);

/**
 * Creates a rectangular grid out of lines. The grid is centered.
 *
 * @param line The data is stored into this structure.
 * @param horizontalExtend The width of the grid.
 * @param verticalExtend The height of the grid.
 * @param rows Number of rows of the grid.
 * @param columns Number of columns of the grid.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusLineCreateRectangularGridf(GLUSline* line, const GLUSfloat horizontalExtend, const GLUSfloat verticalExtend, const GLUSuint rows, const GLUSuint columns);

/**
 * Creates a circle out of lines with the given radius and number sectors. More sectors makes the circle more round.
 *
 * @param line The data is stored into this structure.
 * @param radius The radius of the circle.
 * @param numberSectors The number of sectors the circle should have.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusLineCreateCirclef(GLUSline* line, const GLUSfloat radius, const GLUSuint numberSectors);

/**
 * Copies the line.
 *
 * @param line The target line.
 * @param source The source line.
 *
 * @return GLUS_TRUE, if copy succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusLineCopyf(GLUSline* line, const GLUSline* source);

/**
 * Destroys the line by freeing the allocated memory.
 *
 * @param line The structure which contains the dynamic allocated line data, which will be freed by this function.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusLineDestroyf(GLUSline* line);

#endif /* GLUS_LINE_H_ */
