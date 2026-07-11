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

#define GLUS_MAX_CHARS_LOGGING 2047

static const char* GLUS_LOG_STRINGS[] = {
    "",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "SEVERE"};

static GLUSuint g_verbosity = GLUS_LOG_INFO;

GLUSvoid GLUSAPIENTRY glusLogSetLevel(const GLUSuint verbosity)
{
    g_verbosity = verbosity;
}

GLUSuint GLUSAPIENTRY glusLogGetLevel()
{
    return g_verbosity;
}

GLUSvoid GLUSAPIENTRY glusLogPrint(GLUSuint verbosity, const char* format, ...)
{
    if (g_verbosity == GLUS_LOG_NOTHING || verbosity == GLUS_LOG_NOTHING)
    {
        return;
    }

    if (g_verbosity >= verbosity)
    {
        const char* logString = "UNKNOWN";
        char        buffer[GLUS_MAX_CHARS_LOGGING + 1];
        va_list     argList;

        if (verbosity > GLUS_LOG_NOTHING && verbosity <= GLUS_LOG_SEVERE)
        {
            logString = GLUS_LOG_STRINGS[verbosity];
        }

        buffer[GLUS_MAX_CHARS_LOGGING] = '\0';

        va_start(argList, format);

        vsnprintf(buffer, GLUS_MAX_CHARS_LOGGING, format, argList);

        printf("LOG [%s]: %s\n", logString, buffer);

        va_end(argList);
    }
}

GLUSvoid GLUSAPIENTRY glusLogPrintError(GLUSuint verbosity, const char* format, ...)
{
    GLUSenum error;

    error = glGetError();

    if (g_verbosity == GLUS_LOG_NOTHING || verbosity == GLUS_LOG_NOTHING)
    {
        return;
    }

    if (verbosity < GLUS_LOG_DEBUG && error == GLUS_NO_ERROR)
    {
        return;
    }

    if (g_verbosity >= verbosity)
    {
        const char* logString = "UNKNOWN";
        char        buffer[GLUS_MAX_CHARS_LOGGING + 1];
        va_list     argList;

        if (verbosity > GLUS_LOG_NOTHING && verbosity <= GLUS_LOG_SEVERE)
        {
            logString = GLUS_LOG_STRINGS[verbosity];
        }

        buffer[GLUS_MAX_CHARS_LOGGING] = '\0';

        va_start(argList, format);

        vsnprintf(buffer, GLUS_MAX_CHARS_LOGGING, format, argList);

        printf("LOG [%s]: glGetError() = 0x%x %s\n", logString, error, buffer);

        va_end(argList);
    }
}
