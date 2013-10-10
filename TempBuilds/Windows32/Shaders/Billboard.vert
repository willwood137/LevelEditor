uniform vec3 u_camPos;
uniform vec2 u_size;
uniform vec3 u_position;
uniform mat4 u_viewMatrix;

attribute vec2 a_textureCoords;

void main() {

	

	vec2 offset = (a_textureCoords + vec2(-.5, -.5));

	offset.y *= -1;
	
	vec3 axis1 = vec3(	u_viewMatrix[0][0], 
						u_viewMatrix[1][0],
						u_viewMatrix[2][0]);

	vec3 axis2 = vec3(	u_viewMatrix[0][1], 
						u_viewMatrix[1][1],
						u_viewMatrix[2][1]);

	vec3 pos = (offset.x*axis1*u_size.x + offset.y*axis2*u_size.y) + u_position; 
	gl_Position = gl_ModelViewProjectionMatrix * u_viewMatrix * vec4( pos, 1 );
	gl_TexCoord[0] = vec4(a_textureCoords.xy, 0, 0);
}
			
	