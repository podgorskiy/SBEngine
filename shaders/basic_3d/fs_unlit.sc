$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(u_texture, 0);

void main()
{
	vec3 color = texture2D(u_texture, v_texcoord0).rgb;
	gl_FragColor = vec4(color, 1.0);
}
