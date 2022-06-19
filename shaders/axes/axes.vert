#version 410 core
precision highp float;

in vec3 in_position;
in vec3 in_color;

uniform mat4 modelMatrix;
uniform mat4 viewProj;

out vec3 color;
out float logZ;

uniform float farClipDistance;
float C = 0.5;

float calcLogZDdc(float clipSpaceW)
{
  return log(C*clipSpaceW + 1.0) / log(C*farClipDistance + 1.0);
}

#define ACCURATE_LOG_Z
float logarithmicZ_vertexShader(float clipSpcaeZ, float clipSpaceW, out float zForFrag)
{
#ifdef ACCURATE_LOG_Z
  zForFrag = C*clipSpaceW + 1;
  return clipSpcaeZ;
#else
  zForFrag = calcLogZDdc(clipSpaceW);
  return (2.0*zForFrag - 1.0) * clipSpaceW;
#endif
}

void main() {
  gl_Position = viewProj * modelMatrix * vec4(in_position, 1.0);
  gl_Position.z = logarithmicZ_vertexShader(gl_Position.z, gl_Position.w, logZ);

  color = in_color;
}
