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

#define GLUS_MAX_OBJECTS 1
#define GLUS_MAX_ATTRIBUTES (GLUS_MAX_VERTICES / GLUS_VERTICES_DIVISOR)
#define GLUS_MAX_TRIANGLE_ATTRIBUTES GLUS_MAX_VERTICES
#define GLUS_MAX_LINE_ATTRIBUTES GLUS_MAX_VERTICES
#define GLUS_BUFFERSIZE 1024

static GLUSvoid glusWavefrontCopyString(GLUSchar* destination, size_t destinationSize, const GLUSchar* source)
{
    size_t length;

    if (!destination || destinationSize == 0)
    {
        return;
    }

    if (!source)
    {
        destination[0] = '\0';

        return;
    }

    length = strlen(source);

    if (length > destinationSize - 1)
    {
        length = destinationSize - 1;
    }

    memcpy(destination, source, length);

    destination[length] = '\0';
}

/**
 * Resolves a Wavefront index, which is either one based or relative to the end. Returns -1, if the index is not usable.
 */
static GLUSint glusWavefrontResolveIndex(GLUSint index, GLUSuint count)
{
    if (index > 0)
    {
        index--;
    }
    else if (index < 0)
    {
        index += (GLUSint)count;
    }
    else
    {
        // Zero is not a valid Wavefront index, so the value was not given at all.

        return -1;
    }

    if (index < 0 || index >= (GLUSint)count)
    {
        return -1;
    }

    return index;
}

