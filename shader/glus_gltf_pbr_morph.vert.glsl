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

#version 460 core

// glTF 2.0 morph targets: per-target POSITION / NORMAL / TANGENT deltas packed
// into SSBOs as [target][vertex] are blended by u_morphWeights before the
// model transform. Must match GLUS_GLTF_MAX_MORPH_TARGETS in glus_gltf.c.
#define MAX_MORPH_TARGETS 32

uniform mat4 u_modelMatrix;
uniform mat4 u_viewProjectionMatrix;
uniform mat3 u_normalMatrix;

uniform int   u_morphTargetCount;
uniform int   u_morphVertexCount;
uniform int   u_hasMorphNormal;
uniform int   u_hasMorphTangent;
uniform float u_morphWeights[MAX_MORPH_TARGETS];

layout(std430, binding = 1) readonly buffer MorphPos { float morphPos[]; }; // vec3 per vertex per target
layout(std430, binding = 2) readonly buffer MorphNor { float morphNor[]; };
layout(std430, binding = 3) readonly buffer MorphTan { float morphTan[]; }; // vec4 per vertex per target

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_tangent; // xyz=tangent, w=handedness
layout(location = 3) in vec2 a_texCoord0;
layout(location = 6) in vec2 a_texCoord1;
layout(location = 7) in vec4 a_color0;

out vec3 v_worldPos;
out vec3 v_normal;
out vec4 v_tangent;
out vec2 v_texCoord0;
out vec2 v_texCoord1;
out vec4 v_color;

void main(void)
{
    vec3 pos = a_position;
    vec3 nrm = a_normal;
    vec4 tan = a_tangent;

    for (int t = 0; t < u_morphTargetCount; t++)
    {
        float w = u_morphWeights[t];
        if (w == 0.0)
        {
            continue;
        }
        int base = (t * u_morphVertexCount + gl_VertexID);
        pos += w * vec3(morphPos[base * 3 + 0], morphPos[base * 3 + 1], morphPos[base * 3 + 2]);
        if (u_hasMorphNormal != 0)
        {
            nrm += w * vec3(morphNor[base * 3 + 0], morphNor[base * 3 + 1], morphNor[base * 3 + 2]);
        }
        if (u_hasMorphTangent != 0)
        {
            tan += w * vec4(morphTan[base * 4 + 0], morphTan[base * 4 + 1], morphTan[base * 4 + 2], morphTan[base * 4 + 3]);
        }
    }

    vec4 worldPos = u_modelMatrix * vec4(pos, 1.0);
    v_worldPos    = worldPos.xyz;
    v_normal      = normalize(u_normalMatrix * nrm);
    v_tangent     = vec4(normalize(u_normalMatrix * tan.xyz), tan.w);
    v_texCoord0   = a_texCoord0;
    v_texCoord1   = a_texCoord1;
    v_color       = a_color0;
    gl_Position   = u_viewProjectionMatrix * worldPos;
}
