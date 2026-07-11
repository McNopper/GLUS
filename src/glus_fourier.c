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

static GLUSboolean glusFourierIsPowerOfTwo(const GLUSint n)
{
    GLUSint test = n;

    if (test < 1)
    {
        return GLUS_FALSE;
    }

    while (!(test & 0x1))
    {
        test = test >> 1;
    }

    return test == 1;
}

GLUSboolean glusFourierDFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (n > 0)
    {
        GLUSboolean status;

        GLUSint row, column;

        GLUSfloat scalar = 1.0f / (GLUSfloat)n;

        GLUScomplex* dftMatrix = (GLUScomplex*)glusMemoryMalloc(n * n * sizeof(GLUScomplex));

        if (!dftMatrix)
        {
            return GLUS_FALSE;
        }

        for (column = 0; column < n; column++)
        {
            for (row = 0; row < n; row++)
            {
                glusComplexRootOfUnityc(&dftMatrix[column * n + row], n, row * column, -1.0f);
            }
        }

        status = glusMatrixNxNMultiplyVectorNc(result, dftMatrix, vector, n);

        glusVectorNMultiplyScalarc(result, result, n, scalar);

        glusMemoryFree(dftMatrix);

        return status;
    }

    return GLUS_FALSE;
}

GLUSboolean glusFourierInverseDFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (n > 0)
    {
        GLUSboolean status;

        GLUSint row, column;

        GLUScomplex* dftInverseMatrix = (GLUScomplex*)glusMemoryMalloc(n * n * sizeof(GLUScomplex));

        if (!dftInverseMatrix)
        {
            return GLUS_FALSE;
        }

        for (column = 0; column < n; column++)
        {
            for (row = 0; row < n; row++)
            {
                glusComplexRootOfUnityc(&dftInverseMatrix[column * n + row], n, row * column, 1.0f);
            }
        }

        status = glusMatrixNxNMultiplyVectorNc(result, dftInverseMatrix, vector, n);

        glusMemoryFree(dftInverseMatrix);

        return status;
    }

    return GLUS_FALSE;
}

static GLUSvoid glusFourierRecursiveFunctionFFTc(GLUScomplex* vector, const GLUSint n, GLUSint offset)
{
    if (n > 1)
    {
        GLUSint i, k;

        GLUSint m = n / 2;

        GLUScomplex temp;
        for (i = 1; i < m; i++)
        {
            for (k = 0; k < m - i; k++)
            {
                temp                           = vector[offset + 2 * k + i];
                vector[offset + 2 * k + i]     = vector[offset + 2 * k + 1 + i];
                vector[offset + 2 * k + 1 + i] = temp;
            }
        }

        glusFourierRecursiveFunctionFFTc(vector, m, offset);
        glusFourierRecursiveFunctionFFTc(vector, m, offset + m);

        GLUScomplex currentW;
        currentW.real      = 1.0f;
        currentW.imaginary = 0.0f;

        GLUScomplex w;
        glusComplexRootOfUnityc(&w, n, 1, 1.0f);

        for (i = 0; i < m; i++)
        {
            GLUScomplex multiply;
            GLUScomplex addition;
            GLUScomplex subtraction;

            glusComplexMultiplyComplexc(&multiply, &currentW, &vector[offset + i + m]);

            glusComplexAddComplexc(&addition, &vector[offset + i], &multiply);
            glusComplexSubtractComplexc(&subtraction, &vector[offset + i], &multiply);

            vector[offset + i]     = addition;
            vector[offset + i + m] = subtraction;

            glusComplexMultiplyComplexc(&currentW, &currentW, &w);
        }
    }
    else
    {
        // c0 = v0, so do nothing.
    }
}

GLUSboolean glusFourierRecursiveFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (glusFourierIsPowerOfTwo(n))
    {
        GLUSfloat scalar = 1.0f / (GLUSfloat)n;

        glusVectorNCopyc(result, vector, n);

        glusVectorNConjugatec(result, result, n);

        glusFourierRecursiveFunctionFFTc(result, n, 0);

        glusVectorNConjugatec(result, result, n);
        glusVectorNMultiplyScalarc(result, result, n, scalar);

        return GLUS_TRUE;
    }

    return GLUS_FALSE;
}

GLUSboolean glusFourierRecursiveInverseFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (glusFourierIsPowerOfTwo(n))
    {
        glusVectorNCopyc(result, vector, n);

        glusFourierRecursiveFunctionFFTc(result, n, 0);

        return GLUS_TRUE;
    }

    return GLUS_FALSE;
}

static GLUSvoid glusFourierButterflyShuffleFunctionFFTc(GLUScomplex* vector, const GLUSint n, const GLUSint offset)
{
    if (n > 1)
    {
        GLUSint i, k;

        GLUSint m = n / 2;

        GLUScomplex temp;
        for (i = 1; i < m; i++)
        {
            for (k = 0; k < m - i; k++)
            {
                temp                           = vector[offset + 2 * k + i];
                vector[offset + 2 * k + i]     = vector[offset + 2 * k + 1 + i];
                vector[offset + 2 * k + 1 + i] = temp;
            }
        }

        glusFourierButterflyShuffleFunctionFFTc(vector, m, offset);
        glusFourierButterflyShuffleFunctionFFTc(vector, m, offset + m);
    }
    else
    {
        // c0 = v0, so do nothing.
    }
}

GLUSboolean glusFourierButterflyShuffleFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (glusFourierIsPowerOfTwo(n))
    {
        glusVectorNCopyc(result, vector, n);

        glusFourierButterflyShuffleFunctionFFTc(result, n, 0);

        return GLUS_TRUE;
    }

    return GLUS_FALSE;
}

static GLUSvoid glusFourierButterflyFunctionFFTc(GLUScomplex* vector, const GLUSint n, const GLUSint offset)
{
    GLUSint currentStep;
    GLUSint currentSection;
    GLUSint currentButterfly;

    GLUSint numberSections;
    GLUSint numberButterfliesInSection;

    GLUSint m = n / 2;

    GLUSint steps = 0;
    GLUSint temp  = n;
    while (!(temp & 0x1))
    {
        temp = temp >> 1;
        steps++;
    }

    numberSections             = m;
    numberButterfliesInSection = 1;

    for (currentStep = 0; currentStep < steps; currentStep++)
    {
        for (currentSection = 0; currentSection < numberSections; currentSection++)
        {
            GLUScomplex currentW;
            currentW.real      = 1.0f;
            currentW.imaginary = 0.0f;

            GLUScomplex w;
            glusComplexRootOfUnityc(&w, numberButterfliesInSection * 2, 1, 1.0f);

            for (currentButterfly = 0; currentButterfly < numberButterfliesInSection; currentButterfly++)
            {
                GLUSint leftIndex  = currentButterfly + currentSection * numberButterfliesInSection * 2;
                GLUSint rightIndex = currentButterfly + numberButterfliesInSection + currentSection * numberButterfliesInSection * 2;

                GLUScomplex multiply;
                GLUScomplex addition;
                GLUScomplex subtraction;

                glusComplexMultiplyComplexc(&multiply, &currentW, &vector[rightIndex]);

                glusComplexAddComplexc(&addition, &vector[leftIndex], &multiply);
                glusComplexSubtractComplexc(&subtraction, &vector[leftIndex], &multiply);

                vector[leftIndex]  = addition;
                vector[rightIndex] = subtraction;

                glusComplexMultiplyComplexc(&currentW, &currentW, &w);
            }
        }

        numberButterfliesInSection *= 2;
        numberSections /= 2;
    }
}

GLUSboolean glusFourierButterflyFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (glusFourierIsPowerOfTwo(n))
    {
        GLUSfloat scalar = 1.0f / (GLUSfloat)n;

        glusVectorNCopyc(result, vector, n);

        glusVectorNConjugatec(result, result, n);

        glusFourierButterflyShuffleFFTc(result, result, n);

        glusFourierButterflyFunctionFFTc(result, n, 0);

        glusVectorNConjugatec(result, result, n);
        glusVectorNMultiplyScalarc(result, result, n, scalar);

        return GLUS_TRUE;
    }

    return GLUS_FALSE;
}

GLUSboolean glusFourierButterflyInverseFFTc(GLUScomplex* result, const GLUScomplex* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (glusFourierIsPowerOfTwo(n))
    {
        glusVectorNCopyc(result, vector, n);

        glusFourierButterflyShuffleFFTc(result, result, n);

        glusFourierButterflyFunctionFFTc(result, n, 0);

        return GLUS_TRUE;
    }

    return GLUS_FALSE;
}

static GLUSvoid glusFourierButterflyShuffleFunctionFFTi(GLUSint* vector, const GLUSint n, const GLUSint offset)
{
    if (n > 1)
    {
        GLUSint i, k;

        GLUSint m = n / 2;

        GLUSint temp;
        for (i = 1; i < m; i++)
        {
            for (k = 0; k < m - i; k++)
            {
                temp                           = vector[offset + 2 * k + i];
                vector[offset + 2 * k + i]     = vector[offset + 2 * k + 1 + i];
                vector[offset + 2 * k + 1 + i] = temp;
            }
        }

        glusFourierButterflyShuffleFunctionFFTi(vector, m, offset);
        glusFourierButterflyShuffleFunctionFFTi(vector, m, offset + m);
    }
    else
    {
        // c0 = v0, so do nothing.
    }
}

GLUSboolean glusFourierButterflyShuffleFFTi(GLUSint* result, const GLUSint* vector, const GLUSint n)
{
    if (!result || !vector)
    {
        return GLUS_FALSE;
    }

    if (glusFourierIsPowerOfTwo(n))
    {
        GLUSint i;

        for (i = 0; i < n; i++)
        {
            result[i] = vector[i];
        }

        glusFourierButterflyShuffleFunctionFFTi(result, n, 0);

        return GLUS_TRUE;
    }

    return GLUS_FALSE;
}
