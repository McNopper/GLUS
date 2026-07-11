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

#ifndef GLUS_FILE_TEXT_H_
#define GLUS_FILE_TEXT_H_

/**
 * Loads a text file.
 *
 * @param filename The name of the file to load.
 * @param textfile The structure to fill the text data.
 *
 * @return GLUS_TRUE, if loading succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusFileLoadText(const GLUSchar* filename, GLUStextfile* textfile);

/**
 * Saves a text file.
 *
 * @param filename	The name of the file to save.
 * @param textfile 	The structure with the text data.
 *
 * @return GLUS_TRUE, if saving succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusFileSaveText(const GLUSchar* filename, const GLUStextfile* textfile);

/**
 * Destroys the content of a text structure. Has to be called for freeing the resources.
 *
 * @param textfile The text file structure.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusFileDestroyText(GLUStextfile* textfile);

#endif /* GLUS_FILE_TEXT_H_ */
