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

/**
 * Prints the info log of a shader. Drivers are allowed to report a log length of zero, so
 * this case has to be handled and the buffer always has to be terminated.
 */
static GLUSvoid glusProgramPrintShaderLog(const GLUSuint shader, const GLUSchar* message)
{
    GLUSint logLength = 0;
    GLUSint charsWritten;

    GLUSchar* log;

    glusLogPrint(GLUS_LOG_ERROR, "%s", message);

    glGetShaderiv(shader, GLUS_INFO_LOG_LENGTH, &logLength);

    if (logLength <= 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "No info log available.");

        return;
    }

    log = (GLUSchar*)glusMemoryMalloc((size_t)logLength + 1);

    if (!log)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Info log could not be allocated.");

        return;
    }

    log[0] = '\0';

    glGetShaderInfoLog(shader, logLength, &charsWritten, log);

    log[logLength] = '\0';

    glusLogPrint(GLUS_LOG_ERROR, "%s", log);

    glusMemoryFree(log);
}

/**
 * Prints the info log of a program. See glusProgramPrintShaderLog for the details.
 */
static GLUSvoid glusProgramPrintProgramLog(const GLUSuint program, const GLUSchar* message)
{
    GLUSint logLength = 0;
    GLUSint charsWritten;

    GLUSchar* log;

    glusLogPrint(GLUS_LOG_ERROR, "%s", message);

    glGetProgramiv(program, GLUS_INFO_LOG_LENGTH, &logLength);

    if (logLength <= 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "No info log available.");

        return;
    }

    log = (GLUSchar*)glusMemoryMalloc((size_t)logLength + 1);

    if (!log)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Info log could not be allocated.");

        return;
    }

    log[0] = '\0';

    glGetProgramInfoLog(program, logLength, &charsWritten, log);

    log[logLength] = '\0';

    glusLogPrint(GLUS_LOG_ERROR, "%s", log);

    glusMemoryFree(log);
}

