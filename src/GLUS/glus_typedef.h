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

#ifndef GLUS_TYPEDEF_H_
#define GLUS_TYPEDEF_H_

typedef unsigned char  GLUSboolean;
typedef signed char    GLUSbyte;
typedef unsigned char  GLUSubyte;
typedef char           GLUSchar;
typedef short          GLUSshort;
typedef unsigned short GLUSushort;
typedef int            GLUSint;
typedef unsigned int   GLUSuint;
typedef int            GLUSfixed;
typedef int64_t        GLUSint64;
typedef uint64_t       GLUSuint64;
typedef int            GLUSsizei;
typedef unsigned int   GLUSenum;
// GLUSintptr, GLUSsizeiptr and GLUSsync not implemented.
typedef unsigned int   GLUSbitfield;
typedef unsigned short GLUShalf;
typedef float          GLUSfloat;
typedef float          GLUSclampf;
typedef double         GLUSdouble;
typedef double         GLUSclampd;

typedef struct _GLUScomplex
{
    GLUSfloat real;
    GLUSfloat imaginary;
} GLUScomplex;

#ifdef __cplusplus
#define GLUSvoid void
#else
typedef void GLUSvoid;
#endif

#endif /* GLUS_TYPEDEF_H_ */
