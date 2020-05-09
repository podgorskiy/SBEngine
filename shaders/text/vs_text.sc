$input a_position, a_texcoord0, a_color0
$output v_color0, v_uv

#include <bgfx_shader.sh>

uniform mat4 u_transform;

void main()
{
	v_color0 = a_color0;
	v_uv = a_texcoord0;
	gl_Position = u_transform * vec4(a_position, 0.0, 1.0);
}
