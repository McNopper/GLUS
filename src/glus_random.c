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

#define GLUS_UNIFORM_RANDOM_BIAS 0.0000001f

GLUSvoid GLUSAPIENTRY glusRandomSetSeed(const GLUSuint seed)
{
    srand(seed);
}

GLUSfloat GLUSAPIENTRY glusRandomUniformf(const GLUSfloat start, const GLUSfloat end)
{
    return ((GLUSfloat)rand() / (GLUSfloat)RAND_MAX) * (end - start) + start;
}

// see http://mathworld.wolfram.com/Box-MullerTransformation.html

GLUSfloat GLUSAPIENTRY glusRandomNormalf(const GLUSfloat mean, const GLUSfloat standardDeviation)
{
    GLUSfloat x1, x2;

    // Avoid logf(0.0f) and logf(1.0f)
    x1 = glusRandomUniformf(GLUS_UNIFORM_RANDOM_BIAS, 1.0f - GLUS_UNIFORM_RANDOM_BIAS);
    x2 = glusRandomUniformf(0.0f, 1.0f);

    return mean + standardDeviation * (sqrtf(-2.0f * logf(x1)) * cosf(2.0f * GLUS_PI * x2));
}

// see http://mathworld.wolfram.com/HammersleyPointSet.html
// see https://github.com/wdas/brdf/blob/master/src/shaderTemplates/brdfIBL.frag

GLUSboolean GLUSAPIENTRY glusRandomHammersleyf(GLUSfloat result[2], const GLUSuint sample, const GLUSubyte m)
{
    GLUSuint  revertSample;
    GLUSfloat binaryFractionFactor;

    if (!result)
    {
        return GLUS_FALSE;
    }

    // Check, if m is in the allowed range, as only 32bit unsigned integer is supported.
    if (m == 0 || m > 32)
    {
        return GLUS_FALSE;
    }

    // If not all bits are used: Check, if sample is out of bounds.
    if (m < 32 && sample >= (GLUSuint)(1 << m))
    {
        return GLUS_FALSE;
    }

    // Revert bits by swapping blockwise. Lower bits are moved up and higher bits down.
    revertSample = (sample << 16u) | (sample >> 16u);
    revertSample = ((revertSample & 0x00ff00ffu) << 8u) | ((revertSample & 0xff00ff00u) >> 8u);
    revertSample = ((revertSample & 0x0f0f0f0fu) << 4u) | ((revertSample & 0xf0f0f0f0u) >> 4u);
    revertSample = ((revertSample & 0x33333333u) << 2u) | ((revertSample & 0xccccccccu) >> 2u);
    revertSample = ((revertSample & 0x55555555u) << 1u) | ((revertSample & 0xaaaaaaaau) >> 1u);

    // Shift back, as only m bits are used.
    revertSample = revertSample >> (32 - m);

    // Results are in range [0.0 1.0] and not [0.0, 1.0[.
    binaryFractionFactor = 1.0f / (powf(2.0f, (GLUSfloat)m) - 1.0f);

    result[0] = (GLUSfloat)revertSample * binaryFractionFactor;
    result[1] = (GLUSfloat)sample * binaryFractionFactor;

    return GLUS_TRUE;
}
