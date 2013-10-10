#version 130

uniform sampler2D u_Diffuse;
uniform sampler2D u_NormalMap;
uniform sampler2D u_emissiveMap;
uniform sampler2D u_specularMap;

uniform int u_hasNormalMap;
uniform int u_hasEmissiveMap;
uniform int u_hasSpecularMap;


uniform float u_debugFloat;
uniform int u_debugInt;


uniform vec3 u_lightSource1;
uniform mat4 WorldTransform;
uniform mat4 uMVP;

uniform float u_time;


varying vec3 v_pos;
varying vec3 v_Normal;
varying vec3 v_lightDirTangentSpace;
varying vec3 v_viewDirTangentSpace;
varying vec4 v_screenPos;

void main(void)
{
	float LightIntensity = 0;
	vec3 MultiplicativeLight = vec3(1., 1., 1.);
	vec3 AdditiveLight = vec3(0., 0., 0.);
	vec3 camPos = uMVP[3].xyz * vec3(-1, -1, -1);
	vec3 lightDir = normalize(v_lightDirTangentSpace);
	vec3 Normal;

	if( u_hasNormalMap != 0 )
	{
		vec4 normalFragment = texture2D(u_NormalMap, gl_TexCoord[0].st);
		Normal = normalFragment.xyz;
		Normal *= 2;
		Normal -= vec3(1.0, 1.0, 1.0);

		Normal = normalize(Normal);

		LightIntensity = max(0, dot(Normal, lightDir ) );

	}
	else
	{
		Normal = v_Normal;
		LightIntensity = dot(v_Normal, normalize(u_lightSource1 - v_pos) ); 
	}
	
	if( u_hasEmissiveMap != 0)
	{
		AdditiveLight += texture2D( u_emissiveMap, gl_TexCoord[0].st).xyz;
	}

	if( u_hasSpecularMap != 0)
	{
		if( dot( v_viewDirTangentSpace,  Normal) > 0 )
		{
			vec3 viewDir = normalize(v_viewDirTangentSpace);
			vec4 specularFragment = texture2D( u_specularMap, gl_TexCoord[0].st);
			vec3 halfAngle = normalize((lightDir + viewDir) / (length(lightDir + viewDir) ) );
			AdditiveLight += specularFragment.xyz * pow( max( dot(Normal, halfAngle), 0), 32);

		}
	}
	
	LightIntensity = max(.2, LightIntensity);

	if( u_debugInt == 3)
	{
		//917.87798753429096492458597593536
		vec2 pos = gl_FragCoord.xy - vec2(800, 450);
		//float dist =  25000 / max(dot(pos, pos), 1);
		float Len = length(pos);
		float dist =  min(Len* 0.00167, 1);
		
		//dist = 3 * dist * dist - 2 * dist * dist * dist;
		//vec4 color = texture2D( u_Diffuse, gl_TexCoord[0].st);
		//float gray = (0.299*color.r + 0.587*color.g + 0.114*color.b);
		//gl_FragColor = vec4( vec3((dist), (dist), (dist)) * vec3(gray, gray, gray) + vec3(1 - dist, 1 - dist, 1 - dist)  * color.xyz, 1);

		if(Len < 350 )
		{
			gl_FragColor = texture2D( u_Diffuse, gl_TexCoord[0].st) * vec4(MultiplicativeLight, 1) * LightIntensity + vec4(AdditiveLight, 0);
		}
		else if( Len < 360)
		{
			gl_FragColor = vec4(0, 0, 0, 1);
		}
		else if(Len < 375)
		{
			gl_FragColor = vec4(.6, .6, .6, 1);
		}
		else if(Len < 385)
		{
			gl_FragColor = vec4(0, 0, 0, 1);
		}
		else
		{
			vec4 color = texture2D( u_Diffuse, gl_TexCoord[0].st);
			float gray = (0.299*color.r + 0.587*color.g + 0.114*color.b);
			gl_FragColor = vec4(  vec3(gray, gray, gray), 1);
		}
	}
	else
	{
		gl_FragColor = texture2D( u_Diffuse, gl_TexCoord[0].st) * vec4(MultiplicativeLight, 1) * LightIntensity + vec4(AdditiveLight, 0);
	}
}