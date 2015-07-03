/*
 * TGA2Header - TGA image to header file generator. Copyright (C) 2013 Norbert Nopper
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

int main(int argc, char* argv[])
{
	GLUStgaimage tgaimage;

	FILE* file;

	GLUSint x, y, i, k, source_bpp, target_stride, target_bpp;

	GLUSchar* filename;

	GLUSchar* target_internal_format_str;
	GLUSchar* target_format_str;
	GLUSchar* target_type_str;

	GLUSchar uppercaseFilename[256];

	GLUSubyte channel[4], high, low, conversion;

	//

	if (argc != 4)
	{
		printf("Usage: TGA2Header.exe [TGA Filename] [Header Filename] [Format]\n");
		printf("   Formats:\n");
		printf("      R8\n");
		printf("      RG88\n");
		printf("      RGB565\n");
		printf("      BGR565\n");
		printf("      RGB888\n");
		printf("      BGR888\n");
		printf("      RGBA8888\n");
		printf("      BGRA8888\n");

		return -1;
	}

	//

	printf("Loading '%s' ... ", argv[1]);
	if (!glusImageLoadTga(argv[1], &tgaimage))
	{
		printf("failed! TGA image could not be loaded.\n");

		return -1;
	}
	printf("completed!\n");

	//

	if (tgaimage.depth > 1)
	{
		printf("failed! Only 2D images are supported.\n");

		glusImageDestroyTga(&tgaimage);

		return -1;
	}

	//

	source_bpp = 1;

	if (tgaimage.format == GLUS_RGB)
	{
		source_bpp = 3;
	}
	else if (tgaimage.format == GLUS_RGBA)
	{
		source_bpp = 4;
	}

	//

	if (strcmp(argv[3], "R8") == 0)
	{
		target_bpp = 1;
		target_internal_format_str = "GL_RED";
		target_format_str = "GL_RED";
		target_type_str = "GL_UNSIGNED_BYTE";

		conversion = 0;
	}
	else if (strcmp(argv[3], "RG88") == 0)
	{
		target_bpp = 2;
		target_internal_format_str = "GL_RG";
		target_format_str = "GL_RG";
		target_type_str = "GL_UNSIGNED_BYTE";

		conversion = 1;
	}
	else if (strcmp(argv[3], "RGB565") == 0)
	{
		target_bpp = 2;
		target_internal_format_str = "GL_RGB";
		target_format_str = "GL_RGB";
		target_type_str = "  GL_UNSIGNED_SHORT_5_6_5";

		conversion = 2;
	}
	else if (strcmp(argv[3], "BGR565") == 0)
	{
		target_bpp = 2;
		target_internal_format_str = "GL_RGB";
		target_format_str = "GL_BGR";
		target_type_str = "  GL_UNSIGNED_SHORT_5_6_5";

		conversion = 3;
	}
	else if (strcmp(argv[3], "RGB888") == 0)
	{
		target_bpp = 3;
		target_internal_format_str = "GL_RGB";
		target_format_str = "GL_RGB";
		target_type_str = "GL_UNSIGNED_BYTE";

		conversion = 4;
	}
	else if (strcmp(argv[3], "BGR888") == 0)
	{
		target_bpp = 3;
		target_internal_format_str = "GL_RGB";
		target_format_str = "GL_BGR";
		target_type_str = "GL_UNSIGNED_BYTE";

		conversion = 5;
	}
	else if (strcmp(argv[3], "RGBA8888") == 0)
	{
		target_bpp = 4;
		target_internal_format_str = "GL_RGBA";
		target_format_str = "GL_RGBA";
		target_type_str = "GL_UNSIGNED_BYTE";

		conversion = 6;
	}
	else if (strcmp(argv[3], "BGRA8888") == 0)
	{
		target_bpp = 4;
		target_internal_format_str = "GL_RGBA";
		target_format_str = "GL_BGRA";
		target_type_str = "GL_UNSIGNED_BYTE";

		conversion = 7;
	}
	else
	{
		printf("failed! Unknown format: %s.\n", argv[3]);

		glusImageDestroyTga(&tgaimage);

		return -1;
	}
	target_stride = tgaimage.width * target_bpp;

	//

	printf("Saving '%s' ... ", argv[2]);

	file = fopen(argv[2], "w");

	if (!file)
	{
		printf("failed! Could not create/open header file.\n");

		glusImageDestroyTga(&tgaimage);

		return -1;
	}

	filename = strtok(argv[2], ".");

	if (!filename || strlen(filename) >= 255)
	{
		printf("failed! Invalid filename.\n");

		fclose(file);

		glusImageDestroyTga(&tgaimage);

		return -1;
	}

	for (i = 0; i < strlen(filename); i++)
	{
		uppercaseFilename[i] = toupper(filename[i]);
	}
	uppercaseFilename[i] = '\0';

	//

	fprintf(file, "#ifndef %s_TGA_HEADER\n", uppercaseFilename);
	fprintf(file, "#define %s_TGA_HEADER\n\n", uppercaseFilename);

	fprintf(file, "const GLint %s_width = %d;\n\n", filename, tgaimage.width);

	fprintf(file, "const GLint %s_height = %d;\n\n", filename, tgaimage.height);

	fprintf(file, "const GLint %s_stride = %d;\n\n", filename, target_stride);

	fprintf(file, "const GLenum %s_internal_format = %s;\n\n", filename, target_internal_format_str);

	fprintf(file, "const GLenum %s_format = %s;\n\n", filename, target_format_str);

	fprintf(file, "const GLenum %s_type = %s;\n\n", filename, target_type_str);

	fprintf(file, "const GLint %s_bpp = %d;\n\n", filename, target_bpp);

	fprintf(file, "const GLubyte %s_pixels[] = {\n", filename);

	for (y = 0; y < tgaimage.height; y++)
	{
		for (x = 0; x < tgaimage.width; x++)
		{
			i = x + y * tgaimage.width;

			for (k = 0; k < 4; k++)
			{
				if (k < source_bpp)
				{
					channel[k] = tgaimage.data[i * source_bpp + k];
				}
				else if (k != 3)
				{
					channel[k] = 0;
				}
				else
				{
					channel[k] = 255;
				}
			}

			if (conversion == 0)
			{
				// R8

				fprintf(file, "%d", channel[0]);
			}
			else if (conversion == 1)
			{
				// RG88

				fprintf(file, "%d,%d", channel[0], channel[1]);
			}
			else if (conversion == 2)
			{
				// RGB565

				channel[0] = (GLUSubyte)((GLUSfloat)channel[0] / 255.0f * 31.0f);
				channel[1] = (GLUSubyte)((GLUSfloat)channel[1] / 255.0f * 63.0f);
				channel[2] = (GLUSubyte)((GLUSfloat)channel[2] / 255.0f * 31.0f);

				low = ((channel[1] << 5) | (channel[2])) & 0xFF;
				high = ((channel[0] << 3) | (channel[1] >> 3)) & 0xFF;

				fprintf(file, "%d,%d", low, high);
			}
			else if (conversion == 3)
			{
				// BGR565

				channel[0] = (GLUSubyte)((GLUSfloat)channel[0] / 255.0f * 31.0f);
				channel[1] = (GLUSubyte)((GLUSfloat)channel[1] / 255.0f * 63.0f);
				channel[2] = (GLUSubyte)((GLUSfloat)channel[2] / 255.0f * 31.0f);

				low = ((channel[1] << 5) | (channel[0])) & 0xFF;
				high = ((channel[2] << 3) | (channel[1] >> 3)) & 0xFF;

				fprintf(file, "%d,%d", low, high);
			}
			else if (conversion == 4)
			{
				// RGB888

				fprintf(file, "%d,%d,%d", channel[0], channel[1], channel[2]);
			}
			else if (conversion == 5)
			{
				// BGR888

				fprintf(file, "%d,%d,%d", channel[2], channel[1], channel[0]);
			}
			else if (conversion == 6)
			{
				// RGBA8888

				fprintf(file, "%d,%d,%d,%d", channel[0], channel[1], channel[2], channel[3]);
			}
			else if (conversion == 7)
			{
				// BGRA8888

				fprintf(file, "%d,%d,%d,%d", channel[1], channel[2], channel[0], channel[3]);
			}

			if (y != tgaimage.height - 1 || x != tgaimage.width - 1)
			{
				fprintf(file, ",");
			}
			fprintf(file, "\n");
		}
	}
	fprintf(file, "};\n\n");

	fprintf(file, "#endif // %s_TGA_HEADER\n", uppercaseFilename);

	//

	fclose(file);

	printf("completed!\n");

	//

	glusImageDestroyTga(&tgaimage);

	return 0;
}
