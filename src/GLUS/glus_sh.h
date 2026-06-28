/*
 * GLUS - Modern OpenGL, OpenGL ES and OpenVG Utilities. Copyright (C) since 2010 Norbert Nopper
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

#ifndef GLUS_SH_H_
#define GLUS_SH_H_

/*
 * Spherical-harmonics (Wigner-D) rotation matrices, built recursively per band
 * from a 3x3 rotation matrix using the Ivanic-Ruedenberg method.
 *
 * Convention:
 *  - Standard real spherical harmonics; per band the coefficients are ordered
 *    m = -l .. +l (band 1 = (y, z, x), i.e. SH(1,-1,+y)=SH(1,0,+z)=SH(1,+1,+x)>0).
 *  - Matrices are stored column-major (OpenGL convention): element (row r, col c)
 *    lives at index c*N+r and is applied as out = D * coeffs.
 *  - For a rotation R, D(R) satisfies eval(d; D(R) c) = eval(R d; c).
 *
 * Validated (orthogonality, group homomorphism and equivariance) against the
 * reference implementation in https://github.com/andrewwillmott/sh-lib.
 *
 * Note: the Inria / 3D-Gaussian-Splatting / glTF KHR_gaussian_splatting SH sign
 * convention differs from this standard basis by the Condon-Shortley phase
 * (-1)^m (i.e. a sign flip of all odd-m components). Bridge it by conjugating
 * each band matrix with that diagonal involution S = diag((-1)^m), m = -l..+l:
 * D_3dgs = S * D_std * S.
 */

/**
 * Build the 3x3 Wigner-D rotation matrix for spherical harmonics band 1.
 *
 * All matrices use column-major storage (OpenGL convention):
 * element at row r, column c is stored at index c*N+r.
 *
 * @param result   Output 3x3 band-1 Wigner-D matrix (9 floats, column-major).
 * @param rotation Input 3x3 rotation matrix (9 floats, column-major).
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusSHBuildRotation1f(GLUSfloat result[9], const GLUSfloat rotation[9]);

/**
 * Build the 5x5 Wigner-D rotation matrix for spherical harmonics band 2.
 *
 * All matrices use column-major storage (OpenGL convention).
 *
 * @param result Output 5x5 band-2 Wigner-D matrix (25 floats, column-major).
 * @param sh1    Band-1 Wigner-D matrix from glusSHBuildRotation1f (9 floats, column-major).
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusSHBuildRotation2f(GLUSfloat result[25], const GLUSfloat sh1[9]);

/**
 * Build the 7x7 Wigner-D rotation matrix for spherical harmonics band 3.
 *
 * All matrices use column-major storage (OpenGL convention).
 *
 * @param result Output 7x7 band-3 Wigner-D matrix (49 floats, column-major).
 * @param sh1    Band-1 Wigner-D matrix from glusSHBuildRotation1f (9 floats, column-major).
 * @param sh2    Band-2 Wigner-D matrix from glusSHBuildRotation2f (25 floats, column-major).
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusSHBuildRotation3f(GLUSfloat result[49], const GLUSfloat sh1[9], const GLUSfloat sh2[25]);

#endif /* GLUS_SH_H_ */
