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

#ifndef GLUS_ANIMATION_H_
#define GLUS_ANIMATION_H_

/**
 * Keyframe interpolation: output equals the previous keyframe value (no blending).
 */
#define GLUS_ANIMATION_STEP 0

/**
 * Keyframe interpolation: linear interpolation between adjacent keyframe values.
 * Quaternion tracks use normalized linear interpolation (nlerp).
 */
#define GLUS_ANIMATION_LINEAR 1

/**
 * Keyframe interpolation: cubic Hermite spline.
 * Each keyframe stores three consecutive values: inTangent, vertex, outTangent.
 * The values array stride per keyframe is therefore 3 * components
 * (9 floats for vec3, 12 floats for quaternion).
 */
#define GLUS_ANIMATION_CUBICSPLINE 2

/**
 * Sample a vec3 keyframe track at time t.
 *
 * @param result       Output vec3.
 * @param times        Array of count keyframe times in seconds (ascending).
 * @param values       Keyframe values. Layout depends on interpolation:
 *                       STEP / LINEAR:   count * 3 floats (one vec3 per keyframe).
 *                       CUBICSPLINE:     count * 9 floats (inTangent[3], vertex[3],
 *                                        outTangent[3] per keyframe).
 * @param count        Number of keyframes.
 * @param interpolation GLUS_ANIMATION_STEP, GLUS_ANIMATION_LINEAR, or
 *                       GLUS_ANIMATION_CUBICSPLINE.
 * @param t            Current time in seconds.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusAnimationSampleVec3f(GLUSfloat result[3], const GLUSfloat* times, const GLUSfloat* values, GLUSint count, GLUSint interpolation, GLUSfloat t);

/**
 * Sample a quaternion keyframe track at time t.
 *
 * Quaternions are stored as (x, y, z, w).  For LINEAR interpolation the two
 * adjacent quaternions are blended with nlerp; the sign of the second quaternion
 * is flipped when needed to ensure the shortest-arc rotation.  CUBICSPLINE output
 * is normalized after evaluation.
 *
 * @param result       Output quaternion (x, y, z, w).
 * @param times        Array of count keyframe times in seconds (ascending).
 * @param values       Keyframe values. Layout depends on interpolation:
 *                       STEP / LINEAR:   count * 4 floats (one quaternion per keyframe).
 *                       CUBICSPLINE:     count * 12 floats (inTangent[4], vertex[4],
 *                                        outTangent[4] per keyframe).
 * @param count        Number of keyframes.
 * @param interpolation GLUS_ANIMATION_STEP, GLUS_ANIMATION_LINEAR, or
 *                       GLUS_ANIMATION_CUBICSPLINE.
 * @param t            Current time in seconds.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusAnimationSampleQuaternionf(GLUSfloat result[4], const GLUSfloat* times, const GLUSfloat* values, GLUSint count, GLUSint interpolation, GLUSfloat t);

#endif /* GLUS_ANIMATION_H_ */
