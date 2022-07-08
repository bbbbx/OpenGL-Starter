#version 410 core

in vec4 in_position;
in vec2 in_texCoord;

out vec2 texCoord;

void main() {
  gl_Position = in_position * vec4(2.0) - vec4(1.0);
  texCoord = in_texCoord;
  // texCoord = in_position.xy;
}