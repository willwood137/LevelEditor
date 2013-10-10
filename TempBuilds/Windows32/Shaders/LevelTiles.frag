
uniform sampler2D u_diffuse;

varying vec4 v_color;
varying vec2 v_texCoords;

void main()
{
	//gl_FragColor = texture2D( u_diffuse, v_texCoords) * v_color;
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}