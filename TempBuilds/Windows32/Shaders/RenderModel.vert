#version 130

uniform mat4 WorldTransform;
uniform mat4 uMVP;
uniform vec3 u_camPos;
uniform mat4 u_BoneTMs[100];

uniform float u_time;
uniform mat4 u_spinMat;
uniform vec3 u_lightSource1;
uniform sampler2D u_NormalMap;
uniform int u_hasNormalMap;


uniform float u_debugFloat;
uniform int u_debugInt;

attribute vec3 a_position;
attribute vec3 a_tangent;
attribute vec3 a_biTangent;
attribute vec4 a_boneWeights;
attribute vec4 a_boneIDs;

varying vec3 v_pos;
varying vec3 v_Normal;
varying vec3 v_lightDirTangentSpace;
varying vec3 v_viewDirTangentSpace;
varying vec4 v_screenPos;

void main() 
{
	
	
	vec4 vWorld = vec4(0, 0, 0, 1);
	if( a_boneWeights[0] == 0.f )
	{
		vWorld = (WorldTransform *  gl_Vertex );

		if( u_hasNormalMap != 0)
		{

 			vec3 view = (u_camPos - vWorld.xyz);
 			vec3 lightDir = (u_lightSource1 - vWorld.xyz);
  			vec3 tangentWorld = ( WorldTransform *  vec4(a_tangent, 0)).xyz;
  			vec3 biTangentWorld = ( WorldTransform *  vec4(a_biTangent, 0)).xyz;
  			vec3 normalWorld = ( WorldTransform *  vec4(gl_Normal, 0)).xyz;

 			v_viewDirTangentSpace = (vec3(dot(tangentWorld, view), dot(biTangentWorld, view), dot( normalWorld, view) ) );
 			v_lightDirTangentSpace = (vec3(dot(tangentWorld, lightDir), dot(biTangentWorld, lightDir), dot( normalWorld, lightDir) ) );
		}
		else
		{
			v_Normal = normalize( (WorldTransform * vec4(gl_Normal, 0)).xyz);
		}
	}
	else
	{
		vec3 P = vec3(0,0,0);
		vec3 N = vec3(0,0,0);
		vec3 T = vec3(0,0,0);
		vec3 B = vec3(0,0,0);
		for ( int i = 0 ; i < 4; i++)
		{
			float bone_index = a_boneIDs[ i ];
			float bone_weight = a_boneWeights[ i ];
			P += (u_BoneTMs[int(bone_index)] * gl_Vertex).xyz * bone_weight;
			N += (u_BoneTMs[int(bone_index)] * vec4(gl_Normal, 0) ).xyz * bone_weight;
			T += (u_BoneTMs[int(bone_index)] * vec4(a_tangent, 0) ).xyz * bone_weight;
			B += (u_BoneTMs[int(bone_index)] * vec4(a_biTangent, 0) ).xyz * bone_weight;
		}
		vWorld = vec4(P, 1);

		
		if( u_hasNormalMap != 0)
		{

			vec3 view = (u_camPos - vWorld.xyz);
 			vec3 lightDir = (u_lightSource1 - vWorld.xyz);
 			v_viewDirTangentSpace = (vec3(dot(T, view), dot(B, view), dot( N, view) ) );
 			v_lightDirTangentSpace = (vec3(dot(T, lightDir), dot(B, lightDir), dot( N, lightDir) ) );
		}
		else
		{
			v_Normal = normalize( (WorldTransform * vec4(N, 0)).xyz);
		}
	}


	
	v_screenPos = gl_ModelViewProjectionMatrix * uMVP * vWorld;
	gl_Position = v_screenPos;
	v_pos = vWorld.xyz;

	
	gl_TexCoord[0] = gl_MultiTexCoord0;

}