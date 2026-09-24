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

static GLUSboolean glusWavefrontMallocTempMemory(GLUSfloat** vertices, GLUSfloat** normals, GLUSfloat** texCoords, GLUSfloat** triangleVertices, GLUSfloat** triangleNormals, GLUSfloat** triangleTexCoords)
{
    if (!vertices || !normals || !texCoords || !triangleVertices || !triangleNormals || !triangleTexCoords)
    {
        return GLUS_FALSE;
    }

    *vertices = (GLUSfloat*)glusMemoryMalloc((size_t)4 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*vertices)
    {
        return GLUS_FALSE;
    }

    *normals = (GLUSfloat*)glusMemoryMalloc((size_t)3 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*normals)
    {
        return GLUS_FALSE;
    }

    *texCoords = (GLUSfloat*)glusMemoryMalloc((size_t)2 * GLUS_MAX_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*texCoords)
    {
        return GLUS_FALSE;
    }

    *triangleVertices = (GLUSfloat*)glusMemoryMalloc((size_t)4 * GLUS_MAX_TRIANGLE_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*triangleVertices)
    {
        return GLUS_FALSE;
    }

    *triangleNormals = (GLUSfloat*)glusMemoryMalloc((size_t)3 * GLUS_MAX_TRIANGLE_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*triangleNormals)
    {
        return GLUS_FALSE;
    }

    *triangleTexCoords = (GLUSfloat*)glusMemoryMalloc((size_t)2 * GLUS_MAX_TRIANGLE_ATTRIBUTES * sizeof(GLUSfloat));
    if (!*triangleTexCoords)
    {
        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

static GLUSvoid glusWavefrontFreeTempMemory(GLUSfloat** vertices, GLUSfloat** normals, GLUSfloat** texCoords, GLUSfloat** triangleVertices, GLUSfloat** triangleNormals, GLUSfloat** triangleTexCoords)
{
    if (vertices && *vertices)
    {
        glusMemoryFree(*vertices);

        *vertices = 0;
    }

    if (normals && *normals)
    {
        glusMemoryFree(*normals);

        *normals = 0;
    }

    if (texCoords && *texCoords)
    {
        glusMemoryFree(*texCoords);

        *texCoords = 0;
    }

    if (triangleVertices && *triangleVertices)
    {
        glusMemoryFree(*triangleVertices);

        *triangleVertices = 0;
    }

    if (triangleNormals && *triangleNormals)
    {
        glusMemoryFree(*triangleNormals);

        *triangleNormals = 0;
    }

    if (triangleTexCoords && *triangleTexCoords)
    {
        glusMemoryFree(*triangleTexCoords);

        *triangleTexCoords = 0;
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
            {
                const GLUSchar* cursor = _glusWavefrontSkipField(checkBuffer);

                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.emissive[0]);
                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.emissive[1]);

                _glusWavefrontReadFloat(cursor, &currentMaterialList->material.emissive[2]);
            }

            currentMaterialList->material.emissive[3] = 1.0f;
        }
        else if (strncmp(checkBuffer, "ka", 2) == 0)
        {
            {
                const GLUSchar* cursor = _glusWavefrontSkipField(checkBuffer);

                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.ambient[0]);
                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.ambient[1]);

                _glusWavefrontReadFloat(cursor, &currentMaterialList->material.ambient[2]);
            }

            currentMaterialList->material.ambient[3] = 1.0f;
        }
        else if (strncmp(checkBuffer, "kd", 2) == 0)
        {
            {
                const GLUSchar* cursor = _glusWavefrontSkipField(checkBuffer);

                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.diffuse[0]);
                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.diffuse[1]);

                _glusWavefrontReadFloat(cursor, &currentMaterialList->material.diffuse[2]);
            }

            currentMaterialList->material.diffuse[3] = 1.0f;
        }
        else if (strncmp(checkBuffer, "ks", 2) == 0)
        {
            {
                const GLUSchar* cursor = _glusWavefrontSkipField(checkBuffer);

                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.specular[0]);
                cursor = _glusWavefrontReadFloat(cursor, &currentMaterialList->material.specular[1]);

                _glusWavefrontReadFloat(cursor, &currentMaterialList->material.specular[2]);
            }

            currentMaterialList->material.specular[3] = 1.0f;
        }
        else if (strncmp(checkBuffer, "ns", 2) == 0)
        {
            _glusWavefrontReadFloat(_glusWavefrontSkipField(checkBuffer), &currentMaterialList->material.shininess);
        }
        else if (strncmp(checkBuffer, "d", 1) == 0 || strncmp(checkBuffer, "tr", 2) == 0)
        {
            _glusWavefrontReadFloat(_glusWavefrontSkipField(checkBuffer), &currentMaterialList->material.transparency);
        }
        else if (strncmp(checkBuffer, "ni", 2) == 0)
        {
            _glusWavefrontReadFloat(_glusWavefrontSkipField(checkBuffer), &currentMaterialList->material.indexOfRefraction);
        }
        else if (strncmp(checkBuffer, "map_ke", 6) == 0)
        {
            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            glusWavefrontCopyString(currentMaterialList->material.emissiveTextureFilename, sizeof(currentMaterialList->material.emissiveTextureFilename), name);
        }
        else if (strncmp(checkBuffer, "map_ka", 6) == 0)
        {
            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            glusWavefrontCopyString(currentMaterialList->material.ambientTextureFilename, sizeof(currentMaterialList->material.ambientTextureFilename), name);
        }
        else if (strncmp(checkBuffer, "map_kd", 6) == 0)
        {
            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            glusWavefrontCopyString(currentMaterialList->material.diffuseTextureFilename, sizeof(currentMaterialList->material.diffuseTextureFilename), name);
        }
        else if (strncmp(checkBuffer, "map_ks", 6) == 0)
        {
            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            glusWavefrontCopyString(currentMaterialList->material.specularTextureFilename, sizeof(currentMaterialList->material.specularTextureFilename), name);
        }
        else if (strncmp(checkBuffer, "map_d", 5) == 0 || strncmp(checkBuffer, "map_tr", 6) == 0)
        {
            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            glusWavefrontCopyString(currentMaterialList->material.transparencyTextureFilename, sizeof(currentMaterialList->material.transparencyTextureFilename), name);
        }
        else if (strncmp(checkBuffer, "map_bump", 8) == 0 || strncmp(checkBuffer, "bump", 4) == 0)
        {
            name[0] = '\0';

            sscanf(checkBuffer, "%31s %255s", identifier, name);

            glusWavefrontCopyString(currentMaterialList->material.bumpTextureFilename, sizeof(currentMaterialList->material.bumpTextureFilename), name);
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

GLUSboolean _glusWavefrontParse(const GLUSchar* filename, GLUSshape* shape, GLUSwavefront* wavefront, GLUSscene* scene)
{
    GLUSboolean result;

    FILE* f;

    GLUSchar buffer[GLUS_BUFFERSIZE];
    GLUSchar identifier[32];

    // Initialized: the sscanf below is not required to fill every conversion, and
    // a truncated `v`/`vt` line then wrote stack garbage into the attribute arrays.
    GLUSfloat x = 0.0f, y = 0.0f, z = 0.0f;
    GLUSfloat s = 0.0f, t = 0.0f;

    GLUSfloat* vertices  = 0;
    GLUSfloat* normals   = 0;
    GLUSfloat* texCoords = 0;

    GLUSuint numberVertices  = 0;
    GLUSuint numberNormals   = 0;
    GLUSuint numberTexCoords = 0;

    GLUSfloat* triangleVertices  = 0;
    GLUSfloat* triangleNormals   = 0;
    GLUSfloat* triangleTexCoords = 0;

    GLUSuint offsetNumberVertices  = 0;
    GLUSuint offsetNumberNormals   = 0;
    GLUSuint offsetNumberTexCoords = 0;

    GLUSuint totalNumberVertices  = 0;
    GLUSuint totalNumberNormals   = 0;
    GLUSuint totalNumberTexCoords = 0;

    GLUSuint facesEncoding = 0;

    // Material and groups

    GLUSchar name[GLUS_MAX_STRING] = {'\0'};

    GLUSuint numberIndicesGroup = 0;
    GLUSuint numberMaterials    = 0;
    GLUSuint numberGroups       = 0;

    GLUSgroupList*  currentGroupList  = 0;
    GLUSobjectList* currentObjectList = 0;

    // Objects

    GLUSuint numberObjects = 0;

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

    f = glusFileOpen(filename, "r");

    if (!f)
    {
        return GLUS_FALSE;
    }

    if (!glusWavefrontMallocTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords))
    {
        glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

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
                glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                glusFileClose(f);

                return GLUS_FALSE;
            }
        }

        if (wavefront)
        {
            if (strncmp(buffer, "mtllib", 6) == 0)
            {
                name[0] = '\0';

                if (sscanf(buffer, "%31s %255s", identifier, name) != 2)
                {
                    // Without a file name there is nothing to load.

                    continue;
                }

                if (numberMaterials == 0)
                {
                    wavefront->materials = 0;
                }

                if (!glusWavefrontLoadMaterial(name, &wavefront->materials))
                {
                    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                    glusFileClose(f);

                    return GLUS_FALSE;
                }

                numberMaterials++;
            }
            else if (strncmp(buffer, "usemtl", 6) == 0)
            {
                // Parse the material name first, as it is needed for the group as well.

                name[0] = '\0';

                sscanf(buffer, "%31s %255s", identifier, name);

                if (!currentGroupList || currentGroupList->group.materialName[0] != '\0')
                {
                    GLUSgroupList* newGroupList;

                    newGroupList = (GLUSgroupList*)glusMemoryMalloc(sizeof(GLUSgroupList));

                    if (!newGroupList)
                    {
                        glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                        glusFileClose(f);

                        return GLUS_FALSE;
                    }

                    memset(newGroupList, 0, sizeof(GLUSgroupList));

                    glusWavefrontCopyString(newGroupList->group.name, sizeof(newGroupList->group.name), name);

                    if (numberGroups == 0)
                    {
                        if (!wavefront)
                        {
                            glusMemoryFree(newGroupList);

                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        wavefront->groups = newGroupList;
                    }
                    else
                    {
                        if (!currentGroupList)
                        {
                            glusMemoryFree(newGroupList);

                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        currentGroupList->next = newGroupList;

                        currentGroupList->group.numberIndices = numberIndicesGroup;
                        numberIndicesGroup                    = 0;
                    }

                    currentGroupList = newGroupList;

                    numberGroups++;
                }

                //

                if (!currentGroupList)
                {
                    // Unreachable in practice: the branch above only runs when a
                    // group was just created and currentGroupList was set to it.
                    // Stated explicitly anyway so the dereference below is
                    // provably safe rather than relying on that reasoning.
                    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                    glusFileClose(f);

                    return GLUS_FALSE;
                }

                glusWavefrontCopyString(currentGroupList->group.materialName, sizeof(currentGroupList->group.materialName), name);
            }
            else if (strncmp(buffer, "g", 1) == 0)
            {
                GLUSgroupList* newGroupList;

                name[0] = '\0';

                sscanf(buffer, "%31s %255s", identifier, name);

                newGroupList = (GLUSgroupList*)glusMemoryMalloc(sizeof(GLUSgroupList));

                if (!newGroupList)
                {
                    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                    glusFileClose(f);

                    return GLUS_FALSE;
                }

                memset(newGroupList, 0, sizeof(GLUSgroupList));

                glusWavefrontCopyString(newGroupList->group.name, sizeof(newGroupList->group.name), name);

                if (numberGroups == 0)
                {
                    wavefront->groups = newGroupList;
                }
                else
                {
                    if (!currentGroupList)
                    {
                        glusMemoryFree(newGroupList);

                        glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                        glusFileClose(f);

                        return GLUS_FALSE;
                    }

                    currentGroupList->next = newGroupList;

                    currentGroupList->group.numberIndices = numberIndicesGroup;
                    numberIndicesGroup                    = 0;
                }

                currentGroupList = newGroupList;

                numberGroups++;
            }
        }

        if (strncmp(buffer, "o", 1) == 0)
        {
            if (scene)
            {
                GLUSobjectList* newObjectList;

                if (currentObjectList)
                {
                    if (wavefront && currentGroupList)
                    {
                        currentGroupList->group.numberIndices = numberIndicesGroup;
                        numberIndicesGroup                    = 0; // NOLINT(clang-analyzer-deadcode.DeadStores) - defensive reset; the loop reassigns before the next read
                    }

                    result = glusWavefrontCopyData(shape, totalNumberVertices - offsetNumberVertices, &triangleVertices[(size_t)4 * offsetNumberVertices], totalNumberNormals - offsetNumberNormals, &triangleNormals[(size_t)3 * offsetNumberNormals], totalNumberTexCoords - offsetNumberTexCoords, &triangleTexCoords[(size_t)2 * offsetNumberTexCoords]);

                    if (result)
                    {
                        glusShapeCalculateTangentBitangentf(shape);
                    }

                    if (!_glusWavefrontMove(wavefront, shape))
                    {
                        glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                        glusFileClose(f);

                        return GLUS_FALSE;
                    }

                    memcpy(&currentObjectList->object, wavefront, sizeof(GLUSwavefront));

                    // The snapshot above is a shallow copy, so it owns the group
                    // chain from here on and the live wavefront has to let go of it.
                    // If the next object has faces but no `g`/`usemtl`, the next
                    // _glusWavefrontMove would walk these nodes again: it
                    // reallocates group.indices that the snapshot still references
                    // and hands the same node to two objects, which
                    // glusWavefrontDestroyScene then frees twice.
                    wavefront->groups = 0;
                }

                name[0] = '\0';

                sscanf(buffer, "%31s %255s", identifier, name);

                glusWavefrontCopyString(wavefront->name, sizeof(wavefront->name), name);

                // Always create a new object.

                newObjectList = (GLUSobjectList*)glusMemoryMalloc(sizeof(GLUSobjectList));
                if (!newObjectList)
                {
                    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                    glusFileClose(f);

                    return GLUS_FALSE;
                }
                newObjectList->next = 0;

                // Link together.
                if (currentObjectList)
                {
                    currentObjectList->next = newObjectList;
                }
                currentObjectList = newObjectList;

                // Set as root, if needed.
                if (scene->objectList == 0)
                {
                    scene->objectList = currentObjectList;
                }

                // Remember offset and reset values.

                offsetNumberVertices  = totalNumberVertices;
                offsetNumberNormals   = totalNumberNormals;
                offsetNumberTexCoords = totalNumberTexCoords;

                numberGroups = 0;

                currentGroupList = 0;

                // Reset unconditionally. The flush above only runs while a group is
                // open, so faces emitted outside any `g`/`usemtl` left their count
                // behind and it was credited to the next object's first group - whose
                // index range then ran past this object's vertex count at draw time.
                numberIndicesGroup = 0;
            }
            else if (wavefront)
            {
                GLUSgroupList* newGroupList;

                name[0] = '\0';

                sscanf(buffer, "%31s %255s", identifier, name);

                newGroupList = (GLUSgroupList*)glusMemoryMalloc(sizeof(GLUSgroupList));

                if (!newGroupList)
                {
                    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                    glusFileClose(f);

                    return GLUS_FALSE;
                }

                memset(newGroupList, 0, sizeof(GLUSgroupList));

                glusWavefrontCopyString(newGroupList->group.name, sizeof(newGroupList->group.name), name);

                if (numberGroups == 0)
                {
                    wavefront->groups = newGroupList;
                }
                else
                {
                    if (!currentGroupList)
                    {
                        glusMemoryFree(newGroupList);

                        glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                        glusFileClose(f);

                        return GLUS_FALSE;
                    }

                    currentGroupList->next = newGroupList;

                    currentGroupList->group.numberIndices = numberIndicesGroup;
                    numberIndicesGroup                    = 0;
                }

                currentGroupList = newGroupList;

                numberGroups++;
            }
            else
            {
                if (numberObjects == GLUS_MAX_OBJECTS)
                {
                    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                    glusFileClose(f);

                    return GLUS_FALSE;
                }
            }

            numberObjects++;
        }
        else if (strncmp(buffer, "vt", 2) == 0)
        {
            if (numberTexCoords == GLUS_MAX_ATTRIBUTES)
            {
                glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            {
                const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

                cursor = _glusWavefrontReadFloat(cursor, &s);

                _glusWavefrontReadFloat(cursor, &t);
            }

            texCoords[2 * numberTexCoords + 0] = s;
            texCoords[2 * numberTexCoords + 1] = t;

            numberTexCoords++;
        }
        else if (strncmp(buffer, "vn", 2) == 0)
        {
            if (numberNormals == GLUS_MAX_ATTRIBUTES)
            {
                glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                glusFileClose(f);

                return GLUS_FALSE;
            }

            {
                const GLUSchar* cursor = _glusWavefrontSkipField(buffer);

                cursor = _glusWavefrontReadFloat(cursor, &x);
                cursor = _glusWavefrontReadFloat(cursor, &y);

                _glusWavefrontReadFloat(cursor, &z);
            }

            normals[3 * numberNormals + 0] = x;
            normals[3 * numberNormals + 1] = y;
            normals[3 * numberNormals + 2] = z;

            numberNormals++;
        }
        else if (strncmp(buffer, "v", 1) == 0)
        {
            if (numberVertices == GLUS_MAX_ATTRIBUTES)
            {
                glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

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
        else if (strncmp(buffer, "f", 1) == 0)
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
                continue;
            }

            // Check faces
            if (strstr(token, "//") != 0)
            {
                facesEncoding = 2;
            }
            else if (strstr(token, "/") == 0)
            {
                facesEncoding = 0;
            }
            else if (strstr(token, "/") != 0)
            {
                GLUSchar* c = strstr(token, "/");

                c++;

                if (!c)
                {
                    continue;
                }

                if (strstr(c, "/") == 0)
                {
                    facesEncoding = 1;
                }
                else
                {
                    facesEncoding = 3;
                }
            }

            while (token != 0)
            {
                vIndex  = 0;
                vtIndex = 0;
                vnIndex = 0;

                switch (facesEncoding)
                {
                case 0:
                    _glusWavefrontReadInt(token, &vIndex);
                    break;
                case 1:
                    {
                        const GLUSchar* cursor = _glusWavefrontReadInt(token, &vIndex);

                        _glusWavefrontReadInt(cursor, &vtIndex);
                    }
                    break;
                case 2:
                    {
                        const GLUSchar* cursor = _glusWavefrontReadInt(token, &vIndex);

                        _glusWavefrontReadInt(cursor, &vnIndex);
                    }
                    break;
                case 3:
                    {
                        const GLUSchar* cursor = _glusWavefrontReadInt(token, &vIndex);

                        cursor = _glusWavefrontReadInt(cursor, &vtIndex);
                        _glusWavefrontReadInt(cursor, &vnIndex);
                    }
                    break;
                default:
                    break; // Unreachable: facesEncoding is 0..3.
                }

                // Resolve one based and relative indices and reject everything out of bounds.

                vIndex  = glusWavefrontResolveIndex(vIndex, numberVertices);
                vtIndex = glusWavefrontResolveIndex(vtIndex, numberTexCoords);
                vnIndex = glusWavefrontResolveIndex(vnIndex, numberNormals);

                if (vIndex >= 0)
                {
                    if (emittedVertices < 3)
                    {
                        if (totalNumberVertices >= GLUS_MAX_TRIANGLE_ATTRIBUTES)
                        {
                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        memcpy(&triangleVertices[(size_t)4 * totalNumberVertices], &vertices[(ptrdiff_t)4 * vIndex], 4 * sizeof(GLUSfloat));

                        totalNumberVertices++;
                        numberIndicesGroup++;
                        emittedVertices++;
                    }
                    else
                    {
                        if (totalNumberVertices >= GLUS_MAX_TRIANGLE_ATTRIBUTES - 2)
                        {
                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        memcpy(&triangleVertices[(size_t)4 * (totalNumberVertices)], &triangleVertices[(size_t)4 * (totalNumberVertices - emittedVertices)], 4 * sizeof(GLUSfloat));
                        memcpy(&triangleVertices[(size_t)4 * (totalNumberVertices + 1)], &triangleVertices[(size_t)4 * (totalNumberVertices - 1)], 4 * sizeof(GLUSfloat));
                        memcpy(&triangleVertices[(size_t)4 * (totalNumberVertices + 2)], &vertices[(ptrdiff_t)4 * vIndex], 4 * sizeof(GLUSfloat));

                        totalNumberVertices += 3;
                        numberIndicesGroup += 3;
                        emittedVertices += 3;
                    }
                }
                if (vnIndex >= 0)
                {
                    if (emittedNormals < 3)
                    {
                        if (totalNumberNormals >= GLUS_MAX_TRIANGLE_ATTRIBUTES)
                        {
                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        memcpy(&triangleNormals[(size_t)3 * totalNumberNormals], &normals[(ptrdiff_t)3 * vnIndex], 3 * sizeof(GLUSfloat));

                        totalNumberNormals++;
                        emittedNormals++;
                    }
                    else
                    {
                        if (totalNumberNormals >= GLUS_MAX_TRIANGLE_ATTRIBUTES - 2)
                        {
                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        memcpy(&triangleNormals[(size_t)3 * (totalNumberNormals)], &triangleNormals[(size_t)3 * (totalNumberNormals - emittedNormals)], 3 * sizeof(GLUSfloat));
                        memcpy(&triangleNormals[(size_t)3 * (totalNumberNormals + 1)], &triangleNormals[(size_t)3 * (totalNumberNormals - 1)], 3 * sizeof(GLUSfloat));
                        memcpy(&triangleNormals[(size_t)3 * (totalNumberNormals + 2)], &normals[(ptrdiff_t)3 * vnIndex], 3 * sizeof(GLUSfloat));

                        totalNumberNormals += 3;
                        emittedNormals += 3;
                    }
                }
                if (vtIndex >= 0)
                {
                    if (emittedTexCoords < 3)
                    {
                        if (totalNumberTexCoords >= GLUS_MAX_TRIANGLE_ATTRIBUTES)
                        {
                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        memcpy(&triangleTexCoords[(size_t)2 * totalNumberTexCoords], &texCoords[(ptrdiff_t)2 * vtIndex], 2 * sizeof(GLUSfloat));

                        totalNumberTexCoords++;
                        emittedTexCoords++;
                    }
                    else
                    {
                        if (totalNumberTexCoords >= GLUS_MAX_TRIANGLE_ATTRIBUTES - 2)
                        {
                            glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

                            glusFileClose(f);

                            return GLUS_FALSE;
                        }

                        memcpy(&triangleTexCoords[(size_t)2 * (totalNumberTexCoords)], &triangleTexCoords[(size_t)2 * (totalNumberTexCoords - emittedTexCoords)], 2 * sizeof(GLUSfloat));
                        memcpy(&triangleTexCoords[(size_t)2 * (totalNumberTexCoords + 1)], &triangleTexCoords[(size_t)2 * (totalNumberTexCoords - 1)], 2 * sizeof(GLUSfloat));
                        memcpy(&triangleTexCoords[(size_t)2 * (totalNumberTexCoords + 2)], &texCoords[(ptrdiff_t)2 * vtIndex], 2 * sizeof(GLUSfloat));

                        totalNumberTexCoords += 3;
                        emittedTexCoords += 3;
                    }
                }

                token = strtok(0, " \n");
            }
        }
    }

    glusFileClose(f);

    if (wavefront && currentGroupList)
    {
        currentGroupList->group.numberIndices = numberIndicesGroup;
        numberIndicesGroup                    = 0; // NOLINT(clang-analyzer-deadcode.DeadStores) - end of parse, counter is never read again
    }

    result = glusWavefrontCopyData(shape, totalNumberVertices - offsetNumberVertices, &triangleVertices[(size_t)4 * offsetNumberVertices], totalNumberNormals - offsetNumberNormals, &triangleNormals[(size_t)3 * offsetNumberNormals], totalNumberTexCoords - offsetNumberTexCoords, &triangleTexCoords[(size_t)2 * offsetNumberTexCoords]);

    glusWavefrontFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

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

            currentObjectList = scene->objectList;
        }

        if (!currentObjectList || !wavefront)
        {
            // Unreachable: the block above guarantees a list node and the parser
            // only runs with a live wavefront. Stated explicitly so the copy below
            // is provably safe rather than relying on that reasoning.
            glusWavefrontDestroy(wavefront);

            return GLUS_FALSE;
        }

        memcpy(&currentObjectList->object, wavefront, sizeof(GLUSwavefront));
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
