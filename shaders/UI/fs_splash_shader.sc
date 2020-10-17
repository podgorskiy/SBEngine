$input v_color0, v_uv, v_position

#include "bgfx_shader.sh"

uniform vec4      iResolution;           // viewport resolution (in pixels)
uniform vec4      iTime;                 // shader playback time (in seconds)
// uniform sampler2D iChannel0;
SAMPLER2D(iChannel0,  0);

#define PI 			3.1415926535
#define MAXFLOAT	99999.99

struct Ray
{
    vec3 origin;
    vec3 direction;
};

float hash(float t) {
  return fract(sin(t*788.874));
}

float curve(float t, float d) {
  t/=d;
  return mix(hash(floor(t)), hash(floor(t)+1.0), pow(smoothstep(0.0,1.0,fract(t)),10.0));
}

float hash2(vec2 uv) {
  return fract(dot(sin(uv*425.215+uv.yx*714.388),vec2(522.877)));
}

vec2 hash22(vec2 uv) {
  return fract(sin(uv*425.215+uv.yx*714.388)*vec2(522.877));
}

vec3 hash3(vec2 id) {
  return fract(sin(id.xyy*vec3(427.544,224.877,974.542)+id.yxx*vec3(947.544,547.847,652.454))*342.774);
}


struct Camera
{
    float apertureRadius;
    vec3 origin;
    mat3 cam;
    mat3 k_inv;
    float focusDist;
};


// vfov is top to bottom in degrees
void Camera_init(out Camera camera, vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focusDist)
{
    camera.apertureRadius = aperture;
    camera.focusDist = focusDist;

    float theta = vfov * PI / 180.0;
    float halfHeight = tan(theta / 2.0);
    float halfWidth = aspect * halfHeight;

    camera.origin = lookfrom;

    vec3 z = normalize(lookfrom - lookat);
    vec3 x = normalize(cross(vup, z));
    vec3 y = normalize(cross(z, x));

    camera.cam = mat3(x, y, z);
    camera.k_inv = mat3(vec3(halfWidth, 0, 0), vec3(0, halfHeight, 0), vec3(0.0, 0.0, -1.));
}


vec3 random_in_unit_disk(vec2 uv, float j)
{
    uv = hash22(uv + 13.0 * j);
    float theta = 6.283185 * uv.x;
    return vec3(sqrt(uv.y) * vec2(cos(theta), sin(theta)), 0.0);
}

Ray Camera_getRay_true(Camera camera, vec2 uv, float j)
{
    vec3 rd = camera.apertureRadius * random_in_unit_disk(uv, j);

    Ray ray;
    uv = 2.0 * uv - 1.0;
    ray.origin = camera.origin;
    ray.direction = camera.cam * (normalize(camera.k_inv * vec3(uv, 1.0)));

    return ray;
}

Ray Camera_getRay(Camera camera, vec2 uv, float j)
{
    vec3 rd = camera.apertureRadius * random_in_unit_disk(uv, j);

    Ray ray;
    vec2 pixel = 1.0/iResolution.xy;
	uv += pixel * (hash22(uv) - 0.5);
    uv = 2.0 * uv - 1.0;

    ray.origin = camera.origin + camera.cam * rd;

    vec3 dir = camera.k_inv * vec3(uv, 1.0);
    dir -= rd / camera.focusDist;

    ray.direction = normalize(camera.cam * dir);

    return ray;
}

