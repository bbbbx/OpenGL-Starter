#version 410 core

#include "../DepthPrecision.h"

in vec4 in_position;

uniform mat4 viewProj;
uniform vec3 camera;

// out vec2 uv;
out float logZ;
out vec3 view_ray;

void main() {
  gl_Position = viewProj * in_position;
  gl_Position.z = logarithmicZ_vertexShader(gl_Position.z, gl_Position.w, logZ);

  vec3 outerPositionWC = in_position.xyz;
  view_ray = outerPositionWC - camera;
}