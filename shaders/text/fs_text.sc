$input v_color0, v_uv

#include "bgfx_shader.sh"

SAMPLER2D(u_texture, 0);

void main()
{
	vec2 color = texelFetch(u_texture, ivec2(v_uv), 0).rg;

	vec4 color_strock = vec4(vec3(0.0, 0.0, 0.0), color.g * v_color0.a);
	vec4 color_fill = vec4(v_color0.rgb, color.r * v_color0.a);

	// backward blending order
	float alpha = color_strock.a - color_strock.a * color_fill.a + color_fill.a;

	gl_FragColor = vec4((color_strock.rgb * color_strock.a * (1.0 - color_fill.a) + color_fill.rgb * color_fill.a) / alpha, alpha);
}
