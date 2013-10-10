#version 120

uniform mat4 u_MV;
uniform vec2 u_position;

attribute vec3 a_position;
attribute vec2 a_texCoords;

varying vec2 v_texCoords;
varying vec3 v_worldPosition;

void main()
{	v_texCoords = a_texCoords;
	v_worldPosition = vec3( u_position.x, 0, u_position.y) + a_position.xyz;
	gl_Position = gl_ModelViewProjectionMatrix * u_MV * vec4(v_worldPosition, 1);
	
}