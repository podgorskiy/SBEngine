$input a_position, a_texcoord0, a_color0
$output v_color0, v_uv, v_position

#include <bgfx_shader.sh>

void main()
{
	v_color0 = a_color0;
	v_uv = a_texcoord0;
	vec4 p = mul(u_proj, vec4(a_position, 0.0, 1.0));
	v_position = p.xy;
	gl_Position = p;
}
