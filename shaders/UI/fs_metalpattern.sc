$input v_color0, v_uv, v_position

#include "bgfx_shader.sh"

uniform vec4      iResolution;           // viewport resolution (in pixels)
uniform vec4      iTime;                 // shader playback time (in seconds)
float ndot(vec2 a, vec2 b ) { return a.x*b.x - a.y*b.y; }
float sdRhombus( in vec2 p, in vec2 b )
{
    vec2 q = abs(p);
    float h = clamp((-2.0*ndot(q,b)+ndot(b,b))/dot(b,b),-1.0,1.0);
    float d = length( q - 0.5*b*vec2(1.0-h,1.0+h) );
    return d * sign( q.x*b.y + q.y*b.x - b.x*b.y );
}

vec4 render(float anti, float d, vec3 color)
{
    return vec4(color, smoothstep(anti, -anti, d));
}

vec4 render(float anti, float d, vec4 color)
{
    return vec4(color.rgb, color.a * smoothstep(anti, -anti, d));
}

void render_layer(inout vec4 c, vec4 layer)
{
    c.rgb = mix(c.rgb, layer.rgb, layer.a);
}

void render_layer_mul(inout vec4 c, vec4 layer)
{
    c.rgb = mix(c.rgb, c.rgb * layer.rgb, layer.a);
}

#define LayerF(a, d, color) render_layer(fragColor, render(a, d, color))
#define LayerA(a, d, color) fragColor = vec4(color.rgb, color.a * smoothstep(a, -a, d))

float hash2(vec2 uv) {
  return fract(dot(sin(uv*425.215+uv.yx*714.388),vec2(522.877)));
}

vec2 rotate(vec2 uv, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    mat2 m = mat2(c,s,-s,c);
    return m * uv;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    fragCoord.x = iResolution.x - fragCoord.x;
    vec2 cuv = (fragCoord)/iResolution.y * 2.0;

    vec2 size = vec2(0.1, 0.05) * 0.8;
    vec2 stepp = vec2(0.3, 0.2) * 0.8;

    //size *= 0.03 + smoothstep(-1.8, 2.0, 1.0 - (cuv.x * 0.3 + cuv.y * 0.9));
    float p = (cuv.x * 0.3 + cuv.y * 0.9);
    size *= 0.03 + max(0.0, smoothstep(-2.2, 2.0, 1.0 - p) + 0.1 * sin(p*10.0 + 5. * iTime.x));

    float a = 2.0 / iResolution.y;
    cuv = rotate(cuv, -0.2);
    vec2 r1uv = mod(cuv + stepp / 2.0, stepp) - stepp / 2.0;
    vec2 r2uv = mod((cuv + stepp / 2.0) + stepp / 2.0, stepp) - stepp / 2.0;
    //r1uv = rotate(r1uv, -0.0);
    //r2uv = rotate(r2uv, -0.0);
    float d1 = sdRhombus(r1uv, size);
    float d2 = sdRhombus(r2uv, size);
    float d = min(d1, d2);
    // Output to screen
    LayerA(a, d, vec4(0.3, 0.3, 0.4, 0.6));
}

void main()
{
    vec4 fragColor;
	mainImage(fragColor, v_uv * iResolution.xy);
	gl_FragColor = fragColor;
}
