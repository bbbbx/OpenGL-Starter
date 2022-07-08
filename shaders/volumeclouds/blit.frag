#version 410 core

#include "../ToneMapping.h"

in vec2 texCoord;

uniform sampler2D u_texture;

out vec4 colorOut;

void main() {
  colorOut = texture( u_texture, texCoord );
  // colorOut.rgb = toneMap(colorOut.rgb);
  // float exposure = 1.0;
  // colorOut.rgb = pow(vec3(1.0) - exp(-colorOut.rgb * exposure), vec3(1.0 / 2.2));
  gl_FragDepth = 0.0;
  // colorOut = vec4( texCoord, 0, 1 );
}
