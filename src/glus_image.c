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

GLUSvoid _glusImageGatherSamplePoints(GLUSint sampleIndex[4], GLUSfloat sampleWeight[2], const GLUSfloat st[2], GLUSint width, GLUSint height, GLUSint stride)
{
    GLUSfloat pixelTexCoord[2];
    GLUSfloat pixelTexCoordCenter[2];

    GLUSint samplePixel[2];

    GLUSint i;

    for (i = 0; i < 2; i++)
    {
        pixelTexCoord[i] = glusMathClampf(st[i], 0.0f, 1.0f);
    }

    pixelTexCoord[0] = pixelTexCoord[0] * (GLUSfloat)width;
    pixelTexCoord[1] = pixelTexCoord[1] * (GLUSfloat)height;

    for (i = 0; i < 2; i++)
    {
        pixelTexCoordCenter[i] = floorf(pixelTexCoord[i]) + 0.5f;
    }

    for (i = 0; i < 2; i++)
    {
        sampleWeight[i] = 1.0f - fabsf(pixelTexCoordCenter[i] - pixelTexCoord[i]);
    }

    // Check, if in bounds.

    samplePixel[0] = (GLUSint)pixelTexCoord[0];
    if (samplePixel[0] == width)
    {
        samplePixel[0]--;
    }

    samplePixel[1] = (GLUSint)pixelTexCoord[1];
    if (samplePixel[1] == height)
    {
        samplePixel[1]--;
    }

    // Main sample point

    sampleIndex[0] = samplePixel[1] * width * stride + samplePixel[0] * stride;

    // s axis sample point

    if (pixelTexCoordCenter[0] > 0.5f && pixelTexCoord[0] < pixelTexCoordCenter[0])
    {
        sampleIndex[1] = sampleIndex[0] - stride;
        sampleIndex[3] = -stride;
    }
    else if (pixelTexCoordCenter[0] < (GLUSfloat)width - 0.5f && pixelTexCoord[0] > pixelTexCoordCenter[0])
    {
        sampleIndex[1] = sampleIndex[0] + stride;
        sampleIndex[3] = stride;
    }
    else
    {
        sampleIndex[1] = sampleIndex[0];
        sampleIndex[3] = 0;
    }

    // t axis sample point

    if (pixelTexCoordCenter[1] > 0.5f && pixelTexCoord[1] < pixelTexCoordCenter[1])
    {
        sampleIndex[2] = sampleIndex[0] - stride * width;
    }
    else if (pixelTexCoordCenter[1] < (GLUSfloat)height - 0.5f && pixelTexCoord[1] > pixelTexCoordCenter[1])
    {
        sampleIndex[2] = sampleIndex[0] + stride * width;
    }
    else
    {
        sampleIndex[2] = sampleIndex[0];
    }

    // diagonal sample point

    sampleIndex[3] += sampleIndex[2];
}
