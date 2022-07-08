#version 410 core

#include "../ToneMapping.h"

in vec2 texCoord;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform int DEBUG;

out vec4 color;

void main() {
  color = texture( colorTexture, texCoord.xy ); 
  float depth = texture( depthTexture, texCoord.xy ).r;
  gl_FragDepth = depth;

  // Square the color value stored in the texture to undo the sqrt that was applied when the texture was written to.
  // This gets us back into linear color space.
  color.rgba *= color.rgba;

  // color.rgb = toneMap(color.rgb);
}
