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

#ifndef GLUS_DEFINE_H_
#define GLUS_DEFINE_H_

#define GLUS_NO_ERROR 0
#define GLUS_OK 0
#define GLUS_TRUE 1
#define GLUS_FALSE 0

#define GLUS_POINTS 0x0000

#define GLUS_LINES 0x0001
#define GLUS_LINE_LOOP 0x0002
#define GLUS_LINE_STRIP 0x0003
#define GLUS_LINES_ADJACENCY 0x000A
#define GLUS_LINE_STRIP_ADJACENCY 0x000B

#define GLUS_TRIANGLES 0x0004
#define GLUS_TRIANGLE_STRIP 0x0005
#define GLUS_TRIANGLE_FAN 0x0006
#define GLUS_TRIANGLES_ADJACENCY 0x000C
#define GLUS_TRIANGLE_STRIP_ADJACENCY 0x000D

#define GLUS_UNPACK_ALIGNMENT 0x0CF5
#define GLUS_PACK_ALIGNMENT 0x0D05

#define GLUS_BYTE 0x1400
#define GLUS_UNSIGNED_BYTE 0x1401
#define GLUS_SHORT 0x1402
#define GLUS_UNSIGNED_SHORT 0x1403
#define GLUS_INT 0x1404
#define GLUS_UNSIGNED_INT 0x1405
#define GLUS_FLOAT 0x1406
#define GLUS_DOUBLE 0x140A

#define GLUS_VERSION 0x1F02
#define GLUS_EXTENSIONS 0x1F03

#define GLUS_COMPILE_STATUS 0x8B81
#define GLUS_LINK_STATUS 0x8B82
#define GLUS_VALIDATE_STATUS 0x8B83
#define GLUS_INFO_LOG_LENGTH 0x8B84

#define GLUS_FRAMEBUFFER 0x8D40

#define GLUS_COMPRESSED_R11_EAC 0x9270
#define GLUS_COMPRESSED_SIGNED_R11_EAC 0x9271
#define GLUS_COMPRESSED_RG11_EAC 0x9272
#define GLUS_COMPRESSED_SIGNED_RG11_EAC 0x9273
#define GLUS_COMPRESSED_RGB8_ETC2 0x9274
#define GLUS_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GLUS_COMPRESSED_RGBA8_ETC2_EAC 0x9278

#define GLUS_PI 3.1415926535897932384626433832795f

#define GLUS_LOG_NOTHING 0
#define GLUS_LOG_ERROR 1
#define GLUS_LOG_WARNING 2
#define GLUS_LOG_INFO 3
#define GLUS_LOG_DEBUG 4
#define GLUS_LOG_SEVERE 5

#define GLUS_VERTICES_FACTOR 4
#define GLUS_VERTICES_DIVISOR 4

#define GLUS_MAX_STRING 256

#define GLUS_MAX_FILENAME 2048

#ifndef GLUS_BASE_DIRECTORY
#define GLUS_BASE_DIRECTORY ""
#endif

#endif /* GLUS_DEFINE_H_ */
