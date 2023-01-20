#version 300 es

#ifdef GL_ES
precision highp float;
#endif

#define PI 3.141592653589793

layout (location = 0) in vec2 a_seed;
layout (location = 1) in float a_color;

out vec2 seed_coord;
out vec3 seed_color;

uniform vec2 u_resolution;
uniform int u_trig_count;

vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsv2rgb(vec3 c) {
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  vec3 uv;
  int id = gl_VertexID % (3 * u_trig_count);
  
  float angle0 = float(id / 3) / float(u_trig_count) * 2.0 * PI;
  float angle1 = angle0 + 1.0 / float(u_trig_count) * 2.0 * PI;
  
  switch (id % 3) {
    case 0: uv = vec3(0.0); break;
    case 1: uv = vec3(cos(angle0), sin(angle0), 1.0); break;
    case 2: uv = vec3(cos(angle1), sin(angle1), 1.0); break;
  }

  gl_Position = vec4((uv.xy) * 2.0 / u_resolution.xy * length(u_resolution.xy) + a_seed * 2.0 - 1.0, uv.z, 1.0);

  vec3 hsv = vec3(a_color, 0.6, 1.0);
  vec3 rgb = hsv2rgb(hsv);

  seed_color = mix(rgb, vec3(0.0), uv.z * 4.0);
}
