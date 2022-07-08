#version 410 core

#include "../DepthPrecision.h"

in vec3 in_position;

uniform mat4 modelMatrix;
uniform mat4 viewProj;

out float logZ;
out vec3 v_normal;

void main() {
  gl_Position = viewProj * modelMatrix * vec4(in_position, 1.0);
  gl_Position.z = logarithmicZ_vertexShader(gl_Position.z, gl_Position.w, logZ);

  v_normal = normalize(in_position);
}
