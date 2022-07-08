#version 410 core

#include "../DepthPrecision.h"
#include "../Clouds.h"
#include "../ToneMapping.h"
#include "../Utils/Srgb.h"
#include "../AtmosphericScatteringWithClouds.h"

uniform vec4 color;
uniform sampler2D u_blueEarthSampler;

in float logZ;
in vec3 v_pos;
in vec3 positionRelCamera;
in AtmosphericScattering scattering;

out vec4 colorOut;

#define PI 3.141592653589793
#define TWO_PI 6.283185307179586

vec2 getSphereUv(vec3 position) {
  position = normalize( position );
  float phi = acos( position.z );
  float theta = asin( position.y / sin( phi ) );

  return vec2( theta / TWO_PI, phi / PI );
}

void main() {
  // colorOut = color;
  colorOut = vec4(1);

  vec3 normal = normalize( v_pos );

  // colorOut.rgb = vec3(gl_FragDepth);
  // colorOut = vec4(getSphereUv( normal ), 0, 1);
  // colorOut = vec4( cloudUvFromPosRelPlanet( v_pos ), 0, 1);
  vec2 uv = cloudUvFromPosRelPlanet( v_pos );
  colorOut = texture( u_blueEarthSampler, uv );
  // colorOut.rgb = linearToSrgb(colorOut.rgb);

  // vec3 totalReflectance = colorOut.rgb;
  // colorOut.rgb = totalReflectance * scattering.transmittance + scattering.skyRadianceToPoint;

  // float light = dot(normal, vec3(1.0, 1.0, 0));
  // colorOut.rgb *= light;
  // colorOut = vec4( uv, 0, 1 );
  // colorOut.rgb = vec3( gl_FragDepth );

  // colorOut.rgb = toneMap(colorOut.rgb);
  colorOut.rgb = srgbToLinear(colorOut.rgb);
  // colorOut.rgb = clamp(colorOut.rgb, 0.0, 1.0);
  // colorOut.rgb = pow(colorOut.rgb, vec3(2.4));

  gl_FragDepth = logarithmicZ_fragmentShader(logZ);
}
