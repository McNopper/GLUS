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

GLUSboolean GLUSAPIENTRY glusExtensionIsSupported(const GLUSchar* extension)
{
    const GLUSubyte* allExtensions;
    const GLUSubyte* startExtension;
    GLUSubyte*       walkerExtension;
    GLUSubyte*       terminatorExtension;

    if (!extension)
    {
        return GLUS_FALSE;
    }

    walkerExtension = (GLUSubyte*)strchr(extension, ' ');
    if (walkerExtension || *extension == '\0')
    {
        return GLUS_FALSE;
    }

    allExtensions = vgGetString(VG_EXTENSIONS);

    startExtension = allExtensions;
    while (startExtension)
    {
        walkerExtension = (GLUSubyte*)strstr((const GLUSchar*)startExtension, extension);

        if (!walkerExtension)
        {
            return GLUS_FALSE;
        }

        terminatorExtension = walkerExtension + strlen(extension);

        if (!terminatorExtension)
        {
            return GLUS_FALSE;
        }

        if (walkerExtension == startExtension || *(walkerExtension - 1) == ' ')
        {
            if (*terminatorExtension == ' ' || *terminatorExtension == '\0')
            {
                return GLUS_TRUE;
            }
        }

        startExtension = (const GLUSubyte*)terminatorExtension;
    }

    return GLUS_FALSE;
}
