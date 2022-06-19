#version 410 core

uniform vec3 planetCenter;
uniform mat4 planetMatrixInv;
uniform float innerRadius;

uniform vec3 wrappedNoiseOrigin;
uniform float wrappedNoisePeriod;

vec3 calcWrappedNoiseCoord(vec3 position)
{
	return (position - wrappedNoiseOrigin) / wrappedNoisePeriod;
}
