#version 410 core

#include "../DepthPrecision.h"
#include "../ToneMapping.h"


#define RADIANCE_API_ENABLED
// #include "../ThirdParty/BrunetonAtmosphere/AtmospherePublic.glsl"
#include "./a.glsl"
#include "../Brdfs/HenyeyGreenstein.h"

uniform vec3 camera;
uniform vec3 sun_direction;
const float kSunAngularRadius = 0.00935 / 2.0;
vec2 sun_size = vec2(tan(kSunAngularRadius), cos(kSunAngularRadius));

in float logZ;
in vec3 view_ray;


out vec4 colorOut;

void main() {
  colorOut = vec4(1);
  gl_FragDepth = logarithmicZ_fragmentShader(logZ);

  vec3 view_direction = normalize(view_ray);
  // colorOut.rgb = view_direction;
  // colorOut = texture(scattering_texture, view_direction);
  // colorOut = texture(transmittance_texture, view_direction.xy);
  // colorOut = texture(irradiance_texture, view_direction.xy);
  // return;

  vec3 earth_center = vec3(0);
  float kLengthUnitInMeters = 1000.0;

  float shadow_length = 0.0;
  vec3 transmittance = vec3(0.0);
  vec3 radiance = GetSkyRadiance(
    (camera - earth_center) / kLengthUnitInMeters, view_direction, shadow_length, sun_direction,
    transmittance);

  // radiance += vec3( 0.1 );

  // if (dot(view_direction, sun_direction) > sun_size.y) {
  //   radiance = radiance + transmittance * GetSolarRadiance();
  //   radiance = transmittance * GetSolarRadiance();
  // }
  // fake sun, just a hack
  float cosAngle = dot(view_direction, sun_direction);
  radiance += (henyeyGreenstein(cosAngle, 0.998) * 0.03) * transmittance;

  float exposure = 5.0;
  vec3 white_point = vec3(1);
  // colorOut.rgb = pow(vec3(1.0) - exp(-radiance / white_point * exposure), vec3(1.0 / 2.2));
  colorOut.rgb = radiance;
  colorOut.a = 1.0;

  float magnitude = length(colorOut.rgb * vec3(255));
  // 如果天空颜色较黑，则设为半透明，和天空盒混合。
  colorOut.a = mix(0.0, 1.0, magnitude / 10.0);
}