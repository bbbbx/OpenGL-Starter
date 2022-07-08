#ifndef TEXTURE_QUERY_LOAD_POLYFILL_H
#define TEXTURE_QUERY_LOAD_POLYFILL_H

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_texture_query_lod.txt
// float ComputeAccessedLod(float computedLod)
// {
//   // Clamp the computed LOD according to the texture LOD clamps.
//   if (computedLod < TEXTURE_MIN_LOD) computedLod = TEXTURE_MIN_LOD;
//   if (computedLod > TEXTURE_MAX_LOD) computedLod = TEXTURE_MAX_LOD;
//   // Clamp the computed LOD to the range of accessible levels.
//   if (computedLod < 0)
//       computedLod = 0.0;
//   if (computedLod > (float)
//       maxAccessibleLevel) computedLod = (float) maxAccessibleLevel;
//   // Return a value according to the min filter.
//   if (TEXTURE_MIN_FILTER is LINEAR or NEAREST) {
//     return 0.0;
//   } else if (TEXTURE_MIN_FILTER is NEAREST_MIPMAP_NEAREST
//               or LINEAR_MIPMAP_NEAREST) {
//     return ceil(computedLod + 0.5) - 1.0;
//   } else {
//     return computedLod;
//   }
// }

float ComputeAccessedLod(float computedLod) {
  return computedLod;
}

vec2 textureQueryLod(sampler2D sampler, vec2 st) {
  vec2 widthHeight = textureSize(sampler, 0);
  vec2 uv = widthHeight * st;
  vec2 dUvDx = dFdx(uv);
  vec2 dUvDy = dFdy(uv);
  float dUdx = dUvDx.x;
  float dVdx = dUvDx.y;
  float dUdy = dUvDy.x;
  float dVdy = dUvDy.y;

  float rho = max(sqrt(dUdx*dUdx + dVdx*dVdx), sqrt(dUdy*dUdy + dVdy*dVdy));
  float lambda_prime = log2(rho);
  return vec2(ComputeAccessedLod(lambda_prime), lambda_prime);
}

vec2 textureQueryLod(sampler3D sampler, vec3 str) {
  vec3 widthHeightDepth = textureSize(sampler, 0);
  vec3 uvw = widthHeightDepth * str;
  vec3 dUvwDx = dFdx(uvw);
  vec3 dUvwDy = dFdy(uvw);

  float dUdx = dUvwDx.x;
  float dVdx = dUvwDx.y;
  float dWdx = dUvwDx.z;

  float dUdy = dUvwDy.x;
  float dVdy = dUvwDy.y;
  float dWdy = dUvwDy.z;

  float rho = max(sqrt(dUdx*dUdx + dVdx*dVdx + dWdx*dWdx), sqrt(dUdy*dUdy + dVdy*dVdy + dWdy*dWdy));
  float lambda_prime = log2(rho);
  return vec2(ComputeAccessedLod(lambda_prime), lambda_prime);
}

#endif // TEXTURE_QUERY_LOAD_POLYFILL_H
