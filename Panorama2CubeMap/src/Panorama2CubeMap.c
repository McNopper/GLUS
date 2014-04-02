/*
 * Panorama2CubeMap - Converts a panorma image to six cube map textures.
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

#define MAX_FILENAME_LENGTH 1024	// with '\0'
#define MAX_FILETYPE_LENGTH 5		// with '\0'
#define SIDE_NAMING_LENGTH	6		// without '\0'

int main(int argc, char* argv[])
{
	GLUStgaimage tgaimage;
	GLUShdrimage hdrimage;

	GLUStgaimage tgaCubeMap[6];
	GLUShdrimage hdrCubeMap[6];

	GLUSchar buffer[MAX_FILENAME_LENGTH];
	GLUSchar* extension;
	GLUSchar fileType[MAX_FILETYPE_LENGTH];

	GLUSint i, k, m, o, x, y, length, stride, appendIndex;
	GLUSuint lengthExponent;

	GLUSboolean isHDR = GLUS_FALSE;

	GLUSfloat startVector[3] = { 1.0f, -1.0f, -1.0f };
	GLUSfloat offsetVector[3];
	GLUSfloat normalVector[3];
	GLUSfloat scanVector[3];

	GLUSfloat matrix[9];

	GLUSfloat offset, step;

	GLUSfloat rgb[3];
	GLUSubyte rgba[4];

	GLUSfloat st[2];

	//

	if (argc != 3)
	{
		printf("Usage: Panorama2CubeMap.exe [Panorama Filename] [Length 2^m]\n");

		return -1;
	}

	//

	lengthExponent = (GLUSuint)atoi(argv[2]);

	if (lengthExponent > 16)
	{
		printf("Error: Invalid side length.\n");

		return -1;
	}

	length = 1 << lengthExponent;

	//

	if (strlen(argv[1]) >= MAX_FILENAME_LENGTH - SIDE_NAMING_LENGTH)
	{
		printf("Error: Filename too long.\n");

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

	//

	printf("Loading '%s' ... ", argv[1]);
	if (strcmp(fileType, ".tga") == 0)
	{
		if (!glusLoadTgaImage(argv[1], &tgaimage))
		{
			printf("failed! TGA image could not be loaded.\n");

			return -1;
		}

		stride = 1;

		if (tgaimage.format == GLUS_RGB)
		{
			stride = 3;
		}
		else if (tgaimage.format == GLUS_RGBA)
		{
			stride = 4;
		}

		for (i = 0; i < 6; i++)
		{
			tgaCubeMap[i].width = length;
			tgaCubeMap[i].height = length;
			tgaCubeMap[i].depth = 1;
			tgaCubeMap[i].format = tgaimage.format;

			tgaCubeMap[i].data = (GLUSubyte*)malloc(length * length * stride * sizeof(GLUSubyte));

			if (!tgaCubeMap[i].data)
			{
				printf("failed! Out of memory.\n");

				for (k = i - 1; k >= 0; k--)
				{
					glusDestroyTgaImage(&tgaCubeMap[k]);
				}

				glusDestroyTgaImage(&tgaimage);

				return -1;
			}
		}
	}
	else if (strcmp(fileType, ".hdr") == 0)
	{
		if (!glusLoadHdrImage(argv[1], &hdrimage))
		{
			printf("failed! HDR image could not be loaded.\n");

			return -1;
		}

		isHDR = GLUS_TRUE;

		stride = 3;

		for (i = 0; i < 6; i++)
		{
			hdrCubeMap[i].width = length;
			hdrCubeMap[i].height = length;
			hdrCubeMap[i].depth = 1;
			hdrCubeMap[i].format = GLUS_RGB;

			hdrCubeMap[i].data = (GLUSfloat*)malloc(length * length * stride * sizeof(GLUSfloat));

			if (!hdrCubeMap[i].data)
			{
				printf("failed! Out of memory.\n");

				for (k = i - 1; k >= 0; k--)
				{
					glusDestroyHdrImage(&hdrCubeMap[k]);
				}

				glusDestroyHdrImage(&hdrimage);

				return -1;
			}
		}
	}
	else
	{
		printf("failed. Unknown file type.\n");

		return -1;
	}
	printf("completed!\n");

	//

	step = 2.0f / (GLUSfloat)length;
	offset = step * 0.5f;

	//

	printf("Generating cube maps ... ");
	for (k = 0; k < length; k++)
	{
		for (m = 0; m < length; m++)
		{
			offsetVector[0] = 0.0f;
			offsetVector[1] = offset + step * (GLUSfloat)k;
			offsetVector[2] = offset + step * (GLUSfloat)m;

			glusVector3AddVector3f(normalVector, startVector, offsetVector);
			glusVector3Normalizef(normalVector);

			for (i = 0; i < 6; i++)
			{
				switch (i)
				{
					case 0:
						// Positive X

						glusVector3Copyf(scanVector, normalVector);

						x = length - 1 - m;
						y = length - 1 - k;

					break;
					case 1:
						// Negative X

						glusMatrix3x3Identityf(matrix);
						glusMatrix3x3RotateRyf(matrix, 180.0f);

						glusMatrix3x3MultiplyVector3f(scanVector, matrix, normalVector);

						x = length - 1 - m;
						y = length - 1 - k;

					break;
					case 2:
						// Positive Y

						glusMatrix3x3Identityf(matrix);
						glusMatrix3x3RotateRxf(matrix, 90.0f);
						glusMatrix3x3RotateRyf(matrix, 90.0f);

						glusMatrix3x3MultiplyVector3f(scanVector, matrix, normalVector);

						x = m;
						y = k;

					break;
					case 3:
						// Negative Y

						glusMatrix3x3Identityf(matrix);
						glusMatrix3x3RotateRxf(matrix, -90.0f);
						glusMatrix3x3RotateRyf(matrix, 90.0f);

						glusMatrix3x3MultiplyVector3f(scanVector, matrix, normalVector);

						x = m;
						y = k;

					break;
					case 4:
						// Positive Z

						glusMatrix3x3Identityf(matrix);
						glusMatrix3x3RotateRyf(matrix, -90.0f);

						glusMatrix3x3MultiplyVector3f(scanVector, matrix, normalVector);

						x = length - 1 - m;
						y = length - 1 - k;

					break;
					case 5:
						// Negative Z

						glusMatrix3x3Identityf(matrix);
						glusMatrix3x3RotateRyf(matrix, 90.0f);

						glusMatrix3x3MultiplyVector3f(scanVector, matrix, normalVector);

						x = length - 1 - m;
						y = length - 1 - k;

					break;
				}

				st[0] = 0.5f + 0.5f * atan2f(scanVector[0], -scanVector[2]) / GLUS_PI;
				st[1] = 1.0f - acosf(scanVector[1]) / GLUS_PI;

				if (isHDR)
				{
					glusTexImage2DSampleHdrImage(rgb, &hdrimage, st);

					for (o = 0; o < stride; o++)
					{
						hdrCubeMap[i].data[y * length * stride + x * stride + o] = rgb[o];
					}
				}
				else
				{
					glusTexImage2DSampleTgaImage(rgba, &tgaimage, st);

					for (o = 0; o < stride; o++)
					{
						tgaCubeMap[i].data[y * length * stride + x * stride + o] = rgba[o];
					}
				}
			}
		}
	}

	//

	appendIndex = (extension - argv[1]);

	strncpy(buffer, argv[1], appendIndex);

	buffer[appendIndex + 0] = '_';
	buffer[appendIndex + 4] = '_';

	for (i = SIDE_NAMING_LENGTH; i < SIDE_NAMING_LENGTH + MAX_FILETYPE_LENGTH; i++)
	{
		// File type contains the NULL terminating character.
		buffer[appendIndex + i] = fileType[i - SIDE_NAMING_LENGTH];
	}

	for (i = 0; i < 6; i++)
	{
		switch (i)
		{
			case 0:

				buffer[appendIndex + 1] = 'P';
				buffer[appendIndex + 2] = 'O';
				buffer[appendIndex + 3] = 'S';

				buffer[appendIndex + 5] = 'X';

			break;
			case 1:

				buffer[appendIndex + 1] = 'N';
				buffer[appendIndex + 2] = 'E';
				buffer[appendIndex + 3] = 'G';

				buffer[appendIndex + 5] = 'X';

			break;
			case 2:

				buffer[appendIndex + 1] = 'P';
				buffer[appendIndex + 2] = 'O';
				buffer[appendIndex + 3] = 'S';

				buffer[appendIndex + 5] = 'Y';

			break;
			case 3:

				buffer[appendIndex + 1] = 'N';
				buffer[appendIndex + 2] = 'E';
				buffer[appendIndex + 3] = 'G';

				buffer[appendIndex + 5] = 'Y';

			break;
			case 4:

				buffer[appendIndex + 1] = 'P';
				buffer[appendIndex + 2] = 'O';
				buffer[appendIndex + 3] = 'S';

				buffer[appendIndex + 5] = 'Z';

			break;
			case 5:

				buffer[appendIndex + 1] = 'N';
				buffer[appendIndex + 2] = 'E';
				buffer[appendIndex + 3] = 'G';

				buffer[appendIndex + 5] = 'Z';

			break;
		}

		if (isHDR)
		{
			glusSaveHdrImage(buffer, &hdrCubeMap[i]);

			glusDestroyHdrImage(&hdrCubeMap[i]);
		}
		else
		{
			glusSaveTgaImage(buffer, &tgaCubeMap[i]);

			glusDestroyTgaImage(&tgaCubeMap[i]);
		}
	}

	if (isHDR)
	{
		glusDestroyHdrImage(&hdrimage);
	}
	else
	{
		glusDestroyTgaImage(&tgaimage);
	}
	printf("completed!\n");

	return 0;
}
