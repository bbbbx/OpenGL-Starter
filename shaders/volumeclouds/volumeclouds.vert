#version 410 core

// uniform vec3 planetCenter;
// uniform mat4 planetMatrixInv;
// uniform float innerRadius;
#include "../GlobalUniforms.h"

uniform vec3 cameraPosition;
uniform vec3 topLeftDir;
uniform vec3 topRightDir;
uniform vec3 bottomLeftDir;
uniform vec3 bottomRightDir;


in vec4 in_position;
in vec2 in_texCoord;

out vec3 vertCameraWorldDir;
out float cameraAltitude;
out vec2 v_uv;

void main() {
  gl_Position = in_position * vec4(2.0) - vec4(1.0);

  v_uv = in_texCoord;

  vec3 topDir = mix(topLeftDir, topRightDir, v_uv.x);
  vec3 bottomDir = mix(bottomLeftDir, bottomRightDir, v_uv.x);
  vertCameraWorldDir = mix(bottomDir, topDir, v_uv.y);

  cameraAltitude = length(cameraPosition - planetCenter) - innerRadius;
}