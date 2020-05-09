$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(u_texture, 0);

void main()
{
	vec3 color = texture2D(u_texture, v_texcoord0).rgb;
	color = pow(color, vec3(2.2));
	gl_FragColor = vec4(pow(color, vec3(1.0/2.2)), 1.0);
}
