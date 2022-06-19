#version 410 core

precision highp float;
precision highp int;
precision highp uint;

// vec2 positions[3] = vec2[3](
//   vec2(-30.0, -30.),
//   vec2( 30.0, -30.),
//   vec2( 0.0,  30.)
// );
vec2 positions[6] = vec2[6](
  // vec2(-1.0, -1.),
  // vec2( 3.0, -1.),
  // vec2(-1.0,  3.)

  vec2(-1.0,  -1.0),
  vec2( 1.0,  -1.0),
  vec2(-1.0,   1.0),
  vec2( 1.0,  -1.0),
  vec2(-1.0,   1.0),
  vec2( 1.0,   1.0)
);

uniform vec3 planetCenter;
uniform mat4 planetMatrixInv;
uniform float innerRadius;

uniform vec3 cameraPosition;
uniform vec3 topLeftDir;
uniform vec3 topRightDir;
uniform vec3 bottomLeftDir;
uniform vec3 bottomRightDir;

// uniform mat4 viewProj;

out vec3 vertCameraWorldDir;
out float cameraAltitude;

out vec2 v_uv;

void main() {
  vec4 pos = vec4(positions[gl_VertexID], 0.0, 1.0);
  // gl_Position = viewProj * pos;
  gl_Position = pos;

  v_uv = pos.xy * 0.5 + vec2(0.5);

  vec3 topDir = mix(topLeftDir, topRightDir, v_uv.x);
  vec3 bottomDir = mix(bottomLeftDir, bottomRightDir, v_uv.x);
  vertCameraWorldDir = mix(bottomDir, topDir, v_uv.y);

  cameraAltitude = length(cameraPosition - planetCenter) - innerRadius;
}