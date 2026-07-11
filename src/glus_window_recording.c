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

#define GLUS_MAX_FRAMES 10000
#define GLUS_MAX_FRAMES_PER_SECOND 120

static GLUSint   g_currentFrame  = 0;
static GLUSint   g_numberFrames  = 0;
static GLUSfloat g_recordingTime = 0.0f;

static GLUStgaimage g_tgaimage = {0, 0, 0, 0, 0};

GLUSint _glusWindowGetCurrentRecordingFrame(GLUSvoid)
{
    return g_currentFrame;
}

GLUSint _glusWindowGetCurrentAndIncreaseRecordingFrame(GLUSvoid)
{
    return g_currentFrame++;
}

GLUSint _glusWindowGetRecordingFrames(GLUSvoid)
{
    return g_numberFrames;
}

GLUSfloat _glusWindowGetRecordingTime(GLUSvoid)
{
    return g_recordingTime;
}

const GLUStgaimage* _glusWindowGetRecordingImageTga(GLUSvoid)
{
    return &g_tgaimage;
}

GLUSboolean GLUSAPIENTRY glusWindowStartRecording(GLUSint numberFrames, GLUSint framesPerSecond)
{
    glusWindowStopRecording();

    if (numberFrames < 1 || numberFrames > GLUS_MAX_FRAMES || framesPerSecond < 1 || framesPerSecond > GLUS_MAX_FRAMES_PER_SECOND)
    {
        return GLUS_FALSE;
    }

    if (!glusImageCreateTga(&g_tgaimage, glusWindowGetWidth(), glusWindowGetHeight(), 1, GLUS_RGBA))
    {
        return GLUS_FALSE;
    }

    g_numberFrames = numberFrames;

    g_recordingTime = 1.0f / (GLUSfloat)framesPerSecond;

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusWindowIsRecording(GLUSvoid)
{
    return g_tgaimage.data != 0;
}

GLUSvoid GLUSAPIENTRY glusWindowStopRecording(GLUSvoid)
{
    g_currentFrame  = 0;
    g_numberFrames  = 0;
    g_recordingTime = 0.0f;

    glusImageDestroyTga(&g_tgaimage);
}
