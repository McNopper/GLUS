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

#ifndef GLUS_PADDING_H_
#define GLUS_PADDING_H_

/**
 * Converts the padding of an array of vectors.
 *
 * @param target			Target array.
 * @param source			Source array.
 * @param channels			Number of channels. Values equal greater 1 are allowed.
 * @param padding			Number of padding elements. Values equal greater 0 are allowed.
 * @param numberElements	Number of elements in the array.
 *
 * @return GLUS_TRUE, if succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusPaddingConvertf(GLUSfloat* target, const GLUSfloat* source, const GLUSint channels, const GLUSint padding, const GLUSint numberElements);

#endif /* GLUS_PADDING_H_ */
