$input a_position, a_texcoord0
$output v_texcoord0

#include <bgfx_shader.sh>

void main()
{
	vec4 pos = mul(u_modelView, vec4(a_position, 1.0));
	v_texcoord0 = a_texcoord0;
	gl_Position = mul(u_proj, pos);
}
