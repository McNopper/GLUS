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

GLUSint GLUSAPIENTRY glusIntersectRaySpheref(GLUSfloat* tNear, GLUSfloat* tFar, GLUSboolean* insideSphere, const GLUSfloat rayStart[4], const GLUSfloat rayDirection[3], const GLUSfloat sphereCenter[4], const GLUSfloat radius)
{
    // see http://de.wikipedia.org/wiki/Quadratische_Gleichung (German)
    // see Real-Time Collision Detection p177

    GLUSint   intersections = 0;
    GLUSfloat m[3];
    GLUSfloat b, c, discriminant, sqrtDiscriminant, t;

    glusPoint4SubtractPoint4f(m, rayStart, sphereCenter);

    b = glusVector3Dotf(m, rayDirection);
    c = glusVector3Dotf(m, m) - radius * radius;

    // Ray pointing away from sphere and ray starts outside sphere.
    if (b > 0.0f && c > 0.0f)
    {
        return intersections;
    }

    discriminant = b * b - c;

    // No solution, so no intersection. Ray passes the sphere.
    if (discriminant < 0.0f)
    {
        return intersections;
    }

    sqrtDiscriminant = sqrtf(discriminant);

    // If we come so far, we have at least one intersection.

    if (insideSphere)
    {
        *insideSphere = GLUS_FALSE;
    }

    t = -b - sqrtDiscriminant;

    // Ray starts inside the sphere, as t is negative.
    if (t < 0.0f)
    {
        // Use origin as intersection point.
        t = 0.0f;

        if (insideSphere)
        {
            *insideSphere = GLUS_TRUE;
        }
    }

    intersections++;

    if (tNear)
    {
        *tNear = t;
    }

    // Tangent point. Only one intersection. So leave.
    if (discriminant == 0.0f)
    {
        // Save in both for convenience.
        if (tFar)
        {
            *tFar = t;
        }

        return intersections;
    }

    //

    t = -b + sqrtDiscriminant;

    intersections++;

    if (tFar)
    {
        *tFar = t;
    }

    return intersections;
}
