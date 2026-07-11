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

#if (GLUS_ES || GLUS_ES2 || GLUS_ES3 || GLUS_ES31)

#ifdef GLUS_ES31
#include "../GLES3/glus31.h"
#elif GLUS_ES2
#include "../GLES2/glus2.h"
#else
#include "../GLES3/glus3.h"
#endif

#elif (GLUS_VG || GLUS_VG11) /*GLUS_ES || GLUS_ES2 || GLUS_ES3 || GLUS_ES31*/

#include "../VG/glus.h"

#else /* GLUS_VG || GLUS_VG11 */

#ifndef __glus_h_
#define __glus_h_

#ifdef __cplusplus
extern "C"
{
#endif

    //
    // GLUS libraries.
    //

#include "../GLUS/glus_clib.h"

    //
    // GLEW for binding OpenGL functions. GLFW for hardware abstraction.
    //

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include <GLFW/glfw3.h>

    typedef int EGLint;

#define EGL_TRUE 1
#define EGL_FALSE 0
#define EGL_NONE 0x3038
#define EGL_DONT_CARE ((EGLint) - 1)

    //

#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_OPENGL_BIT 0x0008

#define EGL_RED_SIZE 0x3024
#define EGL_GREEN_SIZE 0x3023
#define EGL_BLUE_SIZE 0x3022
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026
#define EGL_ALPHA_SIZE 0x3021
#define EGL_SAMPLES 0x3031
#define EGL_SAMPLE_BUFFERS 0x3032

    //

#define EGL_BACK_BUFFER 0x3084
#define EGL_SINGLE_BUFFER 0x3085
#define EGL_RENDER_BUFFER 0x3086

#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#define EGL_CONTEXT_MAJOR_VERSION 0x3098
#define EGL_CONTEXT_MINOR_VERSION 0x30FB
#define EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE 0x31B1

#define EGL_CONTEXT_OPENGL_PROFILE_MASK 0x30FD
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT 0x00000002

#define EGL_CONTEXT_OPENGL_DEBUG 0x31B0

    //
    // GLUS function call convention etc.
    //

#include "../GLUS/glus_function.h"

    //
    // GLUS types.
    //

#include "../GLUS/glus_typedef.h"

    //
    // GLUS defines.
    //

#include "../GLUS/glus_define.h"

#include "../GLUS/glus_define_shader.h"

#include "../GLUS/glus_define_color.h"

#include "../GLUS/glus_define_color_channel.h"

#include "../GLUS/glus_define_uint.h"

    //
    // Image and file structures.
    //

#include "../GLUS/glus_file.h"
#include "../GLUS/glus_image.h"

    //
    // Memory manager.
    //

#include "../GLUS/glus_memory.h"

    //
    // Window preparation and creation functions.
    //

#include "../GLUS/glus_window.h"

    //
    // Version check functions.
    //

#include "../GLUS/glus_version.h"

    //
    // Extension functions.
    //

#include "../GLUS/glus_extension.h"

    //
    // Screenshot functions
    //

#include "../GLUS/glus_screenshot.h"

    //
    // Perlin noise functions.
    //

#include "../GLUS/glus_perlin.h"

    //
    // Vector functions.
    //

#include "../GLUS/glus_vector.h"

    //
    // Point functions.
    //

#include "../GLUS/glus_point.h"

    //
    // Plane functions.
    //

#include "../GLUS/glus_plane.h"

    //
    // Sphere functions.
    //

#include "../GLUS/glus_sphere.h"

    //
    // Axis Aligned Box functions.
    //

#include "../GLUS/glus_axisalignedbox.h"

    //
    // Oriented Box functions.
    //

#include "../GLUS/glus_orientedbox.h"

    //
    // Math functions
    //

#include "../GLUS/glus_math.h"

    //
    // Random functions
    //

#include "../GLUS/glus_random.h"

    //
    // View, projection etc. functions.
    //

#include "../GLUS/glus_matrix_viewprojection.h"

    //
    // Functions, related to planar surfaces.
    //

#include "../GLUS/glus_matrix_planar.h"

    //
    // Matrix functions.
    //

#include "../GLUS/glus_matrix.h"

    //
    // Quaternion functions.
    //

#include "../GLUS/glus_quaternion.h"

    //
    // Animation functions.
    //

#include "../GLUS/glus_animation.h"

    //
    // Complex numbers and vector functions.
    //

#include "../GLUS/glus_complex.h"

    //
    // Fourier functions.
    //

#include "../GLUS/glus_fourier.h"

    //
    // Shader creation function.
    //

#include "../GLUS/glus_program.h"

#include "../GLUS/glus_programpipeline.h"

    //
    // Shape / geometry functions.
    //

#include "../GLUS/glus_shape.h"

    //
    // Shape adjacency
    //

#include "../GLUS/glus_shape_adjacency.h"

    //
    // Shape texture coordinate generation
    //

#include "../GLUS/glus_shape_texgen.h"

    //
    // Line / geometry functions.
    //

#include "../GLUS/glus_line.h"
#include "../GLUS/glus_line_wavefront.h"

    //
    // Model loading functions.
    //

#include "../GLUS/glus_shape_wavefront.h"
#include "../GLUS/glus_wavefront.h"

    //
    // Logging
    //

#include "../GLUS/glus_log.h"

    //
    // Profiling
    //

#include "../GLUS/glus_profile.h"

    //
    // Time
    //

#include "../GLUS/glus_time.h"

    //
    // Ray tracing
    //

#include "../GLUS/glus_raytrace.h"

    //
    // Intersection testing
    //

#include "../GLUS/glus_intersect.h"

    //
    // Textures and files
    //

#include "../GLUS/glus_image_tga.h"
#include "../GLUS/glus_image_hdr.h"
#include "../GLUS/glus_image_pkm.h"

#include "../GLUS/glus_file_text.h"
#include "../GLUS/glus_file_binary.h"

    //
    // IBL (Image-Based Lighting)
    //

#include "../GLUS/glus_ibl.h"

    //
    // Spherical harmonics rotation
    //

    #include "../GLUS/glus_sh.h"

    //
    // glTF 2.0 loader (core spec)
    //

    #include "../GLUS/glus_gltf.h"

    //
    // Padding
    //

#include "../GLUS/glus_padding.h"

#ifdef __cplusplus
}
#endif

#endif /*__glus_h_*/

#endif /*GLUS_ES*/
