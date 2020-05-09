$input v_position, v_color0, v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_tex, 0);

uniform mat3 u_paintMat;
uniform vec4 u_innerCol;
uniform vec4 u_outerCol;
uniform vec4 u_extentRadius;
uniform vec4 u_params;

#define u_extent       (u_extentRadius.xy)
#define u_radius       (u_extentRadius.z)
#define u_feather      (u_params.x)

SAMPLER2D(s_noise, 1);

float sdroundrect(vec2 pt, vec2 ext, float rad)
{
	vec2 ext2 = ext - vec2(rad,rad);
	vec2 d = abs(pt) - ext2;
	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0) ) - rad;
}

void main()
{
	vec4 result;
	
	// Calculate gradient color using box gradient
	vec2 pt = mul(u_paintMat, vec3(v_position, 1.0) ).xy;
	
	float sd = sdroundrect(pt, u_extent, u_radius);
	float d = clamp( (sd + u_feather*0.5) / u_feather, 0.0, 1.0);
	vec4 color = mix(u_innerCol, u_outerCol, d);

	color.xyz += (texture2D(s_noise, gl_FragCoord.xy / 128.0).xyz - vec3(0.5)) * 0.1;
	vec4 texel = texture2D(s_tex, v_texcoord0.xy);
	gl_FragColor = color * texel * v_color0;
}
