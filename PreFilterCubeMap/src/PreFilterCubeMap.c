/*
 * PreFilterCubeMap - Pre-Filters a cube map for the first sum of the IBL approximation as seen in
 * http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf page 5.
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
#define SIDE_NAMING_LENGTH		6		// without '\0'
#define ROUGHNESS_NAMING_LENGTH	3		// without '\0'

static GLUStgaimage g_tgaimage[6];
static GLUShdrimage g_hdrimage[6];

static GLUSuint g_cubemap;


static GLUSvoid freeTgaImages(GLUSint currentElement)
{
	GLUSint i;

	for (i = 0; i < currentElement; i++)
	{
		glusDestroyTgaImage(&g_tgaimage[i]);
	}
}

static GLUSvoid freeHdrImages(GLUSint currentElement)
{
	GLUSint i;

	for (i = 0; i < currentElement; i++)
	{
		glusDestroyHdrImage(&g_hdrimage[i]);
	}
}

static GLUSvoid createTgaCubeMap()
{
	GLUSint i;

    glGenTextures(1, &g_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_cubemap);

    for (i = 0; i < 6; i++)
    {
    	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, g_tgaimage[i].format, g_tgaimage[i].width, g_tgaimage[i].height, 0, g_tgaimage[i].format, GL_UNSIGNED_BYTE, g_tgaimage[i].data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

static GLUSvoid createHdrCubeMap()
{
	GLUSint i;

    glGenTextures(1, &g_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_cubemap);

    for (i = 0; i < 6; i++)
    {
    	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, g_hdrimage[i].width, g_hdrimage[i].height, 0, GL_RGB, GL_FLOAT, g_hdrimage[i].data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

int main(int argc, char* argv[])
{
	GLUSchar*	output;
	GLUSchar* extension;
	GLUSchar fileType[MAX_FILETYPE_LENGTH];
	GLUSchar buffer[MAX_FILENAME_LENGTH];

	GLUSint		roughnessSamples;
	GLUSint		samples;

	GLUSint i, k, m, ouputLength;

	GLUSboolean isHDR = GLUS_FALSE;

	GLUStgaimage tgaOutput;
	GLUShdrimage hdrOutput;

	GLUSint	length;
	GLUSint	stride;

	GLUSfloat offset, step, roughness;

	GLUSfloat startVector[3] = { 1.0f, -1.0f, -1.0f };
	GLUSfloat offsetVector[3];
	GLUSfloat normalVector[3];
	GLUSfloat* scanVectors;

	GLUSfloat matrix[9];

	GLUStextfile computeSource;
	GLUSshaderprogram g_computeProgram;

	GLUSuint localSize = 16;


	if (argc != 10)
	{
		printf("Usage: Panorama2CubeMap.exe [Pos X] [Neg X] [Pos Y] [Neg Y] [Pos Z] [Neg Z] [Output] [Roughness] [Samples]\n");

		return -1;
	}

	//

	output = argv[7];

	ouputLength = strlen(output);

	if (ouputLength >= MAX_FILENAME_LENGTH - SIDE_NAMING_LENGTH - ROUGHNESS_NAMING_LENGTH)
	{
		printf("Error: Output filename too long.\n");

		return -1;
	}

	roughnessSamples = atoi(argv[8]);

	if (roughnessSamples < 2 || roughnessSamples >= 100)
	{
		printf("Error: Invalid roughness value.\n");

		return -1;
	}

	samples = atoi(argv[9]);

	if (samples < 1 || samples > 65536)
	{
		printf("Error: Invalid samples calue.\n");

		return -1;
	}

	//

	extension = strrchr(argv[1], '.');

	if (extension == 0)
	{
		printf("Error: No file type found.\n");

		return -1;
	}

	if (strlen(extension) != MAX_FILETYPE_LENGTH - 1)
	{
		printf("Error: Invalid file type.\n");

		return -1;
	}

	// Copy includes NULL terminating character.
	for (i = 0; i < MAX_FILETYPE_LENGTH ; i++)
	{
		fileType[i] = tolower(extension[i]);
	}

	stride = 1;

	printf("Loading texture cube maps ... ");
	if (strcmp(fileType, ".tga") == 0)
	{
		//

		for (i = 0; i < 6; i++)
		{
			if (!glusLoadTgaImage(argv[1 + i], &g_tgaimage[i]))
			{
				printf("failed! TGA image could not be loaded.\n");

				freeTgaImages(i);

				return -1;
			}

			if (i > 0)
			{
				if (g_tgaimage[0].width != g_tgaimage[i].width || g_tgaimage[0].height != g_tgaimage[i].height)
				{
					printf("failed! TGA images do have different dimension.\n");

					freeTgaImages(i + 1);

					return -1;
				}
			}
			else
			{
				if (g_tgaimage[0].width != g_tgaimage[i].height)
				{
					printf("failed! TGA images do have different dimension.\n");

					freeTgaImages(1);

					return -1;
				}
			}
		}

		if (g_tgaimage[0].format == GLUS_RGB)
		{
			stride = 3;
		}
		else if (g_tgaimage[0].format == GLUS_RGBA)
		{
			stride = 4;
		}

		length = g_tgaimage[0].width;

		//

		tgaOutput = g_tgaimage[0];

		tgaOutput.data = (GLUSubyte*)malloc(length * length * stride * sizeof(GLUSubyte));

		if (!tgaOutput.data)
		{
			printf("failed! TGA output image could not be created.\n");

			freeTgaImages(6);

			return -1;
		}
	}
	else if (strcmp(fileType, ".hdr") == 0)
	{
		isHDR = GLUS_TRUE;

		for (i = 0; i < 6; i++)
		{
			if (!glusLoadHdrImage(argv[1 + i], &g_hdrimage[i]))
			{
				printf("failed! HDR image could not be loaded.\n");

				freeHdrImages(i);

				return -1;
			}

			if (i > 0)
			{
				if (g_hdrimage[0].width != g_hdrimage[i].width || g_hdrimage[0].height != g_hdrimage[i].height)
				{
					printf("failed! HDR images do have different dimension.\n");

					freeHdrImages(i + 1);

					return -1;
				}
			}
			else
			{
				if (g_hdrimage[0].width != g_hdrimage[i].height)
				{
					printf("failed! HDR images do have different dimension.\n");

					freeHdrImages(1);

					return -1;
				}
			}
		}

		stride = 3;

		length = g_hdrimage[0].width;

		//

		hdrOutput = g_hdrimage[0];

		hdrOutput.data = (GLUSfloat*)malloc(length * length * stride * sizeof(GLUSfloat));

		if (!hdrOutput.data)
		{
			printf("failed! HDR output image could not be created.\n");

			freeHdrImages(6);

			return -1;
		}
	}
	else
	{
		printf("failed. Unknown file type.\n");

		return -1;
	}
	printf("completed!\n");

	//

	scanVectors = (GLUSfloat*)malloc(length * length * 3 * sizeof(GLUSfloat));

	if (!scanVectors)
	{
		printf("failed! Scan scanVectors could not be created.\n");

		freeHdrImages(6);

		return -1;
	}

	//
	//
	//

	glusPrepareContext(4, 3, GLUS_FORWARD_COMPATIBLE_BIT);

	if (!glusCreateWindow("GLUS Example Window", 512, 512, 0, 0, GLUS_FALSE))
	{
		printf("Could not create window!\n");
		return -1;
	}

	if (!glusStartup())
	{
		return -1;
	}

	//

	glusLoadTextFile("../PreFilterCubeMap/shader/prefilter.comp.glsl", &computeSource);

	glusBuildComputeProgramFromSource(&g_computeProgram, (const GLchar**)&computeSource.text);

	glusDestroyTextFile(&computeSource);

	//
	//
	//

	// Create cube maps
	if (isHDR)
	{
		createHdrCubeMap();

		freeHdrImages(6);

		// TODO Setup output HDR buffer for compute shader.
	}
	else
	{
		createTgaCubeMap();

		freeTgaImages(6);

		// TODO Setup output TGA buffer for compute shader.
	}

	//
	//
	//

	step = 2.0f / (GLUSfloat)length;
	offset = step * 0.5f;

	strcpy(buffer, output);
	buffer[ouputLength + 0] = '_';
	buffer[ouputLength + 4] = '_';

	buffer[ouputLength + 6] = '_';

	for (i = ouputLength + SIDE_NAMING_LENGTH + ROUGHNESS_NAMING_LENGTH; i < ouputLength + SIDE_NAMING_LENGTH + ROUGHNESS_NAMING_LENGTH + MAX_FILETYPE_LENGTH; i++)
	{
		buffer[i] = fileType[i - (ouputLength + SIDE_NAMING_LENGTH + ROUGHNESS_NAMING_LENGTH)];
	}

	// TODO Setup scan ray buffer for compute shader.
	// TODO Setup samples for compute shader.
	// TODO Setup format and type for compute shader.

	printf("Generating pre filtered cube maps ... ");
	for (i = 0; i < 6; i++)
	{
		switch (i)
		{
			case 0:
				// Positive X

				glusMatrix3x3Identityf(matrix);

				buffer[ouputLength + 1] = 'P';
				buffer[ouputLength + 2] = 'O';
				buffer[ouputLength + 3] = 'S';

				buffer[ouputLength + 5] = 'X';

			break;
			case 1:
				// Negative X

				glusMatrix3x3Identityf(matrix);
				glusMatrix3x3RotateRyf(matrix, 180.0f);

				buffer[ouputLength + 1] = 'N';
				buffer[ouputLength + 2] = 'E';
				buffer[ouputLength + 3] = 'G';

				buffer[ouputLength + 5] = 'X';

			break;
			case 2:
				// Positive Y

				glusMatrix3x3Identityf(matrix);
				glusMatrix3x3RotateRxf(matrix, 90.0f);
				glusMatrix3x3RotateRyf(matrix, 90.0f);

				buffer[ouputLength + 1] = 'P';
				buffer[ouputLength + 2] = 'O';
				buffer[ouputLength + 3] = 'S';

				buffer[ouputLength + 5] = 'Y';

			break;
			case 3:
				// Negative Y

				glusMatrix3x3Identityf(matrix);
				glusMatrix3x3RotateRxf(matrix, -90.0f);
				glusMatrix3x3RotateRyf(matrix, 90.0f);

				buffer[ouputLength + 1] = 'N';
				buffer[ouputLength + 2] = 'E';
				buffer[ouputLength + 3] = 'G';

				buffer[ouputLength + 5] = 'Y';

			break;
			case 4:
				// Positive Z

				glusMatrix3x3Identityf(matrix);
				glusMatrix3x3RotateRyf(matrix, -90.0f);

				buffer[ouputLength + 1] = 'P';
				buffer[ouputLength + 2] = 'O';
				buffer[ouputLength + 3] = 'S';

				buffer[ouputLength + 5] = 'Z';

			break;
			case 5:
				// Negative Z

				glusMatrix3x3Identityf(matrix);
				glusMatrix3x3RotateRyf(matrix, 90.0f);

				buffer[ouputLength + 1] = 'N';
				buffer[ouputLength + 2] = 'E';
				buffer[ouputLength + 3] = 'G';

				buffer[ouputLength + 5] = 'Z';

			break;
		}

		// Generate scan scan vectors
		for (k = 0; k < length; k++)
		{
			for (m = 0; m < length; m++)
			{
				offsetVector[0] = 0.0f;
				offsetVector[1] = offset + step * (GLUSfloat)k;
				offsetVector[2] = offset + step * (GLUSfloat)m;

				glusVector3AddVector3f(normalVector, startVector, offsetVector);
				glusVector3Normalizef(normalVector);

				glusMatrix3x3MultiplyVector3f(&scanVectors[k * length * 3 + m * 3], matrix, normalVector);
			}
		}

		// TODO Setup i for compute shader.

		// For all roughness levels
		for (k = 0; k < roughnessSamples; k++)
		{
			roughness = (GLUSfloat)k * 1.0f / (GLUSfloat)(roughnessSamples - 1);

			glUseProgram(g_computeProgram.program);

			// TODO Setup roughness for compute shader.

			glDispatchCompute(length / localSize, length / localSize, 1);

			glUseProgram(0);

			// Construct save name depending on roughness level.
			buffer[ouputLength + 7] = '0' + (k / 10);
			buffer[ouputLength + 8] = '0' + (k % 10);

			if (isHDR)
			{
				glusSaveHdrImage(buffer, &hdrOutput);
			}
			else
			{
				glusSaveTgaImage(buffer, &tgaOutput);
			}
		}
	}
	printf("completed!\n");

	//
	//
	//

	glusDestroyProgram(&g_computeProgram);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    if (g_cubemap)
    {
        glDeleteTextures(1, &g_cubemap);

        g_cubemap = 0;
    }

    if (isHDR)
    {
    	glusDestroyHdrImage(&hdrOutput);
    }
    else
    {
    	glusDestroyTgaImage(&tgaOutput);
    }

	//
	//
	//

	glusShutdown();

	return 0;
}