GLUSboolean GLUSAPIENTRY glusProgramCreateFromSource(GLUSprogram* shaderProgram, const GLUSchar** vertexSource, const GLUSchar** controlSource, const GLUSchar** evaluationSource, const GLUSchar** geometrySource, const GLUSchar** fragmentSource)
{
    GLUSint compiled = GLUS_FALSE;

    if (!shaderProgram || !vertexSource || !fragmentSource)
    {
        return GLUS_FALSE;
    }

    shaderProgram->program    = 0;
    shaderProgram->compute    = 0;
    shaderProgram->vertex     = 0;
    shaderProgram->control    = 0;
    shaderProgram->evaluation = 0;
    shaderProgram->geometry   = 0;
    shaderProgram->fragment   = 0;

    shaderProgram->vertex = glCreateShader(GLUS_VERTEX_SHADER);

    glShaderSource(shaderProgram->vertex, 1, (const char**)vertexSource, 0);

    glCompileShader(shaderProgram->vertex);

    glGetShaderiv(shaderProgram->vertex, GLUS_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        glusProgramPrintShaderLog(shaderProgram->vertex, "Vertex shader compile error:");

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    if (controlSource)
    {
        shaderProgram->control = glCreateShader(GLUS_TESS_CONTROL_SHADER);

        glShaderSource(shaderProgram->control, 1, (const char**)controlSource, 0);

        glCompileShader(shaderProgram->control);

        glGetShaderiv(shaderProgram->control, GLUS_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            glusProgramPrintShaderLog(shaderProgram->control, "Control shader compile error:");

            glusProgramDestroy(shaderProgram);

            return GLUS_FALSE;
        }
    }

    if (evaluationSource)
    {
        shaderProgram->evaluation = glCreateShader(GLUS_TESS_EVALUATION_SHADER);

        glShaderSource(shaderProgram->evaluation, 1, (const char**)evaluationSource, 0);

        glCompileShader(shaderProgram->evaluation);

        glGetShaderiv(shaderProgram->evaluation, GLUS_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            glusProgramPrintShaderLog(shaderProgram->evaluation, "Evaluation shader compile error:");

            glusProgramDestroy(shaderProgram);

            return GLUS_FALSE;
        }
    }

    if (geometrySource)
    {
        shaderProgram->geometry = glCreateShader(GLUS_GEOMETRY_SHADER);

        glShaderSource(shaderProgram->geometry, 1, (const char**)geometrySource, 0);

        glCompileShader(shaderProgram->geometry);

        glGetShaderiv(shaderProgram->geometry, GLUS_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            glusProgramPrintShaderLog(shaderProgram->geometry, "Geometry shader compile error:");

            glusProgramDestroy(shaderProgram);

            return GLUS_FALSE;
        }
    }

    shaderProgram->fragment = glCreateShader(GLUS_FRAGMENT_SHADER);

    glShaderSource(shaderProgram->fragment, 1, (const char**)fragmentSource, 0);

    glCompileShader(shaderProgram->fragment);

    glGetShaderiv(shaderProgram->fragment, GLUS_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        glusProgramPrintShaderLog(shaderProgram->fragment, "Fragment shader compile error:");

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    shaderProgram->program = glCreateProgram();

    glAttachShader(shaderProgram->program, shaderProgram->vertex);

    if (shaderProgram->control)
    {
        glAttachShader(shaderProgram->program, shaderProgram->control);
    }

    if (shaderProgram->evaluation)
    {
        glAttachShader(shaderProgram->program, shaderProgram->evaluation);
    }

    if (shaderProgram->geometry)
    {
        glAttachShader(shaderProgram->program, shaderProgram->geometry);
    }

    glAttachShader(shaderProgram->program, shaderProgram->fragment);

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusProgramCreateComputeFromSource(GLUSprogram* shaderProgram, const GLUSchar** computeSource)
{
    GLUSint compiled = GLUS_FALSE;

    if (!shaderProgram || !computeSource)
    {
        return GLUS_FALSE;
    }

    shaderProgram->program    = 0;
    shaderProgram->compute    = 0;
    shaderProgram->vertex     = 0;
    shaderProgram->control    = 0;
    shaderProgram->evaluation = 0;
    shaderProgram->geometry   = 0;
    shaderProgram->fragment   = 0;

    shaderProgram->compute = glCreateShader(GLUS_COMPUTE_SHADER);

    glShaderSource(shaderProgram->compute, 1, (const char**)computeSource, 0);

    glCompileShader(shaderProgram->compute);

    glGetShaderiv(shaderProgram->compute, GLUS_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        glusProgramPrintShaderLog(shaderProgram->compute, "Compute shader compile error:");

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    shaderProgram->program = glCreateProgram();

    glAttachShader(shaderProgram->program, shaderProgram->compute);

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusProgramLink(GLUSprogram* shaderProgram)
{
    GLUSint linked = GLUS_FALSE;

    if (!shaderProgram)
    {
        return GLUS_FALSE;
    }

    glLinkProgram(shaderProgram->program);

    glGetProgramiv(shaderProgram->program, GLUS_LINK_STATUS, &linked);

    if (!linked)
    {
        glusProgramPrintProgramLog(shaderProgram->program, "Shader program link error:");

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusProgramBuildFromSource(GLUSprogram* shaderProgram, const GLUSchar** vertexSource, const GLUSchar** controlSource, const GLUSchar** evaluationSource, const GLUSchar** geometrySource, const GLUSchar** fragmentSource)
{
    if (!glusProgramCreateFromSource(shaderProgram, vertexSource, controlSource, evaluationSource, geometrySource, fragmentSource))
    {
        return GLUS_FALSE;
    }

    return glusProgramLink(shaderProgram);
}

GLUSboolean GLUSAPIENTRY glusProgramBuildComputeFromSource(GLUSprogram* shaderProgram, const GLUSchar** computeSource)
{
    if (!glusProgramCreateComputeFromSource(shaderProgram, computeSource))
    {
        return GLUS_FALSE;
    }

    return glusProgramLink(shaderProgram);
}

GLUSboolean GLUSAPIENTRY glusProgramBuildSeparableFromSource(GLUSprogram* shaderProgram, const GLUSenum type, const GLUSchar** source)
{
    GLUSint linked = GLUS_FALSE;

    if (!glusVersionIsSupported(4, 1))
    {
        glusLogPrint(GLUS_LOG_ERROR, "Function needs OpenGL version 4.1 or higher");

        return GLUS_FALSE;
    }

    if (!shaderProgram || !source)
    {
        return GLUS_FALSE;
    }

    shaderProgram->program    = 0;
    shaderProgram->compute    = 0;
    shaderProgram->vertex     = 0;
    shaderProgram->control    = 0;
    shaderProgram->evaluation = 0;
    shaderProgram->geometry   = 0;
    shaderProgram->fragment   = 0;

    shaderProgram->program = glCreateShaderProgramv(type, 1, (const char**)source);

    glGetProgramiv(shaderProgram->program, GLUS_LINK_STATUS, &linked);

    if (!linked)
    {
        glusProgramPrintProgramLog(shaderProgram->program, "Shader program link error:");

        glusProgramDestroy(shaderProgram);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
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

    if (shaderprogram->geometry)
    {
        glDeleteShader(shaderprogram->geometry);

        shaderprogram->geometry = 0;
    }

    if (shaderprogram->evaluation)
    {
        glDeleteShader(shaderprogram->evaluation);

        shaderprogram->evaluation = 0;
    }

    if (shaderprogram->control)
    {
        glDeleteShader(shaderprogram->control);

        shaderprogram->control = 0;
    }

    if (shaderprogram->vertex)
    {
        glDeleteShader(shaderprogram->vertex);

        shaderprogram->vertex = 0;
    }

    if (shaderprogram->compute)
    {
        glDeleteShader(shaderprogram->compute);

        shaderprogram->compute = 0;
    }
}
