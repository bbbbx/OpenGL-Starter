#version 410 core

in vec2 v_uv;

out vec4 outColor;

void main() {
  outColor = vec4(v_uv*2.0 + vec2(1.0), 0.0, 1.0);
}
