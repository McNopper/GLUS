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

GLUSboolean _glusFileCheckRead(FILE* f, size_t actualRead, size_t expectedRead)
{
    if (!f)
    {
        return GLUS_FALSE;
    }

    if (actualRead < expectedRead)
    {
        glusFileClose(f);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean _glusFileCheckWrite(FILE* f, size_t actualWrite, size_t expectedWrite)
{
    if (!f)
    {
        return GLUS_FALSE;
    }

    if (actualWrite < expectedWrite)
    {
        if (ferror(f))
        {
            glusFileClose(f);

            return GLUS_FALSE;
        }
    }

    return GLUS_TRUE;
}

FILE* GLUSAPIENTRY glusFileOpen(const char* filename, const char* mode)
{
    char buffer[GLUS_MAX_FILENAME];

    if (!filename)
    {
        // Note: Automatic errno setting.
        return fopen(filename, mode);
    }

    if (strlen(filename) + strlen(GLUS_BASE_DIRECTORY) >= GLUS_MAX_FILENAME)
    {
        // Note: Automatic errno setting.
        return fopen(filename, mode);
    }

    strcpy(buffer, GLUS_BASE_DIRECTORY);
    strcat(buffer, filename);

    return fopen(buffer, mode);
}

int GLUSAPIENTRY glusFileClose(FILE* stream)
{
    return fclose(stream);
}
