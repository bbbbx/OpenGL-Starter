#version 410 core

#include "../DepthPrecision.h"
#include "../AtmosphericScatteringWithClouds.h"

in vec3 in_position;

uniform mat4 modelMatrix;
uniform mat4 viewProj;

uniform vec3 cameraPosition;
uniform sampler2D cloudSampler;
uniform vec3 lightDirection;

out float logZ;
out vec3 v_pos;

out vec3 positionRelCamera;
out AtmosphericScattering scattering;

void main() {
  vec3 positionWorldSpace = vec4(modelMatrix * vec4(in_position, 1.0)).xyz;

  positionRelCamera = positionWorldSpace - cameraPosition;

  vec3 positionRelPlanet = positionWorldSpace/* - planetCenter*/;

  gl_Position = viewProj * vec4(positionWorldSpace, 1.0);
  gl_Position.z = logarithmicZ_vertexShader(gl_Position.z, gl_Position.w, logZ);

  v_pos = in_position;

  vec3 cameraPositionRelPlanet = cameraPosition/* - planetCenter*/;
  scattering = calcAtmosphericScattering(cameraPositionRelPlanet, positionRelPlanet, lightDirection, cloudSampler);
}
