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

/* Return the index i such that times[i] <= t < times[i+1].
 * Returns 0 when t is before the first keyframe, and count-2 when t is at or
 * past the last keyframe (so the caller always has a valid [i, i+1] interval). */
static GLUSint animationFindSegment(const GLUSfloat* times, GLUSint count, GLUSfloat t)
{
    GLUSint i;

    if (t <= times[0])
    {
        return 0;
    }

    for (i = 0; i < count - 1; i++)
    {
        if (t < times[i + 1])
        {
            return i;
        }
    }

    return count - 2;
}

GLUSvoid GLUSAPIENTRY glusAnimationSampleVec3f(GLUSfloat result[3], const GLUSfloat* times, const GLUSfloat* values, GLUSint count, GLUSint interpolation, GLUSfloat t)
{
    GLUSint          i, j;
    GLUSfloat        u, delta;
    GLUSint          stride;
    GLUSint          vertexOffset;
    const GLUSfloat* v0;
    const GLUSfloat* v1;
    const GLUSfloat* m0;
    const GLUSfloat* m1;

    if (count <= 0)
    {
        result[0] = result[1] = result[2] = 0.0f;
        return;
    }

    stride       = (interpolation == GLUS_ANIMATION_CUBICSPLINE) ? 9 : 3;
    vertexOffset = (interpolation == GLUS_ANIMATION_CUBICSPLINE) ? 3 : 0;

    /* Clamp before first keyframe. */
    if (count == 1 || t <= times[0])
    {
        glusVector3Copyf(result, values + vertexOffset);
        return;
    }

    /* Clamp after last keyframe. */
    if (t >= times[count - 1])
    {
        glusVector3Copyf(result, values + (count - 1) * stride + vertexOffset);
        return;
    }

    i = animationFindSegment(times, count, t);

    v0 = values + i * stride + vertexOffset;
    v1 = values + (i + 1) * stride + vertexOffset;

    if (interpolation == GLUS_ANIMATION_STEP)
    {
        glusVector3Copyf(result, v0);
        return;
    }

    u = (t - times[i]) / (times[i + 1] - times[i]);

    if (interpolation == GLUS_ANIMATION_LINEAR)
    {
        glusVector3Mixf(result, v0, v1, u);
        return;
    }

    /* CUBICSPLINE: cubic Hermite spline per component. */
    delta = times[i + 1] - times[i];
    m0    = values + i * stride + 6;   /* outTangent of keyframe i   */
    m1    = values + (i + 1) * stride; /* inTangent of keyframe i+1  */

    for (j = 0; j < 3; j++)
    {
        result[j] = glusMathCubicHermitef(v0[j], delta * m0[j], v1[j], delta * m1[j], u);
    }
}

GLUSvoid GLUSAPIENTRY glusAnimationSampleQuaternionf(GLUSfloat result[4], const GLUSfloat* times, const GLUSfloat* values, GLUSint count, GLUSint interpolation, GLUSfloat t)
{
    GLUSint          i, j;
    GLUSfloat        u, delta;
    GLUSfloat        dot;
    GLUSint          stride;
    GLUSint          vertexOffset;
    const GLUSfloat* v0;
    const GLUSfloat* v1;
    const GLUSfloat* m0;
    const GLUSfloat* m1;
    GLUSfloat        q1[4];

    if (count <= 0)
    {
        glusQuaternionIdentityf(result);
        return;
    }

    stride       = (interpolation == GLUS_ANIMATION_CUBICSPLINE) ? 12 : 4;
    vertexOffset = (interpolation == GLUS_ANIMATION_CUBICSPLINE) ? 4 : 0;

    /* Clamp before first keyframe. */
    if (count == 1 || t <= times[0])
    {
        glusQuaternionCopyf(result, values + vertexOffset);
        return;
    }

    /* Clamp after last keyframe. */
    if (t >= times[count - 1])
    {
        glusQuaternionCopyf(result, values + (count - 1) * stride + vertexOffset);
        return;
    }

    i = animationFindSegment(times, count, t);

    v0 = values + i * stride + vertexOffset;
    v1 = values + (i + 1) * stride + vertexOffset;

    if (interpolation == GLUS_ANIMATION_STEP)
    {
        glusQuaternionCopyf(result, v0);
        return;
    }

    u = (t - times[i]) / (times[i + 1] - times[i]);

    if (interpolation == GLUS_ANIMATION_LINEAR)
    {
        /* Flip q1 to shortest arc when dot product is negative. */
        dot = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3];
        if (dot < 0.0f)
        {
            for (j = 0; j < 4; j++)
            {
                q1[j] = -v1[j];
            }
        }
        else
        {
            glusQuaternionCopyf(q1, v1);
        }

        glusQuaternionNlerpf(result, v0, q1, u);
        return;
    }

    /* CUBICSPLINE: cubic Hermite spline per component, then normalize. */
    delta = times[i + 1] - times[i];
    m0    = values + i * stride + 8;   /* outTangent of keyframe i   */
    m1    = values + (i + 1) * stride; /* inTangent of keyframe i+1  */

    for (j = 0; j < 4; j++)
    {
        result[j] = glusMathCubicHermitef(v0[j], delta * m0[j], v1[j], delta * m1[j], u);
    }

    glusQuaternionNormalizef(result);
}
