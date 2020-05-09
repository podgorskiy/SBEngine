$input a_position, a_texcoord0
$output v_texcoord0

uniform mat4 u_projection;

#include <bgfx_shader.sh>

void main()
{
	vec4 pos = u_modelView * vec4(a_position, 1.0);
	v_texcoord0 = a_texcoord0;
	gl_Position = u_projection * pos;
}
