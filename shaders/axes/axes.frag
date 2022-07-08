#version 410 core

#include "../DepthPrecision.h"

in vec3 color;
in float logZ;
out vec4 outColor;

void main() {
  outColor = vec4( color, 1.0 );

  gl_FragDepth = logarithmicZ_fragmentShader(logZ);

  // outColor.rgb = vec3(gl_FragDepth);
}
