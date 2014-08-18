/*
 * EnvironmentBRDF - Creates a look up table needed for BRDF of the IBL approximation as seen in
 * http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf page 6.
 * Copyright (C) 2014 Norbert Nopper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GL/glus.h"

#define MAX_FILENAME_LENGTH		1024	// with '\0'
#define MAX_FILETYPE_LENGTH		5		// with '\0'
#define MAX_RESOLUTION_LENGTH	5		// without '\0'

int main(int argc, char* argv[])
{
	GLUSbinaryfile binaryfile;

	GLUSchar* output;
	GLUSchar buffer[MAX_FILENAME_LENGTH];

	GLUSuint		exponent;
	GLUSuint		samples;

	GLUSint i, k, ouputLength;

	GLUSint	length;

	GLUStextfile computeSource;
	GLUSshaderprogram computeProgram;

	GLUSuint localSize = 16;

	GLUSuint textureCookTorrance;

	GLUSuint scanVectorsSSBO;

	GLUSint mLocation;
	GLUSint samplesLocation;
	GLUSint binaryFractionFactorLocation;

	EGLint eglConfigAttributes[] = {
	        EGL_RED_SIZE, 8,
	        EGL_GREEN_SIZE, 8,
	        EGL_BLUE_SIZE, 8,
	        EGL_DEPTH_SIZE, 0,
	        EGL_STENCIL_SIZE, 0,
	        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	        EGL_NONE
	};

    EGLint eglContextAttributes[] = {
    		EGL_CONTEXT_MAJOR_VERSION, 4,
    		EGL_CONTEXT_MINOR_VERSION, 3,
    		EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_TRUE,
    		EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
    		EGL_NONE
    };

	if (argc != 4)
	{
		printf("Usage: EnvironmentBRDF.exe [Output] [Length] [Samples 2^m]\n");

		return -1;
	}

	//

	output = argv[1];

	ouputLength = strlen(output);

	if (ouputLength >= MAX_FILENAME_LENGTH - MAX_RESOLUTION_LENGTH - (MAX_FILETYPE_LENGTH - 1))
	{
		printf("Error: Output filename too long.\n");

		return -1;
	}

	length = atoi(argv[2]);

	if (length < 2 || length > 8192)
	{
		printf("Error: Invalid length value.\n");

		return -1;
	}

	exponent = (GLUSuint)atoi(argv[3]);

	if (exponent > 16)
	{
		printf("Error: Invalid samples calue.\n");

		return -1;
	}

	samples = 1 << exponent;

	//

	binaryfile.binary = (GLUSubyte*)malloc(length * length * 2 * sizeof(GLUSfloat));

	if(!binaryfile.binary)
	{
		printf("Error: Color buffer could not be created.\n");

		return -1;
	}

	binaryfile.length = length * length * 2 * sizeof(GLUSfloat);

	//
	// Initialize OpenGL, as it is needed for the compute shader.
	//

	if (!glusCreateWindow("GLUS Example Window", 512, 512, GLUS_FALSE, GLUS_FALSE, eglConfigAttributes, eglContextAttributes))
	{
		printf("Could not create window!\n");

		return -1;
	}

	if (!glusStartup())
	{
		return -1;
	}

	//
	// Compute shader for pre-filtering.
	//

	glusLoadTextFile("../EnvironmentBRDF/shader/environmentBRDF.comp.glsl", &computeSource);

	glusBuildComputeProgramFromSource(&computeProgram, (const GLchar**)&computeSource.text);

	glusDestroyTextFile(&computeSource);

	//

	mLocation = glGetUniformLocation(computeProgram.program, "u_m");
	samplesLocation = glGetUniformLocation(computeProgram.program, "u_samples");
	binaryFractionFactorLocation = glGetUniformLocation(computeProgram.program, "u_binaryFractionFactor");

	//

	glUseProgram(computeProgram.program);

	//
	//
	//

	// Prepare texture, where the pre-filtered image is stored: Cook-Torrance
    glGenTextures(1, &textureCookTorrance);
    glBindTexture(GL_TEXTURE_2D, textureCookTorrance);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, length, length, 0, GL_RG, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // see binding = 0 in the shader
    glBindImageTexture(0, textureCookTorrance, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//
	//
	//

	// Prepare save name.

	strcpy(buffer, output);

	buffer[ouputLength] = '_';

	k = length;
	for (i = 1; i < MAX_RESOLUTION_LENGTH; i++)
	{
		buffer[ouputLength + MAX_RESOLUTION_LENGTH - i] = '0' + (k % 10);

		k /= 10;
	}

	buffer[ouputLength + MAX_RESOLUTION_LENGTH + 0] = '.';
	buffer[ouputLength + MAX_RESOLUTION_LENGTH + 1] = 'd';
	buffer[ouputLength + MAX_RESOLUTION_LENGTH + 2] = 'a';
	buffer[ouputLength + MAX_RESOLUTION_LENGTH + 3] = 't';
	buffer[ouputLength + MAX_RESOLUTION_LENGTH + 4] = 'a';
	buffer[ouputLength + MAX_RESOLUTION_LENGTH + 5] = '\0';

	//

	// Setup scan vectors buffer for compute shader.
	glGenBuffers(1, &scanVectorsSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scanVectorsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, length * length * (3 + 1) * sizeof(GLfloat), 0, GL_DYNAMIC_DRAW);
	// see binding = 1 in the shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scanVectorsSSBO);

	// Setup m and samples for compute shader.
	glUniform1ui(mLocation, exponent);
	glUniform1ui(samplesLocation, samples);
	// Results are in range [0.0 1.0] and not [0.0, 1.0[.
	glUniform1f(binaryFractionFactorLocation, 1.0f / (powf(2.0f, (GLfloat)exponent) - 1.0f));

	printf("Generating environment BRDF texture ... ");

	// Run the compute shader, which is generating the LUT.
	glDispatchCompute(length / localSize, length / localSize, 1);

	// Compute shader stores result in given texture.
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, binaryfile.binary);

	glusSaveBinaryFile(buffer, &binaryfile);

	printf("completed!\n");

	//
	// Freeing resources
	//

	glusDestroyBinaryFile(&binaryfile);

	glusDestroyProgram(&computeProgram);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (textureCookTorrance)
    {
        glDeleteTextures(1, &textureCookTorrance);

        textureCookTorrance = 0;
    }

	//
	// Shutdown OpenGL.
	//

	glusShutdown();

	return 0;
}
