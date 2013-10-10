uniform sampler2D texSampler0;

varying vec2 v_texcoord;

/**
 * Pixel Shader
 */
void main() {
  gl_FragColor = texture2D(texSampler0, gl_TexCoord[0].st);
}
