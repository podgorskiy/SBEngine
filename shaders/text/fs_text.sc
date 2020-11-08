$input v_color0, v_uv

#include "bgfx_shader.sh"

uniform vec4 u_stroke;
SAMPLER2D(u_texture, 0);


float sdf2alpha(float d)
{
    float distanceChange = fwidth(d) * 0.5;
    return smoothstep(distanceChange, -distanceChange, d);
}

void main()
{
	// vec2 color = texelFetch(u_texture, ivec2(v_uv), 0).rg;
	vec2 color = texture2D(u_texture, v_uv / vec2(textureSize(u_texture, 0))).rg;
	color = vec2(sdf2alpha(0.5 - color.x), sdf2alpha(0.5 - color.y));

	// color = pow(color, vec2(2.2));

	vec4 color_strock = vec4(u_stroke.rgb, color.g * v_color0.a * u_stroke.a);
	vec4 color_fill   = vec4(v_color0.rgb, color.r * v_color0.a);

	// Backward blending. We blend fill image (r channel) over stroke image (g channel), and the result is then blended
	// over background. That's opposite of forward blending: stroke image over background and then fill image is blended
	// over the result.
	//
	//    fill image        -| We compute result of these two first
	//   stroke image       -|
	// background image
	//
	float result_alpha = color_strock.a - color_strock.a * color_fill.a + color_fill.a;

	vec3 result_rgb = (color_strock.rgb * color_strock.a * (1.0 - color_fill.a) + color_fill.rgb * color_fill.a);

	result_rgb = pow(result_rgb, vec3(2.2));

	gl_FragColor = vec4(result_rgb, result_alpha);
}
