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

	GLUSint i, k, stride;

	GLUSchar* format;

	GLUSchar* filename;

	GLUSchar uppercaseFilename[256];

	//

	if (argc != 3)
	{
		printf("Usage: TGA2Header.exe [TGA Filename] [Header Filename]\n");

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

	stride = 1;
	format = "GL_LUMINANCE";

	if (tgaimage.format == GLUS_RGB)
	{
		stride = 3;

		format = "GL_RGB";
	}
	else if (tgaimage.format == GLUS_RGBA)
	{
		stride = 4;

		format = "GL_RGBA";
	}

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

	fprintf(file, "static GLint %s_width = %d;\n", filename, tgaimage.width);
	fprintf(file, "static GLint %s_height = %d;\n\n", filename, tgaimage.height);

	fprintf(file, "static GLenum %s_format = %s;\n\n", filename, format);

	fprintf(file, "static GLubyte %s_pixels[] = {\n", filename);

	for (i = 0; i < tgaimage.width * tgaimage.height; i++)
	{
		for (k = 0; k < stride; k++)
		{
			fprintf(file, "%d", tgaimage.data[i * stride + k]);

			if (i * stride + k < tgaimage.width * tgaimage.height * stride - 1)
			{
				fprintf(file, ",");
			}
		}
		fprintf(file, "\n");
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
