#version 410 core
precision highp float;

in vec3 color;
in float logZ;
out vec4 outColor;

uniform float farClipDistance;
float C = 0.5;

float calcLogZDdc(float clipSpaceW)
{
  return log(C*clipSpaceW + 1) / log(C*farClipDistance + 1);
}
#define ACCURATE_LOG_Z
float logarithmicZ_fragmentShader(float z)
{
#ifdef ACCURATE_LOG_Z
  return log(z) / log(C*farClipDistance + 1.0);
#else
  return z;
#endif
}

void main() {
  outColor = vec4( color, 1.0 );

  gl_FragDepth = logarithmicZ_fragmentShader(logZ);

  // outColor.rgb = vec3(gl_FragDepth);
}
