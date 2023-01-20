#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec3 seed_color;
out vec4 frag_color;

void main() {
  frag_color = vec4(seed_color, 1.0);
}
