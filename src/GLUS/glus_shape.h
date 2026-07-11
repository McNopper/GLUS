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

#ifndef GLUS_SHAPE_H_
#define GLUS_SHAPE_H_

/**
 * Structure for holding geometry data.
 */
typedef struct _GLUSshape
{
    /**
     * Vertices in homogeneous coordinates.
     */
    GLUSfloat* vertices;

    /**
     * Normals.
     */
    GLUSfloat* normals;

    /**
     * Tangents.
     */
    GLUSfloat* tangents;

    /**
     * Bitangents.
     */
    GLUSfloat* bitangents;

    /**
     * Texture coordinates.
     */
    GLUSfloat* texCoords;

    /**
     * All above values in one array. Not created by the model loader.
     */
    GLUSfloat* allAttributes;

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
     * Triangle render mode - could be either:
     *
     * GLUS_TRIANGLES
     * GLUS_TRIANGLE_STRIP
     */
    GLUSenum mode;

} GLUSshape;

/**
 * Creates a quadratic plane.
 *
 * @param shape The data is stored into this structure.
 * @param halfExtend The length from the center point to any border of the plane.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreatePlanef(GLUSshape* shape, const GLUSfloat halfExtend);

/**
 * Creates a rectangular plane.
 *
 * @param shape The data is stored into this structure.
 * @param horizontalExtend The length from the center point to the left/right border of the plane.
 * @param verticalExtend The length from the center point to the upper/lower border of the plane.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateRectangularPlanef(GLUSshape* shape, const GLUSfloat horizontalExtend, const GLUSfloat verticalExtend);

/**
 * Creates a rectangular plane organized as a grid with given number rows and columns.
 *
 * @param shape The data is stored into this structure.
 * @param horizontalExtend The length from the center point to the left/right border of the plane.
 * @param verticalExtend The length from the center point to the upper/lower border of the plane.
 * @param rows The number of rows the grid should have.
 * @param columns The number of columns the grid should have.
 * @param triangleStrip Set to GLUS_TRUE, if a triangle strip should be created.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateRectangularGridPlanef(GLUSshape* shape, const GLUSfloat horizontalExtend, const GLUSfloat verticalExtend, const GLUSuint rows, const GLUSuint columns, const GLUSboolean triangleStrip);

/**
 * Creates a disc with the given radius and number sectors. More sectors makes the disc more round.
 *
 * @param shape The data is stored into this structure.
 * @param radius The radius of the disc.
 * @param numberSectors The number of sectors the disc should have.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateDiscf(GLUSshape* shape, const GLUSfloat radius, const GLUSuint numberSectors);

/**
 * Creates a cube.
 *
 * @param shape The data is stored into this structure.
 * @param halfExtend The distance from the center point to any face of the cube.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateCubef(GLUSshape* shape, const GLUSfloat halfExtend);

/**
 * Creates a sphere. More slices makes the sphere more round.
 *
 * @param shape The data is stored into this structure.
 * @param radius The radius of the sphere.
 * @param numberSlices The number of slices the sphere should have.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateSpheref(GLUSshape* shape, const GLUSfloat radius, const GLUSuint numberSlices);

/**
 * Creates a dome. More slices makes the dome more round.
 *
 * @param shape The data is stored into this structure.
 * @param radius The radius of the dome.
 * @param numberSlices The number of slices the dome should have.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateDomef(GLUSshape* shape, const GLUSfloat radius, const GLUSuint numberSlices);

/**
 * Creates a torus / doughnut. More slices and stacks makes the torus more round.
 *
 * @param shape The data is stored into this structure.
 * @param innerRadius The inner radius of the torus. This is the distance from the center point to the closest point from the torus surface.
 * @param outerRadius The outer radius of the torus. This is the distance from the center point to the farthest point from the torus surface.
 * @param numberSlices The number of slices the torus should have.
 * @param numberStacks The number of stacks / elements the torus should have per slice.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateTorusf(GLUSshape* shape, const GLUSfloat innerRadius, const GLUSfloat outerRadius, const GLUSuint numberSlices, const GLUSuint numberStacks);

/**
 * Creates a cylinder. More slices makes the cylinder more round.
 *
 * @param shape The data is stored into this structure.
 * @param halfExtend The distance from the center point to the bottom and top disc of the cylinder.
 * @param radius The radius of the cylinder.
 * @param numberSlices The number of slices the cylinder should have.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateCylinderf(GLUSshape* shape, const GLUSfloat halfExtend, const GLUSfloat radius, const GLUSuint numberSlices);

/**
 * Creates a cone. More slices and stacks makes the torus more round and smooth.
 *
 * @param shape The data is stored into this structure.
 * @param halfExtend The distance from the center point to the bottom disc of the cone.
 * @param radius The radius of the cone at the bottom.
 * @param numberSlices The number of slices the cone should have.
 * @param numberStacks The number of stacks the cone should have.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCreateConef(GLUSshape* shape, const GLUSfloat halfExtend, const GLUSfloat radius, const GLUSuint numberSlices, const GLUSuint numberStacks);

/**
 * Calculates and creates the tangent and bitangent vectors. Uses the previous created memory for the tangents and bitangents.
 *
 * @param shape The structure which will be filled with the calculated vectors.
 *
 * @return GLUS_TRUE, if creation succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCalculateTangentBitangentf(GLUSshape* shape);

/**
 * Copies the shape.
 *
 * @param shape  The target shape.
 * @param source The source shape.
 *
 * @return GLUS_TRUE, if copy succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusShapeCopyf(GLUSshape* shape, const GLUSshape* source);

/**
 * Destroys the shape by freeing the allocated memory.
 *
 * @param shape The structure which contains the dynamic allocated shape data, which will be freed by this function.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusShapeDestroyf(GLUSshape* shape);

#endif /* GLUS_SHAPE_H_ */
