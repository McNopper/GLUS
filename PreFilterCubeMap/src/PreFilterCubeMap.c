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
#define TYPE_NAMING_LENGTH		2		// without '\0'

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
    // see binding = 0 in the shader
    glActiveTexture(GL_TEXTURE0);
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
    // see binding = 0 in the shader
    glActiveTexture(GL_TEXTURE0);
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
	GLUSuint		exponent;
	GLUSuint		samples;

	GLUSint i, k, m, o, p, q, x, y, ouputLength;

	GLUSboolean isHDR = GLUS_FALSE;

	GLUStgaimage tgaOutput[2];
	GLUShdrimage hdrOutput[2];

	GLUSint	length;
	GLUSint	lengthExponent;
	GLUSint	stride;

	GLUSfloat offset, step, roughness;

	GLUSfloat startVector[3] = { 1.0f, -1.0f, -1.0f };
	GLUSfloat offsetVector[3];
	GLUSfloat normalVector[3];
	GLUSfloat* scanVectors;
	GLUSfloat* colorBufferLambert;
	GLUSfloat* colorBufferCookTorrance;

	GLUSfloat matrix[9];

	GLUStextfile computeSource;
	GLUSshaderprogram computeProgram;

	GLUSuint localSize = 16;

	GLUSuint textureLambert;
	GLUSuint textureCookTorrance;

	GLUSuint scanVectorsSSBO;

	GLUSint mLocation;
	GLUSint samplesLocation;
	GLUSint binaryFractionFactorLocation;
	GLUSint roughnessLocation;

	if (argc != 11)
	{
		printf("Usage: Panorama2CubeMap.exe [Pos X] [Neg X] [Pos Y] [Neg Y] [Pos Z] [Neg Z] [Output] [Roughness] [Samples 2^m] [Length 2^n]\n");

		return -1;
	}

	//

	output = argv[7];

	ouputLength = strlen(output);

	if (ouputLength >= MAX_FILENAME_LENGTH - (MAX_FILETYPE_LENGTH - 1) - SIDE_NAMING_LENGTH - ROUGHNESS_NAMING_LENGTH - TYPE_NAMING_LENGTH)
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

	exponent = (GLUSuint)atoi(argv[9]);

	if (exponent > 16)
	{
		printf("Error: Invalid samples value.\n");

		return -1;
	}

	samples = 1 << exponent;

	lengthExponent = (GLUSuint)atoi(argv[10]);

	if (lengthExponent > 16)
	{
		printf("Error: Invalid length value.\n");

		return -1;
	}

	length = 1 << lengthExponent;

	if (roughnessSamples < 2 || roughnessSamples >= 100)
	{
		printf("Error: Invalid roughness value.\n");

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

		//

		tgaOutput[0] = g_tgaimage[0];

		tgaOutput[0].width = length;
		tgaOutput[0].height = length;

		tgaOutput[0].data = (GLUSubyte*)malloc(length * length * stride * sizeof(GLUSubyte));

		if (!tgaOutput[0].data)
		{
			printf("failed! TGA output image could not be created.\n");

			freeTgaImages(6);

			return -1;
		}

		tgaOutput[1] = g_tgaimage[0];

		tgaOutput[1].width = length;
		tgaOutput[1].height = length;

		tgaOutput[1].data = (GLUSubyte*)malloc(length * length * stride * sizeof(GLUSubyte));

		if (!tgaOutput[1].data)
		{
			printf("failed! TGA output image could not be created.\n");

			freeTgaImages(6);

			glusDestroyTgaImage(&tgaOutput[0]);

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

		//

		hdrOutput[0] = g_hdrimage[0];

		hdrOutput[0].width = length;
		hdrOutput[0].height = length;

		hdrOutput[0].data = (GLUSfloat*)malloc(length * length * stride * sizeof(GLUSfloat));

		if (!hdrOutput[0].data)
		{
			printf("failed! HDR output image could not be created.\n");

			freeHdrImages(6);

			return -1;
		}

		hdrOutput[1] = g_hdrimage[0];

		hdrOutput[1].width = length;
		hdrOutput[1].height = length;

		hdrOutput[1].data = (GLUSfloat*)malloc(length * length * stride * sizeof(GLUSfloat));

		if (!hdrOutput[1].data)
		{
			printf("failed! HDR output image could not be created.\n");

			freeHdrImages(6);

			glusDestroyHdrImage(&hdrOutput[1]);

			return -1;
		}
	}
	else
	{
		printf("failed. Unknown file type.\n");

		return -1;
	}
	printf("completed!\n");

	// Contains the vectors to scan and generate one side of the pre-filtered cube map.
	scanVectors = (GLUSfloat*)malloc(length * length * (3 + 1) * sizeof(GLUSfloat));

	if (!scanVectors)
	{
		printf("Error: Scan scanVectors could not be created.\n");

		freeHdrImages(6);

		return -1;
	}

	// Color buffer needed to gather the pixels from the texture.
	colorBufferLambert = (GLUSfloat*)malloc(length * length * 4 * sizeof(GLUSfloat));

	if (!colorBufferLambert)
	{
		printf("Error: Color buffer could not be created.\n");

		freeHdrImages(6);

		free(scanVectors);

		return -1;
	}

	// Color buffer needed to gather the pixels from the texture.
	colorBufferCookTorrance = (GLUSfloat*)malloc(length * length * 4 * sizeof(GLUSfloat));

	if (!colorBufferCookTorrance)
	{
		printf("Error: Color buffer could not be created.\n");

		freeHdrImages(6);

		free(scanVectors);

		free(colorBufferLambert);

		return -1;
	}

	//
	// Initialize OpenGL, as it is needed for the compute shader.
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
	// Compute shader for pre-filtering.
	//

	glusLoadTextFile("../PreFilterCubeMap/shader/prefilter.comp.glsl", &computeSource);

	glusBuildComputeProgramFromSource(&computeProgram, (const GLchar**)&computeSource.text);

	glusDestroyTextFile(&computeSource);

	//

	mLocation = glGetUniformLocation(computeProgram.program, "u_m");
	samplesLocation = glGetUniformLocation(computeProgram.program, "u_samples");
	binaryFractionFactorLocation = glGetUniformLocation(computeProgram.program, "u_binaryFractionFactor");
	roughnessLocation = glGetUniformLocation(computeProgram.program, "u_roughness");

	//

	glUseProgram(computeProgram.program);

	//
	//
	//

	// Create cube maps
	if (isHDR)
	{
		createHdrCubeMap();

		freeHdrImages(6);
	}
	else
	{
		createTgaCubeMap();

		freeTgaImages(6);
	}

	// Prepare texture, where the pre-filtered image is stored: Lambert
    glGenTextures(1, &textureLambert);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureLambert);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, length, length, 0, GL_RGBA, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // see binding = 1 in the shader
    glBindImageTexture(1, textureLambert, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//

	// Prepare texture, where the pre-filtered image is stored: Cook-Torrance
    glGenTextures(1, &textureCookTorrance);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureCookTorrance);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, length, length, 0, GL_RGBA, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // see binding = 2 in the shader
    glBindImageTexture(2, textureCookTorrance, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//
	//
	//

	step = 2.0f / (GLUSfloat)length;
	offset = step * 0.5f;

	// Prepare save name.

	strcpy(buffer, output);
	buffer[ouputLength + 0] = '_';
	buffer[ouputLength + 4] = '_';

	buffer[ouputLength + 6] = '_';

	buffer[ouputLength + 9] = '_';

	for (i = ouputLength + SIDE_NAMING_LENGTH + ROUGHNESS_NAMING_LENGTH + TYPE_NAMING_LENGTH; i < ouputLength + SIDE_NAMING_LENGTH + ROUGHNESS_NAMING_LENGTH + TYPE_NAMING_LENGTH + MAX_FILETYPE_LENGTH; i++)
	{
		buffer[i] = fileType[i - (ouputLength + SIDE_NAMING_LENGTH + ROUGHNESS_NAMING_LENGTH + TYPE_NAMING_LENGTH)];
	}

	//

	// Setup scan vectors buffer for compute shader.
	glGenBuffers(1, &scanVectorsSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scanVectorsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, length * length * (3 + 1) * sizeof(GLfloat), 0, GL_DYNAMIC_DRAW);
	// see binding = 3 in the shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, scanVectorsSSBO);

	// Setup m and samples for compute shader.
	glUniform1ui(mLocation, exponent);
	glUniform1ui(samplesLocation, samples);
	// Results are in range [0.0 1.0] and not [0.0, 1.0[.
	glUniform1f(binaryFractionFactorLocation, 1.0f / (powf(2.0f, (GLfloat)exponent) - 1.0f));

	printf("Generating pre filtered cube maps ...\n");
	for (i = 0; i < 6; i++)
	{
		printf("Side: %d\n", i);

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

		// Generate scan vectors
		for (k = 0; k < length; k++)
		{
			for (m = 0; m < length; m++)
			{
				offsetVector[0] = 0.0f;
				offsetVector[1] = offset + step * (GLUSfloat)k;
				offsetVector[2] = offset + step * (GLUSfloat)m;

				glusVector3AddVector3f(normalVector, startVector, offsetVector);
				glusVector3Normalizef(normalVector);

				glusMatrix3x3MultiplyVector3f(&scanVectors[k * length * (3 + 1) + m * (3 + 1)], matrix, normalVector);
			}
		}

		// Upload scan vectors for each side.
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, length * length * (3 + 1) * sizeof(GLfloat), scanVectors);

		// For all roughness levels
		for (k = 0; k < roughnessSamples; k++)
		{
			// Calculate roughness ...
			roughness = (GLUSfloat)k * 1.0f / (GLUSfloat)(roughnessSamples - 1);

			printf("Roughness: %f\n", roughness);

			// ... and set it up for compute shader.
			glUniform1f(roughnessLocation, roughness);

			// Run the compute shader, which is doing the pre-filtering.
			glDispatchCompute(length / localSize, length / localSize, 1);

		    glActiveTexture(GL_TEXTURE1);
		    glBindTexture(GL_TEXTURE_2D, textureLambert);

		    if (roughness == 0.0f)
		    {
		    	// Compute shader stores result in given texture.
		    	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, colorBufferLambert);
		    }

		    glActiveTexture(GL_TEXTURE2);
		    glBindTexture(GL_TEXTURE_2D, textureCookTorrance);
			// Compute shader stores result in given texture.
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, colorBufferCookTorrance);

			// Resolve
			for (p = 0; p < length; p++)
			{
				for (q = 0; q < length; q++)
				{
					// Some of the textures need to be stored flipped and mirrored down.
					switch (i)
					{
						case 0:
						case 1:
						case 4:
						case 5:
							// Positive X
							// Negative X
							// Positive Z
							// Negative Z

							x = length - 1 - q;
							y = length - 1 - p;

						break;
						case 2:
						case 3:
							// Positive Y
							// Negative Y

							x = q;
							y = p;

						break;
					}

					for (o = 0; o < stride; o++)
					{
						if (isHDR)
						{
							if (roughness == 0.0f)
							{
								hdrOutput[0].data[p * length * stride + q * stride + o] = colorBufferLambert[y * length * 4 + x * 4 + o];
							}
							hdrOutput[1].data[p * length * stride + q * stride + o] = colorBufferCookTorrance[y * length * 4 + x * 4 + o];
						}
						else
						{
							if (roughness == 0.0f)
							{
								tgaOutput[0].data[p * length * stride + q * stride + o] = (GLUSubyte)glusClampf(colorBufferLambert[y * length * 4 + x * 4 + o] * 255.0f, 0.0f, 255.0f);
							}
							tgaOutput[1].data[p * length * stride + q * stride + o] = (GLUSubyte)glusClampf(colorBufferCookTorrance[y * length * 4 + x * 4 + o] * 255.0f, 0.0f, 255.0f);
						}
					}
				}
			}

			// Construct save name depending on roughness level.
			buffer[ouputLength + 7] = '0' + (k / 10);
			buffer[ouputLength + 8] = '0' + (k % 10);

			if (isHDR)
			{
				if (roughness == 0.0f)
				{
					buffer[ouputLength + 10] = 'd';
					glusSaveHdrImage(buffer, &hdrOutput[0]);
				}
				buffer[ouputLength + 10] = 's';
				glusSaveHdrImage(buffer, &hdrOutput[1]);
			}
			else if (roughness == 0.0f)
			{
				if (roughness == 0.0f)
				{
					buffer[ouputLength + 10] = 'd';
					glusSaveTgaImage(buffer, &tgaOutput[0]);
				}
				buffer[ouputLength + 10] = 's';
				glusSaveTgaImage(buffer, &tgaOutput[1]);
			}
		}
	}
	printf("completed!\n");

	//
	// Freeing resources
	//

	free(scanVectors);

	free(colorBufferLambert);

	free(colorBufferCookTorrance);

	glusDestroyProgram(&computeProgram);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    if (g_cubemap)
    {
        glDeleteTextures(1, &g_cubemap);

        g_cubemap = 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    if (textureLambert)
    {
        glDeleteTextures(1, &textureLambert);

        textureLambert = 0;
    }

    if (textureCookTorrance)
    {
        glDeleteTextures(1, &textureCookTorrance);

        textureCookTorrance = 0;
    }


    if (isHDR)
    {
    	glusDestroyHdrImage(&hdrOutput[0]);
    	glusDestroyHdrImage(&hdrOutput[1]);
    }
    else
    {
    	glusDestroyTgaImage(&tgaOutput[0]);
    	glusDestroyTgaImage(&tgaOutput[1]);
    }

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	if (scanVectorsSSBO)
	{
		glDeleteBuffers(1, &scanVectorsSSBO);

		scanVectorsSSBO = 0;
	}

	//
	// Shutdown OpenGL.
	//

	glusShutdown();

	return 0;
}
