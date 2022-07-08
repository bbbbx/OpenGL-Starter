#version 410 core

#include "ToneMapping.h"

in vec2 texCoord;

uniform sampler2D inputSampler;

out vec4 color;

void main() {
    color = texture(inputSampler, texCoord.xy);
    color.rgb = toneMap(color.rgb);
    gl_FragDepth = 0.0;
}