static GLUSboolean glusWavefrontMallocTempMemoryLine(GLUSfloat** vertices, GLUSindex** indices)
{
    if (!vertices || !indices)
    {
        return GLUS_FALSE;
    }

    *vertices = (GLUSfloat*)glusMemoryMalloc((size_t)4 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*vertices)
    {
        return GLUS_FALSE;
    }

    *indices = (GLUSindex*)glusMemoryMalloc(GLUS_MAX_LINE_ATTRIBUTES * sizeof(GLUSindex));
    if (!*indices)
    {
        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

static GLUSvoid glusWavefrontFreeTempMemoryLine(GLUSfloat** vertices, GLUSindex** indices)
{
    if (vertices && *vertices)
    {
        glusMemoryFree(*vertices);

        *vertices = 0;
    }

    if (indices && *indices)
    {
        glusMemoryFree(*indices);

        *indices = 0;
    }
}

// The six scratch buffers of the parser, bundled so allocation, cleanup and the
// parse context share one definition instead of threading six GLUSfloat**.
typedef struct GLUSwavefrontTempMemory_
{
    GLUSfloat* vertices;
    GLUSfloat* normals;
    GLUSfloat* texCoords;

    GLUSfloat* triangleVertices;
    GLUSfloat* triangleNormals;
    GLUSfloat* triangleTexCoords;
} GLUSwavefrontTempMemory;

static GLUSboolean glusWavefrontMallocTempMemory(GLUSwavefrontTempMemory* tempMemory)
{
    if (!tempMemory)
    {
        return GLUS_FALSE;
    }

    memset(tempMemory, 0, sizeof(GLUSwavefrontTempMemory));

    tempMemory->vertices = (GLUSfloat*)glusMemoryMalloc((size_t)4 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!tempMemory->vertices)
    {
        return GLUS_FALSE;
    }

    tempMemory->normals = (GLUSfloat*)glusMemoryMalloc((size_t)3 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!tempMemory->normals)
    {
        return GLUS_FALSE;
    }

    tempMemory->texCoords = (GLUSfloat*)glusMemoryMalloc((size_t)2 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!tempMemory->texCoords)
    {
        return GLUS_FALSE;
    }

    tempMemory->triangleVertices = (GLUSfloat*)glusMemoryMalloc((size_t)4 * GLUS_MAX_TRIANGLE_ATTRIBUTES * sizeof(GLUSfloat));
    if (!tempMemory->triangleVertices)
    {
        return GLUS_FALSE;
    }

    tempMemory->triangleNormals = (GLUSfloat*)glusMemoryMalloc((size_t)3 * GLUS_MAX_TRIANGLE_ATTRIBUTES * sizeof(GLUSfloat));
    if (!tempMemory->triangleNormals)
    {
        return GLUS_FALSE;
    }

    tempMemory->triangleTexCoords = (GLUSfloat*)glusMemoryMalloc((size_t)2 * GLUS_MAX_TRIANGLE_ATTRIBUTES * sizeof(GLUSfloat));
    if (!tempMemory->triangleTexCoords)
    {
        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

static GLUSvoid glusWavefrontFreeTempMemory(GLUSwavefrontTempMemory* tempMemory)
{
    if (!tempMemory)
    {
        return;
    }

    if (tempMemory->vertices)
    {
        glusMemoryFree(tempMemory->vertices);

        tempMemory->vertices = 0;
    }

    if (tempMemory->normals)
    {
        glusMemoryFree(tempMemory->normals);

        tempMemory->normals = 0;
    }

    if (tempMemory->texCoords)
    {
        glusMemoryFree(tempMemory->texCoords);

        tempMemory->texCoords = 0;
    }

    if (tempMemory->triangleVertices)
    {
        glusMemoryFree(tempMemory->triangleVertices);

        tempMemory->triangleVertices = 0;
    }

    if (tempMemory->triangleNormals)
    {
        glusMemoryFree(tempMemory->triangleNormals);

        tempMemory->triangleNormals = 0;
    }

    if (tempMemory->triangleTexCoords)
    {
        glusMemoryFree(tempMemory->triangleTexCoords);

        tempMemory->triangleTexCoords = 0;
    }
}

static GLUSvoid glusWavefrontInitMaterial(GLUSmaterial* material)
{
    if (!material)
    {
        return;
    }

    material->name[0] = 0;

    material->emissive[0] = 0.0f;
    material->emissive[1] = 0.0f;
    material->emissive[2] = 0.0f;
    material->emissive[3] = 1.0f;

    material->ambient[0] = 0.0f;
    material->ambient[1] = 0.0f;
    material->ambient[2] = 0.0f;
    material->ambient[3] = 1.0f;

    material->diffuse[0] = 0.0f;
    material->diffuse[1] = 0.0f;
    material->diffuse[2] = 0.0f;
    material->diffuse[3] = 1.0f;

    material->specular[0] = 0.0f;
    material->specular[1] = 0.0f;
    material->specular[2] = 0.0f;
    material->specular[3] = 1.0f;

    material->shininess = 0.0f;

    material->transparency = 1.0f;

    material->reflection = GLUS_FALSE;

    material->refraction = GLUS_FALSE;

    material->indexOfRefraction = 1.0f;

    material->emissiveTextureFilename[0] = 0;

    material->ambientTextureFilename[0] = 0;

    material->diffuseTextureFilename[0] = 0;

    material->specularTextureFilename[0] = 0;

    material->transparencyTextureFilename[0] = 0;

    material->bumpTextureFilename[0] = 0;

    material->emissiveTextureName = 0;

    material->ambientTextureName = 0;

    material->diffuseTextureName = 0;

    material->specularTextureName = 0;

    material->transparencyTextureName = 0;

    material->bumpTextureName = 0;
}

static GLUSvoid glusWavefrontDestroyMaterial(GLUSmaterialList** materialList)
{
    GLUSmaterialList* currentMaterialList = 0;
    GLUSmaterialList* nextMaterialList    = 0;

    if (!materialList || !*materialList)
    {
        return;
    }

    currentMaterialList = *materialList;
    while (currentMaterialList != 0)
    {
        nextMaterialList = currentMaterialList->next;

        memset(&currentMaterialList->material, 0, sizeof(GLUSmaterial));

        glusMemoryFree(currentMaterialList);

        currentMaterialList = nextMaterialList;
    }

    *materialList = 0;
}

// Numeric fields are extracted with strtof/strtol rather than sscanf: those
// report a failed conversion through endptr, which sscanf cannot. Every target
// is pre-initialised by the caller, so a missing or malformed field keeps the
// previous value - the same best-effort behaviour the sscanf calls had.
static const GLUSchar* _glusWavefrontSkipField(const GLUSchar* cursor)
{
    while (*cursor == ' ' || *cursor == '\t')
    {
        cursor++;
    }
    while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && *cursor != '/')
    {
        cursor++;
    }

    return cursor;
}

static const GLUSchar* _glusWavefrontSkipSeparator(const GLUSchar* cursor)
{
    while (*cursor == ' ' || *cursor == '\t' || *cursor == '/')
    {
        cursor++;
    }

    return cursor;
}

static const GLUSchar* _glusWavefrontReadFloat(const GLUSchar* cursor, GLUSfloat* value)
{
    GLUSchar* end = 0;

    cursor = _glusWavefrontSkipSeparator(cursor);

    *value = strtof(cursor, &end);

    return end != cursor ? end : _glusWavefrontSkipField(cursor);
}

static const GLUSchar* _glusWavefrontReadInt(const GLUSchar* cursor, GLUSint* value)
{
    GLUSchar* end = 0;

    cursor = _glusWavefrontSkipSeparator(cursor);

    *value = (GLUSint)strtol(cursor, &end, 10);

    return end != cursor ? end : _glusWavefrontSkipField(cursor);
}

// Reads the three RGB channels of a color record and forces alpha to opaque.
static GLUSvoid glusWavefrontReadColor4f(const GLUSchar* checkBuffer, GLUSfloat color[4])
{
    const GLUSchar* cursor = _glusWavefrontSkipField(checkBuffer);

    cursor = _glusWavefrontReadFloat(cursor, &color[0]);
    cursor = _glusWavefrontReadFloat(cursor, &color[1]);

    _glusWavefrontReadFloat(cursor, &color[2]);

    color[3] = 1.0f;
}

// Reads a "map_*" texture record and copies the filename into the material field.
static GLUSvoid glusWavefrontReadTextureFilename(const GLUSchar* checkBuffer, GLUSchar* filename, size_t filenameSize)
{
    GLUSchar identifier[32]; /* scratch - the keyword itself was already dispatched on */
    GLUSchar name[GLUS_MAX_STRING];

    name[0] = '\0';

    sscanf(checkBuffer, "%31s %255s", identifier, name);

    glusWavefrontCopyString(filename, filenameSize, name);
}

static GLUSboolean glusWavefrontLoadMaterial(const GLUSchar* filename, GLUSmaterialList** materialList)
{
    FILE* f;

    GLUSint i, k;

    GLUSchar  buffer[GLUS_BUFFERSIZE];
    GLUSchar* checkBuffer;
    GLUSchar  name[GLUS_MAX_STRING];
    GLUSchar  identifier[32]; /* was [7]; "map_bump" is 8 chars + null = 9 bytes */

    // currentMaterialList is NULL until the first `newmtl` *in this file* - that
    // is what the "without a material" guard below keys off. Appending has to
    // continue from the tail of whatever the caller already holds, though: this
    // function runs once per `mtllib`, and on a second one *materialList is
    // already non-empty while currentMaterialList is still NULL, so
    // `currentMaterialList->next = ...` below wrote straight through a NULL
    // pointer. Keep a separate cursor for the tail.
    GLUSmaterialList* currentMaterialList = 0;
    GLUSmaterialList* lastMaterialList   = 0;

    if (!filename || !materialList)
    {
        return GLUS_FALSE;
    }

    lastMaterialList = *materialList;

    while (lastMaterialList != 0 && lastMaterialList->next != 0)
    {
        lastMaterialList = lastMaterialList->next;
    }

    f = glusFileOpen(filename, "r");

    if (!f)
    {
        return GLUS_FALSE;
    }

    while (!feof(f))
    {
        buffer[0] = 0;

        if (fgets(buffer, GLUS_BUFFERSIZE, f) == 0)
        {
            if (ferror(f))
            {
                glusFileClose(f);

                return GLUS_FALSE;
            }
        }

        checkBuffer = buffer;

        k = 0;

        // Skip first spaces etc.
        while (*checkBuffer)
        {
            if (*checkBuffer != ' ' && *checkBuffer != '\t')
            {
                break;
            }

            checkBuffer++;
            k++;

            if (k >= GLUS_BUFFERSIZE)
            {
                glusFileClose(f);

                return GLUS_FALSE;
            }
        }

        i = 0;

        while (checkBuffer[i])
        {
            if (checkBuffer[i] == ' ' || checkBuffer[i] == '\t')
            {
                break;
            }

            checkBuffer[i] = (GLUSchar)tolower((unsigned char)checkBuffer[i]);

            i++;

            if (i >= GLUS_BUFFERSIZE - k)
            {
                glusFileClose(f);

                return GLUS_FALSE;
            }
        }

        if (strncmp(checkBuffer, "newmtl", 6) == 0)
        {
            GLUSmaterialList* newMaterialList = 0;

            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            newMaterialList = (GLUSmaterialList*)glusMemoryMalloc(sizeof(GLUSmaterialList));

            if (!newMaterialList)
            {
                glusWavefrontDestroyMaterial(materialList);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            memset(newMaterialList, 0, sizeof(GLUSmaterialList));

            glusWavefrontInitMaterial(&newMaterialList->material);

            glusWavefrontCopyString(newMaterialList->material.name, sizeof(newMaterialList->material.name), name);

            if (*materialList == 0)
            {
                *materialList = newMaterialList;
            }
            else
            {
                lastMaterialList->next = newMaterialList;
            }

            lastMaterialList    = newMaterialList;
            currentMaterialList = newMaterialList;
        }
        else if (currentMaterialList == 0)
        {
            // Without a material, there is nothing the following entries could be stored in.

            continue;
        }
        else if (strncmp(checkBuffer, "ke", 2) == 0)
        {
            glusWavefrontReadColor4f(checkBuffer, currentMaterialList->material.emissive);
        }
        else if (strncmp(checkBuffer, "ka", 2) == 0)
        {
            glusWavefrontReadColor4f(checkBuffer, currentMaterialList->material.ambient);
        }
        else if (strncmp(checkBuffer, "kd", 2) == 0)
        {
            glusWavefrontReadColor4f(checkBuffer, currentMaterialList->material.diffuse);
        }
        else if (strncmp(checkBuffer, "ks", 2) == 0)
        {
            glusWavefrontReadColor4f(checkBuffer, currentMaterialList->material.specular);
        }
        else if (strncmp(checkBuffer, "ns", 2) == 0)
        {
            _glusWavefrontReadFloat(_glusWavefrontSkipField(checkBuffer), &currentMaterialList->material.shininess);
        }
        // The "d" keyword must be followed by a separator - otherwise the
        // unhandled "decal" and "disp" statements match and the failed float
        // read clobbers transparency to 0, making the material invisible.
        else if ((strncmp(checkBuffer, "d", 1) == 0 && (checkBuffer[1] == ' ' || checkBuffer[1] == '\t')) || strncmp(checkBuffer, "tr", 2) == 0)
        {
            _glusWavefrontReadFloat(_glusWavefrontSkipField(checkBuffer), &currentMaterialList->material.transparency);
        }
        else if (strncmp(checkBuffer, "ni", 2) == 0)
        {
            _glusWavefrontReadFloat(_glusWavefrontSkipField(checkBuffer), &currentMaterialList->material.indexOfRefraction);
        }
        else if (strncmp(checkBuffer, "map_ke", 6) == 0)
        {
            glusWavefrontReadTextureFilename(checkBuffer, currentMaterialList->material.emissiveTextureFilename, sizeof(currentMaterialList->material.emissiveTextureFilename));
        }
        else if (strncmp(checkBuffer, "map_ka", 6) == 0)
        {
            glusWavefrontReadTextureFilename(checkBuffer, currentMaterialList->material.ambientTextureFilename, sizeof(currentMaterialList->material.ambientTextureFilename));
        }
        else if (strncmp(checkBuffer, "map_kd", 6) == 0)
        {
            glusWavefrontReadTextureFilename(checkBuffer, currentMaterialList->material.diffuseTextureFilename, sizeof(currentMaterialList->material.diffuseTextureFilename));
        }
        else if (strncmp(checkBuffer, "map_ks", 6) == 0)
        {
            glusWavefrontReadTextureFilename(checkBuffer, currentMaterialList->material.specularTextureFilename, sizeof(currentMaterialList->material.specularTextureFilename));
        }
        // Same prefix hazard as "d" above: "map_disp" must not match "map_d".
        else if ((strncmp(checkBuffer, "map_d", 5) == 0 && (checkBuffer[5] == ' ' || checkBuffer[5] == '\t')) || strncmp(checkBuffer, "map_tr", 6) == 0)
        {
            glusWavefrontReadTextureFilename(checkBuffer, currentMaterialList->material.transparencyTextureFilename, sizeof(currentMaterialList->material.transparencyTextureFilename));
        }
        else if (strncmp(checkBuffer, "map_bump", 8) == 0 || strncmp(checkBuffer, "bump", 4) == 0)
        {
            glusWavefrontReadTextureFilename(checkBuffer, currentMaterialList->material.bumpTextureFilename, sizeof(currentMaterialList->material.bumpTextureFilename));
        }
        else if (strncmp(checkBuffer, "illum", 5) == 0)
        {
            GLUSint illum;

            illum = 0;

            _glusWavefrontReadInt(_glusWavefrontSkipField(checkBuffer), &illum);

            // Only setting reflection and refraction depending on illumination model.
            switch (illum)
            {
            case 3:
            case 4:
            case 5:
            case 8:
            case 9:
                currentMaterialList->material.reflection = GLUS_TRUE;
                break;
            case 6:
            case 7:
                currentMaterialList->material.reflection = GLUS_TRUE;
                currentMaterialList->material.refraction = GLUS_TRUE;
                break;
            default:
                break; // Illumination models 0-2 and 10+: neither reflection nor refraction.
            }
        }
    }

    glusFileClose(f);

    return GLUS_TRUE;
}

static GLUSvoid glusWavefrontDestroyGroup(GLUSgroupList** groupList)
{
    GLUSgroupList* currentGroupList = 0;
    GLUSgroupList* nextGroupList    = 0;

    if (!groupList || !*groupList)
    {
        return;
    }

    currentGroupList = *groupList;
    while (currentGroupList != 0)
    {
        nextGroupList = currentGroupList->next;

        if (currentGroupList->group.indices)
        {
            glusMemoryFree(currentGroupList->group.indices);
        }
        memset(&currentGroupList->group, 0, sizeof(GLUSgroup));

        glusMemoryFree(currentGroupList);

        currentGroupList = nextGroupList;
    }

    *groupList = 0;
}

static GLUSboolean glusWavefrontCopyDataLine(GLUSline* line, GLUSuint totalNumberVertices, GLUSfloat* lineVertices, GLUSuint totalNumberIndices, GLUSindex* lineIndices)
{
    if (!line || !lineVertices || !lineIndices)
    {
        return GLUS_FALSE;
    }

    memset(line, 0, sizeof(GLUSline));

    line->numberVertices = totalNumberVertices;
    line->numberIndices  = totalNumberIndices;

    if (totalNumberVertices > 0)
    {
        line->vertices = (GLUSfloat*)glusMemoryMalloc((size_t)totalNumberVertices * 4 * sizeof(GLUSfloat));

        if (line->vertices == 0)
        {
            glusLineDestroyf(line);

            return GLUS_FALSE;
        }

        memcpy(line->vertices, lineVertices, (size_t)totalNumberVertices * 4 * sizeof(GLUSfloat));
    }
    if (totalNumberIndices > 0)
    {
        line->indices = (GLUSindex*)glusMemoryMalloc(totalNumberIndices * sizeof(GLUSindex));

        if (line->indices == 0)
        {
            glusLineDestroyf(line);

            return GLUS_FALSE;
        }

        memcpy(line->indices, lineIndices, totalNumberIndices * sizeof(GLUSindex));
    }

    line->mode = GLUS_LINES;

    return GLUS_TRUE;
}

static GLUSboolean glusWavefrontCopyData(GLUSshape* shape, GLUSuint totalNumberVertices, GLUSfloat* triangleVertices, GLUSuint totalNumberNormals, GLUSfloat* triangleNormals, GLUSuint totalNumberTexCoords, GLUSfloat* triangleTexCoords)
{
    GLUSuint indicesCounter = 0;

    if (!shape || !triangleVertices || !triangleNormals || !triangleTexCoords)
    {
        return GLUS_FALSE;
    }

    memset(shape, 0, sizeof(GLUSshape));

    shape->numberVertices = totalNumberVertices;

    if (totalNumberVertices > 0)
    {
        shape->vertices = (GLUSfloat*)glusMemoryMalloc((size_t)totalNumberVertices * 4 * sizeof(GLUSfloat));

        if (shape->vertices == 0)
        {
            glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }

        memcpy(shape->vertices, triangleVertices, (size_t)totalNumberVertices * 4 * sizeof(GLUSfloat));
    }
    if (totalNumberNormals > 0)
    {
        GLUSuint copyNormals = totalNumberNormals < totalNumberVertices ? totalNumberNormals : totalNumberVertices;
        GLUSuint i;

        // Sized to numberVertices rather than to the number of `vn` references:
        // the tangent pass and glusShapeCopyf index this array by vertex index, so
        // a stream left short by faces that omit `vn` used to be read past its end.
        // The tail is filled with a defined default normal so the file still loads
        // - rejecting it outright would break every OBJ that mixes `f v/vt/vn` with
        // `f v`, which is common and legal. A stream absent as a whole still stays
        // NULL (the zero count above).
        shape->normals = (GLUSfloat*)glusMemoryMalloc((size_t)totalNumberVertices * 3 * sizeof(GLUSfloat));

        if (shape->normals == 0)
        {
            glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }

        memcpy(shape->normals, triangleNormals, (size_t)copyNormals * 3 * sizeof(GLUSfloat));

        for (i = copyNormals; i < totalNumberVertices; i++)
        {
            shape->normals[3 * i + 0] = 0.0f;
            shape->normals[3 * i + 1] = 0.0f;
            shape->normals[3 * i + 2] = 1.0f;
        }
    }
    if (totalNumberTexCoords > 0)
    {
        GLUSuint copyTexCoords = totalNumberTexCoords < totalNumberVertices ? totalNumberTexCoords : totalNumberVertices;
        GLUSuint i;

        shape->texCoords = (GLUSfloat*)glusMemoryMalloc((size_t)totalNumberVertices * 2 * sizeof(GLUSfloat));

        if (shape->texCoords == 0)
        {
            glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }

        memcpy(shape->texCoords, triangleTexCoords, (size_t)copyTexCoords * 2 * sizeof(GLUSfloat));

        for (i = copyTexCoords; i < totalNumberVertices; i++)
        {
            shape->texCoords[2 * i + 0] = 0.0f;
            shape->texCoords[2 * i + 1] = 0.0f;
        }
    }

    // Just create the indices from the list of vertices.

    shape->numberIndices = totalNumberVertices;

    if (totalNumberVertices > 0)
    {
        shape->indices = (GLUSindex*)glusMemoryMalloc(totalNumberVertices * sizeof(GLUSindex));

        if (shape->indices == 0)
        {
            glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }

        for (indicesCounter = 0; indicesCounter < totalNumberVertices; indicesCounter++)
        {
            shape->indices[indicesCounter] = indicesCounter;
        }
    }

    shape->mode = GLUS_TRIANGLES;

    return GLUS_TRUE;
}

GLUSboolean _glusWavefrontMove(GLUSwavefront* wavefront, GLUSshape* shape)
{
    GLUSmaterialList* materialWalker;
    GLUSgroupList*    groupWalker;

    GLUSuint i;
    GLUSuint counter = 0;

    if (!wavefront || !shape)
    {
        return GLUS_FALSE;
    }

    // No clear of wavefront by purpose.

    wavefront->vertices       = shape->vertices;
    wavefront->normals        = shape->normals;
    wavefront->texCoords      = shape->texCoords;
    wavefront->tangents       = shape->tangents;
    wavefront->bitangents     = shape->bitangents;
    wavefront->numberVertices = shape->numberVertices;

    groupWalker = wavefront->groups;
    while (groupWalker)
    {
        groupWalker->group.indices = (GLUSindex*)glusMemoryMalloc(groupWalker->group.numberIndices * sizeof(GLUSindex));

        if (!groupWalker->group.indices)
        {
            // The vertex buffers have already been moved, so they have to be freed here.

            glusMemoryFree(wavefront->vertices);
            glusMemoryFree(wavefront->normals);
            glusMemoryFree(wavefront->texCoords);
            glusMemoryFree(wavefront->tangents);
            glusMemoryFree(wavefront->bitangents);

            wavefront->vertices       = 0;
            wavefront->normals        = 0;
            wavefront->texCoords      = 0;
            wavefront->tangents       = 0;
            wavefront->bitangents     = 0;
            wavefront->numberVertices = 0;

            glusMemoryFree(shape->indices);

            memset(shape, 0, sizeof(GLUSshape));

            return GLUS_FALSE;
        }

        for (i = 0; i < groupWalker->group.numberIndices; i++)
        {
            groupWalker->group.indices[i] = counter++;
        }

        materialWalker = wavefront->materials;

        while (materialWalker)
        {
            if (strcmp(materialWalker->material.name, groupWalker->group.materialName) == 0)
            {
                groupWalker->group.material = &materialWalker->material;

                break;
            }

            materialWalker = materialWalker->next;
        }

        groupWalker = groupWalker->next;
    }

    glusMemoryFree(shape->indices);
    shape->indices = 0;

    memset(shape, 0, sizeof(GLUSshape));

    return GLUS_TRUE;
}

// Emits one resolved face-corner attribute into the triangle stream, fanning out
// to a triangle when the face has more than three corners. Returns false when
// the stream is full.
static GLUSboolean _glusWavefrontEmitFaceAttribute(GLUSfloat* triangleAttributes, const GLUSfloat* attributes, GLUSuint stride, GLUSint index, GLUSuint* totalNumberAttributes, GLUSuint* emittedAttributes, GLUSuint* emitted)
{
    if (*emittedAttributes < 3)
    {
        if (*totalNumberAttributes >= GLUS_MAX_TRIANGLE_ATTRIBUTES)
        {
            return GLUS_FALSE;
        }

        memcpy(&triangleAttributes[(size_t)stride * *totalNumberAttributes], &attributes[(ptrdiff_t)stride * index], stride * sizeof(GLUSfloat));

        (*totalNumberAttributes)++;
        (*emittedAttributes)++;

        *emitted = 1;
    }
    else
    {
        if (*totalNumberAttributes >= GLUS_MAX_TRIANGLE_ATTRIBUTES - 2)
        {
            return GLUS_FALSE;
        }

        memcpy(&triangleAttributes[(size_t)stride * *totalNumberAttributes], &triangleAttributes[(size_t)stride * (*totalNumberAttributes - *emittedAttributes)], stride * sizeof(GLUSfloat));
        memcpy(&triangleAttributes[(size_t)stride * (*totalNumberAttributes + 1)], &triangleAttributes[(size_t)stride * (*totalNumberAttributes - 1)], stride * sizeof(GLUSfloat));
        memcpy(&triangleAttributes[(size_t)stride * (*totalNumberAttributes + 2)], &attributes[(ptrdiff_t)stride * index], stride * sizeof(GLUSfloat));

        *totalNumberAttributes += 3;
        *emittedAttributes += 3;

        *emitted = 3;
    }

    return GLUS_TRUE;
}

// Face vertex-index encodings accepted by the OBJ format:
// v, v/vt, v//vn and v/vt/vn.
typedef enum GLUSwavefrontFaceEncoding_
{
    GLUSWF_FACE_V       = 0,
    GLUSWF_FACE_V_VT    = 1,
    GLUSWF_FACE_V_VN    = 2,
    GLUSWF_FACE_V_VT_VN = 3
} GLUSwavefrontFaceEncoding;

// Parser state shared by the per-record handlers. Bundling it gives the main
// loop a single cleanup path: a handler reports failure and the loop owner
// releases the file and the temp memory in exactly one place.
typedef struct GLUSwavefrontParseContext_
{
    FILE* f;

    GLUSshape*     shape;
    GLUSwavefront* wavefront;
    GLUSscene*     scene;

    GLUSwavefrontTempMemory tempMemory;

    GLUSuint numberVertices;
    GLUSuint numberNormals;
    GLUSuint numberTexCoords;

    GLUSuint offsetNumberVertices;
    GLUSuint offsetNumberNormals;
    GLUSuint offsetNumberTexCoords;

    GLUSuint totalNumberVertices;
    GLUSuint totalNumberNormals;
    GLUSuint totalNumberTexCoords;

    GLUSwavefrontFaceEncoding facesEncoding;

    GLUSuint numberIndicesGroup;
    GLUSuint numberMaterials;
    GLUSuint numberGroups;
    GLUSuint numberObjects;

    GLUSgroupList*  currentGroupList;
    GLUSobjectList* currentObjectList;
} GLUSwavefrontParseContext;

static GLUSboolean _glusWavefrontHandleMtllib(GLUSwavefrontParseContext* ctx, GLUSchar* buffer)
{
    GLUSchar identifier[32];
    GLUSchar name[GLUS_MAX_STRING] = {'\0'};

    if (sscanf(buffer, "%31s %255s", identifier, name) != 2)
    {
        // Without a file name there is nothing to load; the line is skipped.
        return GLUS_TRUE;
    }

    if (ctx->numberMaterials == 0)
    {
        ctx->wavefront->materials = 0;
    }

    if (!glusWavefrontLoadMaterial(name, &ctx->wavefront->materials))
    {
        return GLUS_FALSE;
    }

    ctx->numberMaterials++;

    return GLUS_TRUE;
}

// Appends a new group node to the wavefront's chain and makes it current. The
// usemtl handler and the g/o records share this creation path.
static GLUSboolean _glusWavefrontAppendGroup(GLUSwavefrontParseContext* ctx, const GLUSchar* name)
{
    GLUSgroupList* newGroupList = (GLUSgroupList*)glusMemoryMalloc(sizeof(GLUSgroupList));

    if (!newGroupList)
    {
        return GLUS_FALSE;
    }

    memset(newGroupList, 0, sizeof(GLUSgroupList));

    glusWavefrontCopyString(newGroupList->group.name, sizeof(newGroupList->group.name), name);

    if (ctx->numberGroups == 0)
    {
        if (!ctx->wavefront)
        {
            glusMemoryFree(newGroupList);

            return GLUS_FALSE;
        }

        ctx->wavefront->groups = newGroupList;
    }
    else
    {
        if (!ctx->currentGroupList)
        {
            glusMemoryFree(newGroupList);

            return GLUS_FALSE;
        }

        ctx->currentGroupList->next = newGroupList;

        ctx->currentGroupList->group.numberIndices = ctx->numberIndicesGroup;
        ctx->numberIndicesGroup                    = 0;
    }

    ctx->currentGroupList = newGroupList;

    ctx->numberGroups++;

    return GLUS_TRUE;
}

static GLUSboolean _glusWavefrontHandleUsemtl(GLUSwavefrontParseContext* ctx, GLUSchar* buffer)
{
    GLUSchar identifier[32];
    GLUSchar name[GLUS_MAX_STRING] = {'\0'};

    // Parse the material name first, as it is needed for the group as well.
    sscanf(buffer, "%31s %255s", identifier, name);

    if (!ctx->currentGroupList || ctx->currentGroupList->group.materialName[0] != '\0')
    {
        if (!_glusWavefrontAppendGroup(ctx, name))
        {
            return GLUS_FALSE;
        }
    }

    //

    if (!ctx->currentGroupList)
    {
        // Unreachable in practice: the branch above only runs when a group was
        // just created and currentGroupList was set to it. Stated explicitly
        // anyway so the dereference below is provably safe rather than relying
        // on that reasoning.
        return GLUS_FALSE;
    }

    glusWavefrontCopyString(ctx->currentGroupList->group.materialName, sizeof(ctx->currentGroupList->group.materialName), name);

    return GLUS_TRUE;
}

// Closes the object currently accumulated: copies the triangle streams into the
// shape and moves the shape into the wavefront, then snapshots the wavefront
// into the object node.
static GLUSboolean _glusWavefrontFlushObject(GLUSwavefrontParseContext* ctx)
{
    if (ctx->currentObjectList)
    {
        GLUSboolean copyResult;

        if (ctx->wavefront && ctx->currentGroupList)
        {
            ctx->currentGroupList->group.numberIndices = ctx->numberIndicesGroup;
            ctx->numberIndicesGroup                    = 0; // NOLINT(clang-analyzer-deadcode.DeadStores) - defensive reset; the loop reassigns before the next read
        }

        copyResult = glusWavefrontCopyData(ctx->shape, ctx->totalNumberVertices - ctx->offsetNumberVertices, &ctx->tempMemory.triangleVertices[(size_t)4 * ctx->offsetNumberVertices], ctx->totalNumberNormals - ctx->offsetNumberNormals, &ctx->tempMemory.triangleNormals[(size_t)3 * ctx->offsetNumberNormals], ctx->totalNumberTexCoords - ctx->offsetNumberTexCoords, &ctx->tempMemory.triangleTexCoords[(size_t)2 * ctx->offsetNumberTexCoords]);

        if (copyResult)
        {
            glusShapeCalculateTangentBitangentf(ctx->shape);
        }

        if (!_glusWavefrontMove(ctx->wavefront, ctx->shape))
        {
            return GLUS_FALSE;
        }

        memcpy(&ctx->currentObjectList->object, ctx->wavefront, sizeof(GLUSwavefront));

        // The snapshot above is a shallow copy, so it owns the group chain from
        // here on and the live wavefront has to let go of it. If the next object
        // has faces but no `g`/`usemtl`, the next _glusWavefrontMove would walk
        // these nodes again: it reallocates group.indices that the snapshot still
        // references and hands the same node to two objects, which
        // glusWavefrontDestroyScene then frees twice.
        ctx->wavefront->groups = 0;
    }

    return GLUS_TRUE;
}

static GLUSboolean _glusWavefrontHandleObject(GLUSwavefrontParseContext* ctx, GLUSchar* buffer)
{
    GLUSchar identifier[32];
    GLUSchar name[GLUS_MAX_STRING] = {'\0'};

    if (ctx->scene)
    {
        if (!_glusWavefrontFlushObject(ctx))
        {
            return GLUS_FALSE;
        }
    }

    sscanf(buffer, "%31s %255s", identifier, name);

    if (ctx->scene)
    {
        GLUSobjectList* newObjectList;

        glusWavefrontCopyString(ctx->wavefront->name, sizeof(ctx->wavefront->name), name);

        // Always create a new object.

        newObjectList = (GLUSobjectList*)glusMemoryMalloc(sizeof(GLUSobjectList));
        if (!newObjectList)
        {
            return GLUS_FALSE;
        }
        newObjectList->next = 0;

        // Link together.
        if (ctx->currentObjectList)
        {
            ctx->currentObjectList->next = newObjectList;
        }
        ctx->currentObjectList = newObjectList;

        // Set as root, if needed.
        if (ctx->scene->objectList == 0)
        {
            ctx->scene->objectList = ctx->currentObjectList;
        }

        // Remember offset and reset values.

        ctx->offsetNumberVertices  = ctx->totalNumberVertices;
        ctx->offsetNumberNormals   = ctx->totalNumberNormals;
        ctx->offsetNumberTexCoords = ctx->totalNumberTexCoords;

        ctx->numberGroups = 0;

        ctx->currentGroupList = 0;

        // Reset unconditionally. The flush above only runs while a group is
        // open, so faces emitted outside any `g`/`usemtl` left their count
        // behind and it was credited to the next object's first group - whose
        // index range then ran past this object's vertex count at draw time.
        ctx->numberIndicesGroup = 0;
    }
    else if (ctx->wavefront)
    {
        // Without a scene an "o" record starts a fresh group chain, like "g".
        if (!_glusWavefrontAppendGroup(ctx, name))
        {
            return GLUS_FALSE;
        }
    }
    else
    {
        if (ctx->numberObjects == GLUS_MAX_OBJECTS)
        {
            return GLUS_FALSE;
        }
    }

    ctx->numberObjects++;

    return GLUS_TRUE;
}

static GLUSboolean _glusWavefrontHandleVertex(GLUSwavefrontParseContext* ctx, const GLUSchar* buffer)
{
    // Initialized: the sscanf below is not required to fill every conversion, and
    // a truncated `v`/`vt` line then wrote stack garbage into the attribute arrays.
    GLUSfloat x = 0.0f, y = 0.0f, z = 0.0f;

    if (ctx->numberVertices == GLUS_MAX_ATTRIBUTES)
    {
        return GLUS_FALSE;
    }

    {
        const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

        cursor = _glusWavefrontReadFloat(cursor, &x);
        cursor = _glusWavefrontReadFloat(cursor, &y);

        _glusWavefrontReadFloat(cursor, &z);
    }

    ctx->tempMemory.vertices[4 * ctx->numberVertices + 0] = x;
    ctx->tempMemory.vertices[4 * ctx->numberVertices + 1] = y;
    ctx->tempMemory.vertices[4 * ctx->numberVertices + 2] = z;
    ctx->tempMemory.vertices[4 * ctx->numberVertices + 3] = 1.0f;

    ctx->numberVertices++;

    return GLUS_TRUE;
}

static GLUSboolean _glusWavefrontHandleNormal(GLUSwavefrontParseContext* ctx, const GLUSchar* buffer)
{
    // Same truncated-line hazard as the vertex reader above.
    GLUSfloat x = 0.0f, y = 0.0f, z = 0.0f;

    if (ctx->numberNormals == GLUS_MAX_ATTRIBUTES)
    {
        return GLUS_FALSE;
    }

    {
        const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

        cursor = _glusWavefrontReadFloat(cursor, &x);
        cursor = _glusWavefrontReadFloat(cursor, &y);

        _glusWavefrontReadFloat(cursor, &z);
    }

    ctx->tempMemory.normals[3 * ctx->numberNormals + 0] = x;
    ctx->tempMemory.normals[3 * ctx->numberNormals + 1] = y;
    ctx->tempMemory.normals[3 * ctx->numberNormals + 2] = z;

    ctx->numberNormals++;

    return GLUS_TRUE;
}

static GLUSboolean _glusWavefrontHandleTexCoord(GLUSwavefrontParseContext* ctx, const GLUSchar* buffer)
{
    // Same truncated-line hazard as the vertex reader above.
    GLUSfloat s = 0.0f, t = 0.0f;

    if (ctx->numberTexCoords == GLUS_MAX_ATTRIBUTES)
    {
        return GLUS_FALSE;
    }

    {
        const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

        cursor = _glusWavefrontReadFloat(cursor, &s);

        _glusWavefrontReadFloat(cursor, &t);
    }

    ctx->tempMemory.texCoords[2 * ctx->numberTexCoords + 0] = s;
    ctx->tempMemory.texCoords[2 * ctx->numberTexCoords + 1] = t;

    ctx->numberTexCoords++;

    return GLUS_TRUE;
}

static GLUSboolean _glusWavefrontHandleFace(GLUSwavefrontParseContext* ctx, GLUSchar* buffer)
{
    GLUSchar* token;

    GLUSint vIndex, vtIndex, vnIndex;

    GLUSuint emittedVertices  = 0;
    GLUSuint emittedNormals   = 0;
    GLUSuint emittedTexCoords = 0;

    // Deliberately discarded: this call only primes strtok() so the next
    // one can read the first *vertex* token - the leading "f" itself is
    // not wanted.
    strtok(buffer, " \t");
    token = strtok(0, " \n");

    if (!token)
    {
        return GLUS_TRUE;
    }

    // Check faces
    if (strstr(token, "//") != 0)
    {
        ctx->facesEncoding = GLUSWF_FACE_V_VN;
    }
    else if (strstr(token, "/") == 0)
    {
        ctx->facesEncoding = GLUSWF_FACE_V;
    }
    else if (strstr(token, "/") != 0)
    {
        GLUSchar* c = strstr(token, "/");

        c++;

        if (!c)
        {
            return GLUS_TRUE;
        }

        if (strstr(c, "/") == 0)
        {
            ctx->facesEncoding = GLUSWF_FACE_V_VT;
        }
        else
        {
            ctx->facesEncoding = GLUSWF_FACE_V_VT_VN;
        }
    }

    while (token != 0)
    {
        GLUSuint emitted = 0;

        vIndex  = 0;
        vtIndex = 0;
        vnIndex = 0;

        switch (ctx->facesEncoding)
        {
        case GLUSWF_FACE_V:
            _glusWavefrontReadInt(token, &vIndex);
            break;
        case GLUSWF_FACE_V_VT:
            {
                const GLUSchar* cursor = _glusWavefrontReadInt(token, &vIndex);

                _glusWavefrontReadInt(cursor, &vtIndex);
            }
            break;
        case GLUSWF_FACE_V_VN:
            {
                const GLUSchar* cursor = _glusWavefrontReadInt(token, &vIndex);

                _glusWavefrontReadInt(cursor, &vnIndex);
            }
            break;
        case GLUSWF_FACE_V_VT_VN:
            {
                const GLUSchar* cursor = _glusWavefrontReadInt(token, &vIndex);

                cursor = _glusWavefrontReadInt(cursor, &vtIndex);
                _glusWavefrontReadInt(cursor, &vnIndex);
            }
            break;
        default:
            break; // Unreachable: facesEncoding is one of the four encodings above.
        }

        // Resolve one based and relative indices and reject everything out of bounds.

        vIndex  = glusWavefrontResolveIndex(vIndex, ctx->numberVertices);
        vtIndex = glusWavefrontResolveIndex(vtIndex, ctx->numberTexCoords);
        vnIndex = glusWavefrontResolveIndex(vnIndex, ctx->numberNormals);

        if (vIndex >= 0)
        {
            if (!_glusWavefrontEmitFaceAttribute(ctx->tempMemory.triangleVertices, ctx->tempMemory.vertices, 4, vIndex, &ctx->totalNumberVertices, &emittedVertices, &emitted))
            {
                return GLUS_FALSE;
            }

            ctx->numberIndicesGroup += emitted;
        }
        if (vnIndex >= 0)
        {
            if (!_glusWavefrontEmitFaceAttribute(ctx->tempMemory.triangleNormals, ctx->tempMemory.normals, 3, vnIndex, &ctx->totalNumberNormals, &emittedNormals, &emitted))
            {
                return GLUS_FALSE;
            }
        }
        if (vtIndex >= 0)
        {
            if (!_glusWavefrontEmitFaceAttribute(ctx->tempMemory.triangleTexCoords, ctx->tempMemory.texCoords, 2, vtIndex, &ctx->totalNumberTexCoords, &emittedTexCoords, &emitted))
            {
                return GLUS_FALSE;
            }
        }

        token = strtok(0, " \n");
    }

    return GLUS_TRUE;
}

GLUSboolean _glusWavefrontParse(const GLUSchar* filename, GLUSshape* shape, GLUSwavefront* wavefront, GLUSscene* scene)
{
    GLUSboolean result;

    GLUSchar buffer[GLUS_BUFFERSIZE];

    GLUSwavefrontParseContext context;

    GLUSwavefrontParseContext* ctx = &context;

    if (scene)
    {
        memset(scene, 0, sizeof(GLUSscene));
    }

    if (wavefront)
    {
        memset(wavefront, 0, sizeof(GLUSwavefront));
    }

    if (shape)
    {
        memset(shape, 0, sizeof(GLUSshape));
    }

    if (!filename || !shape)
    {
        return GLUS_FALSE;
    }

    memset(&context, 0, sizeof(GLUSwavefrontParseContext));

    ctx->shape     = shape;
    ctx->wavefront = wavefront;
    ctx->scene     = scene;

    ctx->f = glusFileOpen(filename, "r");

    if (!ctx->f)
    {
        return GLUS_FALSE;
    }

    if (!glusWavefrontMallocTempMemory(&ctx->tempMemory))
    {
        glusFileClose(ctx->f);

        return GLUS_FALSE;
    }

    result = GLUS_TRUE;

    while (!feof(ctx->f) && result)
    {
        buffer[0] = 0;

        if (fgets(buffer, GLUS_BUFFERSIZE, ctx->f) == 0)
        {
            if (ferror(ctx->f))
            {
                result = GLUS_FALSE;

                break;
            }
        }

        if (wavefront && strncmp(buffer, "mtllib", 6) == 0)
        {
            result = _glusWavefrontHandleMtllib(ctx, buffer);
        }
        else if (wavefront && strncmp(buffer, "usemtl", 6) == 0)
        {
            result = _glusWavefrontHandleUsemtl(ctx, buffer);
        }
        else if (wavefront && strncmp(buffer, "g", 1) == 0)
        {
            GLUSchar identifier[32];
            GLUSchar name[GLUS_MAX_STRING] = {'\0'};

            sscanf(buffer, "%31s %255s", identifier, name);

            result = _glusWavefrontAppendGroup(ctx, name);
        }
        else if (strncmp(buffer, "o", 1) == 0)
        {
            result = _glusWavefrontHandleObject(ctx, buffer);
        }
        else if (strncmp(buffer, "vt", 2) == 0)
        {
            result = _glusWavefrontHandleTexCoord(ctx, buffer);
        }
        else if (strncmp(buffer, "vn", 2) == 0)
        {
            result = _glusWavefrontHandleNormal(ctx, buffer);
        }
        else if (strncmp(buffer, "v", 1) == 0)
        {
            result = _glusWavefrontHandleVertex(ctx, buffer);
        }
        else if (strncmp(buffer, "f", 1) == 0)
        {
            result = _glusWavefrontHandleFace(ctx, buffer);
        }
    }

    glusFileClose(ctx->f);

    if (!result)
    {
        // A record handler or the line read failed: nothing is flushed, matching
        // the early exits the per-record code had before the split.
        glusWavefrontFreeTempMemory(&ctx->tempMemory);

        return GLUS_FALSE;
    }

    if (wavefront && ctx->currentGroupList)
    {
        ctx->currentGroupList->group.numberIndices = ctx->numberIndicesGroup;
        ctx->numberIndicesGroup                    = 0; // NOLINT(clang-analyzer-deadcode.DeadStores) - end of parse, counter is never read again
    }

    result = glusWavefrontCopyData(shape, ctx->totalNumberVertices - ctx->offsetNumberVertices, &ctx->tempMemory.triangleVertices[(size_t)4 * ctx->offsetNumberVertices], ctx->totalNumberNormals - ctx->offsetNumberNormals, &ctx->tempMemory.triangleNormals[(size_t)3 * ctx->offsetNumberNormals], ctx->totalNumberTexCoords - ctx->offsetNumberTexCoords, &ctx->tempMemory.triangleTexCoords[(size_t)2 * ctx->offsetNumberTexCoords]);

    glusWavefrontFreeTempMemory(&ctx->tempMemory);

    if (result)
    {
        glusShapeCalculateTangentBitangentf(shape);
    }

    if (scene)
    {
        if (!_glusWavefrontMove(wavefront, shape))
        {
            if (result)
            {
                glusShapeDestroyf(shape);
            }

            return GLUS_FALSE;
        }

        if (!scene->objectList)
        {
            scene->objectList = (GLUSobjectList*)glusMemoryMalloc(sizeof(GLUSobjectList));
            if (!scene->objectList)
            {
                glusWavefrontDestroy(wavefront);

                return GLUS_FALSE;
            }
            scene->objectList->next = 0;

            ctx->currentObjectList = scene->objectList;
        }

        if (!ctx->currentObjectList || !wavefront)
        {
            // Unreachable: the block above guarantees a list node and the parser
            // only runs with a live wavefront. Stated explicitly so the copy below
            // is provably safe rather than relying on that reasoning.
            glusWavefrontDestroy(wavefront);

            return GLUS_FALSE;
        }

        memcpy(&ctx->currentObjectList->object, wavefront, sizeof(GLUSwavefront));
    }

    return result;
}

GLUSboolean _glusWavefrontParseLine(const GLUSchar* filename, GLUSline* line)
{
    GLUSboolean result;

    FILE* f;

    GLUSchar buffer[GLUS_BUFFERSIZE];

    // Initialized: the parse below is best effort and does not have to fill every
    // field, and a truncated `v`/`vt` line then wrote stack garbage into the
    // attribute arrays.
    GLUSfloat x = 0.0f, y = 0.0f, z = 0.0f;

    GLUSint start, end;

    GLUSfloat* vertices = 0;

    GLUSindex* indices = 0;

    GLUSuint numberVertices = 0;

    GLUSuint numberIndices = 0;

    // Objects

    GLUSuint numberObjects = 0;

    if (line)
    {
        memset(line, 0, sizeof(GLUSline));
    }

    if (!filename || !line)
    {
        return GLUS_FALSE;
    }

    f = glusFileOpen(filename, "r");

    if (!f)
    {
        return GLUS_FALSE;
    }

    if (!glusWavefrontMallocTempMemoryLine(&vertices, &indices))
    {
        glusWavefrontFreeTempMemoryLine(&vertices, &indices);

        glusFileClose(f);

        return GLUS_FALSE;
    }

    while (!feof(f))
    {
        buffer[0] = 0;

        if (fgets(buffer, GLUS_BUFFERSIZE, f) == 0)
        {
            if (ferror(f))
            {
                glusWavefrontFreeTempMemoryLine(&vertices, &indices);

                glusFileClose(f);

                return GLUS_FALSE;
            }
        }

        if (strncmp(buffer, "o", 1) == 0)
        {
            if (numberObjects == GLUS_MAX_OBJECTS)
            {
                glusWavefrontFreeTempMemoryLine(&vertices, &indices);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            numberObjects++;
        }
        else if (strncmp(buffer, "v", 1) == 0)
        {
            if (numberVertices == GLUS_MAX_ATTRIBUTES)
            {
                glusWavefrontFreeTempMemoryLine(&vertices, &indices);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            {
                const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

                cursor = _glusWavefrontReadFloat(cursor, &x);
                cursor = _glusWavefrontReadFloat(cursor, &y);

                _glusWavefrontReadFloat(cursor, &z);
            }

            vertices[4 * numberVertices + 0] = x;
            vertices[4 * numberVertices + 1] = y;
            vertices[4 * numberVertices + 2] = z;
            vertices[4 * numberVertices + 3] = 1.0f;

            numberVertices++;
        }
        else if (strncmp(buffer, "l", 1) == 0)
        {
            if (numberIndices == GLUS_MAX_LINE_ATTRIBUTES)
            {
                glusWavefrontFreeTempMemoryLine(&vertices, &indices);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            start = 0;
            end   = 0;

            {
                const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

                cursor = _glusWavefrontReadInt(cursor, &start);

                _glusWavefrontReadInt(cursor, &end);
            }

            // Resolve one based and relative indices and reject everything out of bounds.

            start = glusWavefrontResolveIndex(start, numberVertices);
            end   = glusWavefrontResolveIndex(end, numberVertices);

            if (start < 0 || end < 0)
            {
                glusWavefrontFreeTempMemoryLine(&vertices, &indices);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            indices[numberIndices + 0] = (GLUSindex)start;
            indices[numberIndices + 1] = (GLUSindex)end;

            numberIndices += 2;
        }
    }

    glusFileClose(f);

    result = glusWavefrontCopyDataLine(line, numberVertices, vertices, numberIndices, indices);

    glusWavefrontFreeTempMemoryLine(&vertices, &indices);

    return result;
}

//

GLUSboolean GLUSAPIENTRY glusWavefrontLoad(const GLUSchar* filename, GLUSwavefront* wavefront)
{
    GLUSshape dummyShape;

    if (!_glusWavefrontParse(filename, &dummyShape, wavefront, 0))
    {
        glusWavefrontDestroy(wavefront);

        return GLUS_FALSE;
    }

    if (!_glusWavefrontMove(wavefront, &dummyShape))
    {
        glusWavefrontDestroy(wavefront);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusWavefrontDestroy(GLUSwavefront* wavefront)
{
    if (!wavefront)
    {
        return;
    }

    glusWavefrontDestroyMaterial(&wavefront->materials);
    glusWavefrontDestroyGroup(&wavefront->groups);

    if (wavefront->vertices)
    {
        glusMemoryFree(wavefront->vertices);

        wavefront->vertices = 0;
    }

    if (wavefront->normals)
    {
        glusMemoryFree(wavefront->normals);

        wavefront->normals = 0;
    }

    if (wavefront->texCoords)
    {
        glusMemoryFree(wavefront->texCoords);

        wavefront->texCoords = 0;
    }

    if (wavefront->tangents)
    {
        glusMemoryFree(wavefront->tangents);

        wavefront->tangents = 0;
    }

    if (wavefront->bitangents)
    {
        glusMemoryFree(wavefront->bitangents);

        wavefront->bitangents = 0;
    }

    memset(wavefront, 0, sizeof(GLUSwavefront));
}

GLUSboolean GLUSAPIENTRY glusWavefrontLoadScene(const GLUSchar* filename, GLUSscene* scene)
{
    GLUSshape     dummyShape;
    GLUSwavefront dummyWavefront;

    if (!scene)
    {
        return GLUS_FALSE;
    }

    memset(&dummyShape, 0, sizeof(GLUSshape));
    memset(&dummyWavefront, 0, sizeof(GLUSwavefront));

    memset(scene, 0, sizeof(GLUSscene));

    if (!_glusWavefrontParse(filename, &dummyShape, &dummyWavefront, scene))
    {
        glusWavefrontDestroyScene(scene);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusWavefrontDestroyScene(GLUSscene* scene)
{
    GLUSobjectList* walker;
    GLUSobjectList* toDelete;

    if (!scene)
    {
        return;
    }

    walker = scene->objectList;

    while (walker)
    {
        // Avoid deleting materials several times.
        if (walker != scene->objectList)
        {
            walker->object.materials = 0;
        }

        glusWavefrontDestroy(&walker->object);

        toDelete = walker;

        walker = walker->next;

        glusMemoryFree(toDelete);
    }

    memset(scene, 0, sizeof(GLUSscene));
}
