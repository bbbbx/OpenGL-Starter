#version 410 core

in vec2 v_uv;

uniform vec4 u_color;

out vec4 outColor;

void main() {
  outColor = u_color;
}
