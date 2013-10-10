#version 120

uniform mat4 u_MV;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec2 a_texCoords;

varying vec4 v_color;
varying vec2 v_texCoords;

void main()
{
	v_color = vec4(a_color, 1);
	v_texCoords = a_texCoords;
	gl_Position = gl_ModelViewProjectionMatrix * u_MV * vec4(a_position, 1);
}