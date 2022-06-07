#version 410 core

vec2 positions[3] = vec2[3](
  vec2(-0.5, -0.5),
  vec2( 0.5, -0.5),
  vec2( 0.0,  0.5)
);

out vec2 v_uv;

void main() {
  vec2 pos = positions[gl_VertexID];
  gl_Position = vec4(pos, 0.0, 1.0);
  v_uv = pos;
}
