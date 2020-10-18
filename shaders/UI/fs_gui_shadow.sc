$input v_color0, v_uv, v_position

#include "bgfx_shader.sh"

void main()
{
    vec2 auv = abs(v_uv - vec2(0.5)) - vec2(0.5);
    float d = length(max(auv, 0.));
    float a = atan(1. / 5. / d);
    float f = 0.5 * (cos(a) + 1.0);
	gl_FragColor = vec4(vec3(0.0), 1.0 - f);
}
