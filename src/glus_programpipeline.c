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

GLUSboolean GLUSAPIENTRY glusProgramPipelineBuild(GLUSprogrampipeline* programPipeline, GLUSuint vertexProgram, GLUSuint controlProgram, GLUSuint evaluationProgram, GLUSuint geometryProgram, GLUSuint fragmentProgram)
{
    if (!programPipeline)
    {
        return GLUS_FALSE;
    }

    programPipeline->computeProgram    = 0;
    programPipeline->vertexProgram     = vertexProgram;
    programPipeline->controlProgram    = controlProgram;
    programPipeline->evaluationProgram = evaluationProgram;
    programPipeline->geometryProgram   = geometryProgram;
    programPipeline->fragmentProgram   = fragmentProgram;

    glGenProgramPipelines(1, &programPipeline->pipeline);
    if (vertexProgram)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_VERTEX_SHADER_BIT, vertexProgram);
    }
    if (controlProgram)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_TESS_CONTROL_SHADER_BIT, controlProgram);
    }
    if (evaluationProgram)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_TESS_EVALUATION_SHADER_BIT, evaluationProgram);
    }
    if (geometryProgram)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_GEOMETRY_SHADER_BIT, geometryProgram);
    }
    if (fragmentProgram)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_FRAGMENT_SHADER_BIT, fragmentProgram);
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusProgramPipelineBuildCompute(GLUSprogrampipeline* programPipeline, GLUSuint computeProgram)
{
    if (!programPipeline)
    {
        return GLUS_FALSE;
    }

    programPipeline->computeProgram    = computeProgram;
    programPipeline->vertexProgram     = 0;
    programPipeline->controlProgram    = 0;
    programPipeline->evaluationProgram = 0;
    programPipeline->geometryProgram   = 0;
    programPipeline->fragmentProgram   = 0;

    glGenProgramPipelines(1, &programPipeline->pipeline);
    if (computeProgram)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_COMPUTE_SHADER_BIT, computeProgram);
    }

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusProgramPipelineDestroy(GLUSprogrampipeline* programPipeline)
{
    if (!programPipeline)
    {
        return;
    }

    if (programPipeline->pipeline)
    {
        glUseProgramStages(programPipeline->pipeline, GLUS_COMPUTE_SHADER_BIT, 0);
        glUseProgramStages(programPipeline->pipeline, GLUS_VERTEX_SHADER_BIT, 0);
        glUseProgramStages(programPipeline->pipeline, GLUS_TESS_CONTROL_SHADER_BIT, 0);
        glUseProgramStages(programPipeline->pipeline, GLUS_TESS_EVALUATION_SHADER_BIT, 0);
        glUseProgramStages(programPipeline->pipeline, GLUS_GEOMETRY_SHADER_BIT, 0);
        glUseProgramStages(programPipeline->pipeline, GLUS_FRAGMENT_SHADER_BIT, 0);

        glDeleteProgramPipelines(1, &programPipeline->pipeline);

        programPipeline->pipeline = 0;
    }

    programPipeline->computeProgram    = 0;
    programPipeline->fragmentProgram   = 0;
    programPipeline->evaluationProgram = 0;
    programPipeline->controlProgram    = 0;
    programPipeline->vertexProgram     = 0;
}
