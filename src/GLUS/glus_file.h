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

#ifndef GLUS_FILE_H_
#define GLUS_FILE_H_

/**
 * Structure used for text file loading.
 */
typedef struct _GLUStextfile
{
    /**
     * Contains the data of the text file.
     */
    GLUSchar* text;

    /**
     * The length of the text file without the null terminator.
     */
    GLUSint length;

} GLUStextfile;

/**
 * Structure used for binary file loading.
 */
typedef struct _GLUSbinaryfile
{
    /**
     * The binary data of the file.
     */
    GLUSubyte* binary;

    /**
     * The length of the binary data.
     */
    GLUSint length;

} GLUSbinaryfile;

/**
 * Opens the file whose name is specified in the parameter filename and
 * associates it with a stream that can be identified in future operations by the FILE pointer returned.
 *
 * @param filename C string containing the name of the file to be opened.
 * @param mode C string containing a file access mode
 *
 * @return If the file is successfully opened, the function returns a pointer to a FILE object that can be used to identify the stream on future operations.
 *		   Otherwise, a null pointer is returned.
 */
GLUSAPI FILE* GLUSAPIENTRY glusFileOpen(const char* filename, const char* mode);

/**
 * Closes the file associated with the stream and disassociates it.
 *
 * @param stream Pointer to a FILE object that specifies the stream to be closed.
 *
 * @return If the stream is successfully closed, a zero value is returned.
 * On failure, EOF is returned.
 */
GLUSAPI int GLUSAPIENTRY glusFileClose(FILE* stream);

#endif /* GLUS_FILE_H_ */