vec3 trace(Ray ray, float j, vec2 uv, float time)
{
        vec3 size = vec3(4.0,3.0,10.);

        vec3 alpha=vec3(1);
        vec3 col = vec3(0);

        // Bounces
        for(float i=0.0; i<3.0; ++i) {

          // box collision
          vec3 boxmin = (size-ray.origin) / ray.direction;
          vec3 boxmax = (-size-ray.origin) / ray.direction;

          vec3 box=max(boxmin,boxmax);

          // only check box x and y axis
          float d = min(box.x, box.y);
          vec3 p = ray.origin + ray.direction * d;
          vec2 cuv = p.xz;
          vec3 n=vec3(0,sign(box.y),0);

          if(box.x<box.y) {
            cuv=p.yz;
            cuv.x+=1.0;
            n=vec3(sign(box.x),0.0,0.0);
          }

          vec3 p2 = p;
          ivec2 id = ivec2(floor(cuv));

          float tp1 = float(id.x % 2)* 2.0 - 1.0;
          float tp2 = float(id.y % 2)* 2.0 - 1.0;
          float rough = min(1.0, 0.95 + 0.1 * tp1 * tp2);


          float denom = ray.direction.z;
          float plane_t = (30.0 - ray.origin.z) / denom;
          vec3 plane_p = ray.origin + plane_t * ray.direction;

          plane_p.xy /= 4.0;

          if (all(lessThan(abs(plane_p.xy), vec2(0.5))))
          {
              if (ray.direction.z > 0. && plane_t > 0.)
              {
                vec2 puv = plane_p.xy + 0.5;
                puv.x = 1.0 - puv.x;

                // return vec3(0.5, 0.5, 0.5) * alpha;
                vec4 tex = texture2D(iChannel0, puv);
                tex.xyz = pow(tex.xyz, vec3(1.0 / 2.2));
                if (tex.a > 0.1)
                	return tex.xyz * alpha;
              }
          }

          tp1 = float(id.x != 1 && id.x != -2) - 1.0;
          tp2 = float(id.y % 16) - 1.0;
          if (tp1 < 0. && tp2 < 0. )
          {
                return vec3(0.8,0.8,1.0) * smoothstep(6.0, 5.0, iTime.x);
          }

          tp2 = float((id.y+8) % 16) - 1.0;
          if (tp1 < 0. && tp2 < 0. )
          {
                return vec3(1.0,0.8,0.8) * smoothstep(6.0, 5.0, iTime.x);
          }

          float fre = pow(1.0-max(0.0, dot(normalize(n), normalize(ray.direction))), 3.0);
          alpha *= fre*0.9;

          vec3 pure=reflect(ray.direction, n);

          ray.direction = normalize(hash3(uv+j*74.524+i*35.712)-0.5);
          float dr = dot(ray.direction, n);
          if (dr<0.0) ray.direction = -ray.direction;

          ray.direction = normalize(mix(ray.direction, pure, rough));

          ray.origin = p;
   		}
    return col;
}

mat2 rot(float a) {
  float ca=cos(a);
  float sa=sin(a);
  return mat2(ca,sa,-sa,ca);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    int steps = 10;

    //vec2 uv = fragCoord / vec2(iResolution);
    vec2 uv = fragCoord;

    vec3 col_acc = vec3(0.0, 0.0, 0.0);

    // Path tracing
    for(int j=0; j<steps; ++j)
    {
        float time = -4.0 + 2.0 * iTime.x + 0.05 * hash2(uv + float(j));

        // camera
        float d =  5. * time - 20.0;
        vec3 ro = vec3(mix(3.0, 0.0,smoothstep(4., 6.0, time)),
                       0.0,
                       mix(d, 20.0,smoothstep(6., 7.0, time)));
        vec3 de = vec3(0.0, 0.0, 2.0) * smoothstep(1.0, 0.0, time - 3.0);

        vec3 d1 = vec3(-5.5, -4.5, 10.0);

        d1.xz = d1.xz *rot(-0.3 * smoothstep(5.0, 0.0, time));

        vec3 lookat = ro + mix(
            d1,
            vec3(0.0, 0.0, 10.0),
            smoothstep(4.0, 6., time));

        vec3 up = mix(
            vec3(-0.2f, 1.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            smoothstep(4.0, 5., time));
        up = normalize(up);


        float fov = mix(
            15.,
            30.,
            smoothstep(0.0, 6., time));

        float aperture = 0.1;

        float distToFocus = mix(
                mix(
                1.,
                10.,
                smoothstep(-2.0, 1., time)),
            30.0 - ro.z,
            smoothstep(3.0, 5., time));

        Camera camera;
        Camera_init(camera, ro, lookat, up, fov, float(iResolution.x) / float(iResolution.y), aperture, distToFocus);

        Ray ray = Camera_getRay(camera, uv, float(j));

        col_acc += trace(ray, float(j), uv, time);
   }
   col_acc /= float(steps);


  col_acc=smoothstep(0.0,1.0,col_acc);
  // col_acc=pow(col_acc, vec3(0.4545));

  col_acc = mix(
            vec3(0.0),
            col_acc,
            smoothstep(0.0, 2.5, iTime.x) * smoothstep(7.0, 6.0, iTime.x));


  fragColor = vec4(col_acc, 1);
}

void main()
{
    vec4 fragColor;
	mainImage(fragColor, v_uv);
	gl_FragColor = fragColor;
}
