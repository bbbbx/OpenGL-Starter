#version 410 core

#include "../DepthPrecision.h"

uniform vec4 color;

in float logZ;
in vec3 v_normal;

out vec4 outColor;

void main() {
  outColor = color;

  float light = dot(v_normal, vec3(1.0, 1.0, 0));
  outColor.rgb *= light;

  gl_FragDepth = logarithmicZ_fragmentShader(logZ);

  // outColor.rgb = vec3(gl_FragDepth);
}
