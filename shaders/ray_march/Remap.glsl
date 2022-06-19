#version 410 core
precision highp float;

float remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax) {
  // return mix(originalValue, originalMin, originalMax) * (newMax - newMin) + newMin;
  return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

vec2 remap(vec2 originalValue, vec2 originalMin, vec2 originalMax, vec2 newMin, vec2 newMax) {
  // return mix(originalValue, originalMin, originalMax) * (newMax - newMin) + newMin;
  return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

float remapNormalized(float originalValue, float originalMin, float originalMax) {
  return (originalValue - originalMin) / (originalMax - originalMin);
}

vec2 remapNormalized(vec2 originalValue, vec2 originalMin, vec2 originalMax) {
  return (originalValue - originalMin) / (originalMax - originalMin);
}

vec3 remapNormalized(vec3 originalValue, vec3 originalMin, vec3 originalMax) {
  return (originalValue - originalMin) / (originalMax - originalMin);
}
