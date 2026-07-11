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

#ifndef GLUS_PROGRAMPIPELINE_ES31_H_
#define GLUS_PROGRAMPIPELINE_ES31_H_

/**
 * Structure for program pipeline handling.
 */
typedef struct _GLUSprogrampipeline
{
    /**
     * The created pipeline.
     */
    GLUSuint pipeline;

    /**
     * Compute shader program.
     */
    GLUSuint computeProgram;

    /**
     * Vertex shader program.
     */
    GLUSuint vertexProgram;

    /**
     * Fragment shader program.
     */
    GLUSuint fragmentProgram;

} GLUSprogrampipeline;

/**
 * Builds a program pipeline.
 *
 * @param programPipeline This structure holds the necessary information of the program pipeline and the different shader programs.
 * @param vertexProgram   Vertex shader program.
 * @param fragmentProgram Fragment shader program.
 *
 * @return GLUS_TRUE, if building of program pipeline succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusProgramPipelineBuild(GLUSprogrampipeline* programPipeline, GLUSuint vertexProgram, GLUSuint fragmentProgram);

/**
 * Builds a compute shader program pipeline.
 *
 * @param programPipeline This structure holds the necessary information of the program pipeline and the different shader programs.
 * @param computeProgram  Compute shader program.
 *
 * @return GLUS_TRUE, if building of program pipeline succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusProgramPipelineBuildCompute(GLUSprogrampipeline* programPipeline, GLUSuint computeProgram);

/**
 * Destroys a program pipeline by freeing the pipeline. The programs are not freed.
 *
 * @param programPipeline This structure holds the necessary information of the program pipeline and the different shader programs.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusProgramPipelineDestroy(GLUSprogrampipeline* programPipeline);

#endif /* GLUS_PROGRAMPIPELINE_ES31_H_ */
