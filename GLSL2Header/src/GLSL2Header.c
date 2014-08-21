/*
 * GLSL2Header - Shader to header file generator. Copyright (C) 2013 Norbert Nopper
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
	GLUStextfile glslfile;

	FILE* file;

	GLUSint i;

	GLUSchar* filename;

	GLUSchar uppercaseFilename[256];

	GLUSchar lastChar = 0;

	GLUSboolean commentMultipleLines = GLUS_FALSE;

	GLUSboolean commentEndOfLine = GLUS_FALSE;

	//

	if (argc != 3)
	{
		printf("Usage: GLSL2Header.exe [GLSL Filename] [Header Filename]\n");

		return -1;
	}

	//

	printf("Loading '%s' ... ", argv[1]);
	if (!glusFileLoadText(argv[1], &glslfile))
	{
		printf("failed! GLSL file could not be loaded.\n");

		return -1;
	}
	printf("completed!\n");

	//

	printf("Saving '%s' ... ", argv[2]);

	file = fopen(argv[2], "w");

	if (!file)
	{
		printf("failed! Could not create/open header file.\n");

		glusFileDestroyText(&glslfile);

		return -1;
	}

	//

	filename = strtok(argv[2], ".");

	if (!filename || strlen(filename) >= 255)
	{
		printf("failed! Invalid filename.\n");

		fclose(file);

		glusFileDestroyText(&glslfile);

		return -1;
	}

	for (i = 0; i < strlen(filename); i++)
	{
		uppercaseFilename[i] = toupper(filename[i]);
	}
	uppercaseFilename[i] = '\0';

	//

	fprintf(file, "#ifndef %s_GLSL_HEADER\n", uppercaseFilename);
	fprintf(file, "#define %s_GLSL_HEADER\n\n", uppercaseFilename);

	fprintf(file, "static const char* %s_glsl = \"", filename);
	for (i = 0; i < glslfile.length; i++)
	{
		if (glslfile.text[i] == 0)
		{
			break;
		}
		else if (glslfile.text[i] == '\n')
		{
			fprintf(file, "\"\n");
			fprintf(file, "\"");

			commentEndOfLine = GLUS_FALSE;
			lastChar = 0;
		}
		else
		{
			// This code does remove all comments. Alternatively, escape characters could be created.
			// Did not do this for keeping the generated header readable.
			if (!commentEndOfLine && !commentMultipleLines)
			{
				if (glslfile.text[i] == '/')
				{
					if (lastChar == '/')
					{
						commentEndOfLine = GLUS_TRUE;
						lastChar = 0;

						continue;
					}
				}
				else if (glslfile.text[i] == '*')
				{
					if (lastChar == '/')
					{
						commentMultipleLines = GLUS_TRUE;
						lastChar = 0;

						continue;
					}
				}
				else
				{
					if (lastChar == '*' || lastChar == '/')
					{
						fputc(lastChar, file);
					}

					fputc(glslfile.text[i], file);
				}
			}
			else if (commentEndOfLine)
			{
				// Do nothing. Will be ended when end of line is reached.
			}
			else if (commentMultipleLines)
			{
				if (lastChar == '*' && glslfile.text[i] == '/')
				{
					commentMultipleLines = GLUS_FALSE;
					lastChar = 0;

					continue;
				}
			}

			lastChar = glslfile.text[i];
		}
	}
	fprintf(file, "\";\n\n");

	fprintf(file, "#endif // %s_GLSL_HEADER\n", uppercaseFilename);

	//

	fclose(file);

	printf("completed!\n");

	//

	glusFileDestroyText(&glslfile);

	return 0;
}
