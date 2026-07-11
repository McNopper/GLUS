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

#version 460 core

uniform mat4 u_modelMatrix;
uniform mat4 u_viewProjectionMatrix;
uniform mat3 u_normalMatrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_tangent; // xyz=tangent, w=handedness
layout(location = 3) in vec2 a_texCoord0;
layout(location = 6) in vec2 a_texCoord1; // second UV set (default 0,0)
layout(location = 7) in vec4 a_color0;    // vertex color (default 1,1,1,1)

out vec3 v_worldPos;
out vec3 v_normal;
out vec4 v_tangent;
out vec2 v_texCoord0;
out vec2 v_texCoord1;
out vec4 v_color;

void main(void)
{
    vec4 worldPos = u_modelMatrix * vec4(a_position, 1.0);
    v_worldPos    = worldPos.xyz;
    v_normal      = normalize(u_normalMatrix * a_normal);
    v_tangent     = vec4(normalize(u_normalMatrix * a_tangent.xyz), a_tangent.w);
    v_texCoord0   = a_texCoord0;
    v_texCoord1   = a_texCoord1;
    v_color       = a_color0;
    gl_Position   = u_viewProjectionMatrix * worldPos;
}
