#version 410 core

#include "../DepthPrecision.h"

in vec3 in_position;
in vec3 in_color;

uniform mat4 modelMatrix;
uniform mat4 viewProj;

out vec3 color;
out float logZ;

void main() {
  gl_Position = viewProj * modelMatrix * vec4(in_position, 1.0);
  gl_Position.z = logarithmicZ_vertexShader(gl_Position.z, gl_Position.w, logZ);

  color = in_color;
}
