$input v_color0, v_uv, v_position

#include "bgfx_shader.sh"

uniform vec4      iResolution;           // viewport resolution (in pixels)
uniform vec4      iTime;                 // shader playback time (in seconds)

#define COL(R, G, B) pow(vec3(float(R), float(G), float(B)) / 255., vec3(2.2))

vec2 rotate(vec2 uv, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    mat2 m = mat2(c,s,-s,c);
    return m * uv;
}

float dot2(in vec2 v ) { return dot(v,v); }
float dot3(in vec2 v ) { return dot(v,v) * length(v); }
float dot4(in vec2 v ) { return dot(v,v) * dot(v,v); }

float sdTrapezoid(in float r1, float r2, float he, in vec2 p)
{
    vec2 k1 = vec2(r2,he);
    vec2 k2 = vec2(r2-r1,2.0*he);
    p.x = abs(p.x);
    vec2 ca = vec2(p.x-min(p.x,(p.y<0.0)?r1:r2), abs(p.y)-he);
    vec2 cb = p - k1 + k2*clamp( dot(k1-p,k2)/dot2(k2), 0.0, 1.0 );
    float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
    return s*sqrt( min(dot2(ca),dot2(cb)) );
}

vec4 render(float anti, float d, vec3 color)
{
    return vec4(color, smoothstep(anti, -anti, d));
}

vec4 render(float anti, float d, vec4 color)
{
    return vec4(color.rgb, color.a * smoothstep(anti, -anti, d));
}

vec4 render_stroked(float anti, float d, vec3 color, float stroke)
{
    vec4 strokeLayer = vec4(vec3(0.01), smoothstep(anti, -anti, d - stroke));
    vec4 colorLayer = vec4(color, smoothstep(anti, -anti, d));
    return vec4(mix(strokeLayer.rgb, colorLayer.rgb, colorLayer.a), strokeLayer.a);
}

void render_layer(inout vec4 c, vec4 layer)
{
    c.rgb = mix(c.rgb, layer.rgb, layer.a);
}

#define LayerF(a, d, color) render_layer(fragColor, render(a, d, color))
#define LayerS(a, d, color, stroke) render_layer(fragColor, render_stroked(a, d, color, stroke))


void make_sword(inout vec4 fragColor, vec2 uv, float offset)
{
    uv *= 10.0;
    uv.y += 23.0 - offset;
    float a = 2.0 / iResolution.y * 10.0;

    float d1 = sdTrapezoid(1.1, 1.0, 16.0, uv + vec2(0., 8.));
    float d2 = sdTrapezoid(1.0, 0.0, 2.0, uv - vec2(0., 10.));
    float d = min(d1, d2) - 0.04;
    float d3 = sdTrapezoid(0.2, 0.05, 8.0, uv + vec2(0., 2.));
    LayerF(a, d, COL(247, 246, 233));
    LayerF(a, d3, vec3(0., 0., 0.) / 255.);
}

float hash2(vec2 uv) {
  return fract(dot(sin(uv*425.215+uv.yx*714.388),vec2(522.877)));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord )
{
    vec2 cuv = (fragCoord-0.5*iResolution.xy)/iResolution.y * 2.0;
    vec2 uv = (fragCoord)/iResolution.y * 2.0;

    float time = 2. * (iTime.x + 0.2 * float(int(fragCoord.x) % 8) / 8.0 - 1.0);

    fragColor.rgb = COL(0, 0, 0) * (0.8 + 0.2*cuv.y) *(1.1-0.5*length(cuv));
    // fragColor.r += 0.02 * smoothstep(2.0, 6.0, iTime.x);

    //cuv = rotate(cuv, 3.14 / 3.0);
    //make_sword(fragColor, cuv, min(70. * (time - 0.0), 9.0));

    //cuv = rotate(cuv, -3.14 / 3.0);
    //make_sword(fragColor, cuv, min(70. * (time - 1.0), 9.0));

    //cuv = rotate(cuv, -3.14 / 3.0);
    //make_sword(fragColor, cuv, min(70. * (time - 2.0), 9.0));

    cuv = rotate(cuv, 3.14 - 0.5);
    make_sword(fragColor, cuv, min(70. * (time - 0.0), 21.0));

    cuv = rotate(cuv, 0.5);
    make_sword(fragColor, cuv, min(70. * (time - 1.0), 21.0));

    cuv = rotate(cuv, 0.5);
    make_sword(fragColor, cuv, min(70. * (time - 2.0), 21.0));

    float n = hash2(cuv * 200.0) - 0.5;
    fragColor += n * 0.001;
    fragColor.a = 1.0;
}

void main()
{
    vec4 fragColor;
	mainImage(fragColor, v_uv * iResolution.xy);
	gl_FragColor = fragColor;
}
