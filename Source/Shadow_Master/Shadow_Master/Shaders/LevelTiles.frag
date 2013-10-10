
uniform sampler2D u_diffuse;

uniform vec3 u_intersection;
uniform vec2 u_position;

varying vec2 v_texCoords;
varying vec3 v_worldPosition;

void main()
{
	float x = u_intersection.x - v_worldPosition.x;
	float y = u_intersection.y - v_worldPosition.y;
	float z = u_intersection.z - v_worldPosition.z;

	if( (x*x + y*y + z*z) < 25.0f )
		gl_FragColor = texture2D( u_diffuse, v_texCoords)  + vec4( .6, .4, 0.0, 0);
	else
		gl_FragColor =  texture2D( u_diffuse, v_texCoords);
}