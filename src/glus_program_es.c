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

GLUSboolean GLUSAPIENTRY glusProgramCreateFromSource(GLUSprogram* shaderProgram, const GLUSchar** vertexSource, const GLUSchar** fragmentSource)
{
    GLUSint compiled;

    GLUSint logLength, charsWritten;

    char* log;

    if (!shaderProgram || !vertexSource || !fragmentSource)
    {
        return GLUS_FALSE;
    }

    shaderProgram->program  = 0;
    shaderProgram->vertex   = 0;
    shaderProgram->fragment = 0;

    shaderProgram->vertex = glCreateShader(GLUS_VERTEX_SHADER);

    glShaderSource(shaderProgram->vertex, 1, (const char**)vertexSource, 0);

    glCompileShader(shaderProgram->vertex);

    glGetShaderiv(shaderProgram->vertex, GLUS_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        glGetShaderiv(shaderProgram->vertex, GLUS_INFO_LOG_LENGTH, &logLength);

        log = (char*)glusMemoryMalloc((size_t)logLength);

        if (!log)
        {
            return GLUS_FALSE;
        }

        glGetShaderInfoLog(shaderProgram->vertex, logLength, &charsWritten, log);

        glusLogPrint(GLUS_LOG_ERROR, "Vertex shader compile error:");
        glusLogPrint(GLUS_LOG_ERROR, "%s", log);

        glusMemoryFree(log);

        shaderProgram->vertex = 0;

        return GLUS_FALSE;
    }

    shaderProgram->fragment = glCreateShader(GLUS_FRAGMENT_SHADER);

    glShaderSource(shaderProgram->fragment, 1, (const char**)fragmentSource, 0);

    glCompileShader(shaderProgram->fragment);

    glGetShaderiv(shaderProgram->fragment, GLUS_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        glGetShaderiv(shaderProgram->fragment, GLUS_INFO_LOG_LENGTH, &logLength);

        log = (char*)glusMemoryMalloc((size_t)logLength);

        if (!log)
        {
            glusProgramDestroy(shaderProgram);

            return GLUS_FALSE;
        }

        glGetShaderInfoLog(shaderProgram->fragment, logLength, &charsWritten, log);

        glusLogPrint(GLUS_LOG_ERROR, "Fragment shader compile error:");
        glusLogPrint(GLUS_LOG_ERROR, "%s", log);

        glusMemoryFree(log);

        shaderProgram->fragment = 0;

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    shaderProgram->program = glCreateProgram();

    glAttachShader(shaderProgram->program, shaderProgram->vertex);

    glAttachShader(shaderProgram->program, shaderProgram->fragment);

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusProgramLink(GLUSprogram* shaderProgram)
{
    GLUSint linked;

    GLUSint logLength, charsWritten;

    char* log;

    if (!shaderProgram)
    {
        return GLUS_FALSE;
    }

    glLinkProgram(shaderProgram->program);

    glGetProgramiv(shaderProgram->program, GLUS_LINK_STATUS, &linked);

    if (!linked)
    {
        glGetProgramiv(shaderProgram->program, GLUS_INFO_LOG_LENGTH, &logLength);

        log = (char*)glusMemoryMalloc((size_t)logLength);

        if (!log)
        {
            glusProgramDestroy(shaderProgram);

            return GLUS_FALSE;
        }

        glGetProgramInfoLog(shaderProgram->program, logLength, &charsWritten, log);

        glusLogPrint(GLUS_LOG_ERROR, "Shader program link error:");
        glusLogPrint(GLUS_LOG_ERROR, "%s", log);

        glusMemoryFree(log);

        shaderProgram->program = 0;

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusProgramBuildFromSource(GLUSprogram* shaderProgram, const GLUSchar** vertexSource, const GLUSchar** fragmentSource)
{
    if (!glusProgramCreateFromSource(shaderProgram, vertexSource, fragmentSource))
    {
        return GLUS_FALSE;
    }

    return glusProgramLink(shaderProgram);
}

GLUSvoid GLUSAPIENTRY glusProgramDestroy(GLUSprogram* shaderprogram)
{
    if (!shaderprogram)
    {
        return;
    }

    if (shaderprogram->program)
    {
        glDeleteProgram(shaderprogram->program);

        shaderprogram->program = 0;
    }

    if (shaderprogram->fragment)
    {
        glDeleteShader(shaderprogram->fragment);

        shaderprogram->fragment = 0;
    }

    if (shaderprogram->vertex)
    {
        glDeleteShader(shaderprogram->vertex);

        shaderprogram->vertex = 0;
    }
}
