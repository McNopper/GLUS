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

#define GLUS_MAX_TEXTFILE_LENGTH 2147483646

extern GLUSboolean _glusFileCheckRead(FILE* f, size_t actualRead, size_t expectedRead);
extern GLUSboolean _glusFileCheckWrite(FILE* f, size_t actualWrite, size_t expectedWrite);

GLUSboolean GLUSAPIENTRY glusFileLoadText(const GLUSchar* filename, GLUStextfile* textfile)
{
    FILE*  f;
    size_t elementsRead;

    if (!filename || !textfile)
    {
        return GLUS_FALSE;
    }

    textfile->text = 0;

    textfile->length = 0;

    f = glusFileOpen(filename, "rb");

    if (!f)
    {
        return GLUS_FALSE;
    }

    if (fseek(f, 0, SEEK_END))
    {
        glusFileClose(f);

        return GLUS_FALSE;
    }

    textfile->length = ftell(f);

    if (textfile->length < 0 || textfile->length == GLUS_MAX_TEXTFILE_LENGTH)
    {
        glusFileClose(f);

        textfile->length = 0;

        return GLUS_FALSE;
    }

    textfile->text = (GLUSchar*)glusMemoryMalloc((size_t)textfile->length + 1);

    if (!textfile->text)
    {
        glusFileClose(f);

        textfile->length = 0;

        return GLUS_FALSE;
    }

    memset(textfile->text, 0, (size_t)textfile->length + 1);

    rewind(f);

    elementsRead = fread(textfile->text, 1, (size_t)textfile->length, f);

    if (!_glusFileCheckRead(f, elementsRead, (size_t)textfile->length))
    {
        glusFileDestroyText(textfile);

        return GLUS_FALSE;
    }

    glusFileClose(f);

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusFileSaveText(const GLUSchar* filename, const GLUStextfile* textfile)
{
    FILE*  file;
    size_t elementsWritten;

    if (!filename || !textfile)
    {
        return GLUS_FALSE;
    }

    file = glusFileOpen(filename, "w");

    if (!file)
    {
        return GLUS_FALSE;
    }

    elementsWritten = fwrite(textfile->text, 1, textfile->length * sizeof(GLUSchar), file);

    if (!_glusFileCheckWrite(file, elementsWritten, textfile->length * sizeof(GLUSchar)))
    {
        return GLUS_FALSE;
    }

    glusFileClose(file);

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusFileDestroyText(GLUStextfile* textfile)
{
    if (!textfile)
    {
        return;
    }

    if (textfile->text)
    {
        glusMemoryFree(textfile->text);

        textfile->text = 0;
    }

    textfile->length = 0;
}
