#version 410 core

precision highp float;
precision highp int;
precision highp uint;

// --- Remap.glsl ---
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
// --- Remap.glsl ---

// --- HenyeyGreenstein.glsl ---
// FIXME: 1/(4pi) 是 0.7853981633974483
float oneOnFourPi = 0.0795774715459;

vec4 henyeyGreenstein(float cosAngle, vec4 eccentricity) {
  vec4 eccentricity2 = eccentricity * eccentricity;
  return oneOnFourPi * ((vec4(1.0) - eccentricity2) / pow(vec4(1.0) + eccentricity2 - 2.0*eccentricity*cosAngle, vec4(3.0/2.0)));
}

float henyeyGreenstein(float cosAngle, float occentricity) {
  return henyeyGreenstein(cosAngle, vec4(occentricity)).x;
}

float watooHenyeyGreenstein(float cosAngle) {
  // Tuned to match The "Watoo" phase function for clouds, from Bouthors et al.
	// See http://wiki.nuaj.net/index.php?title=Clouds
  return dot(
    henyeyGreenstein(cosAngle, vec4(-0.5, 0.3, 0.96, 0.0)),
    vec4(0.5, 0.5, 0.03, 0.0)
  );
}
// --- HenyeyGreenstein.glsl ---

// r0: ray origin
// rd: normalized ray direction
// s0: sphere center
// sr: sphere radius
// Returns distances to intersections t0 and t1. Sets t0 to -1 if there was no intersection.
void raySphereIntersections(vec3 r0, vec3 rd, vec3 s0, float sr, out float t0, out float t1)
{
  vec3 s0_r0 = r0 - s0;
  float b = 2.0 * dot(rd, s0_r0);
  float c = dot(s0_r0, s0_r0) - (sr * sr);

	float det = b*b - 4.0*c;
  if (det < 0.0)
	{
    t0 = -1.0;
    t1 = -1.0;
    return;
  }
	float sqrtDet = sqrt(det);
  t0 = (-b - sqrtDet) * 0.5;
	t1 = (-b + sqrtDet) * 0.5;
}


// r0: ray origin
// rd: normalized ray direction
// s0: sphere center
// sr: sphere radius
// Returns distance from r0 to first intersection with sphere, or -1.0 if no intersection.
float raySphereFirstIntersection(vec3 r0, vec3 rd, vec3 s0, float sr)
{
  vec3 s0_r0 = r0 - s0;
  float b = 2.0 * dot(rd, s0_r0);
  float c = dot(s0_r0, s0_r0) - (sr * sr);
	
	float det = b*b - 4.0*c;
  if (det < 0.0)
	{
    return -1.0;
  }
  return (-b - sqrt(det)) * 0.5;
}

// r0: ray origin
// rd: normalized ray direction
// s0: sphere center
// sr: sphere radius
// Returns distance from r0 to second intersection with sphere, or -1.0 if no intersection.
float raySphereSecondIntersection(vec3 r0, vec3 rd, vec3 s0, float sr)
{
  vec3 s0_r0 = r0 - s0;
  float b = 2.0 * dot(rd, s0_r0);
  float c = dot(s0_r0, s0_r0) - (sr * sr);

	float det = b*b - 4.0*c;
  if (det < 0.0)
	{
    return -1.0;
  }
  return (-b + sqrt(det)) * 0.5;
}



// Remaps value from 0 to 1 across pixelCount pixels.
float antiAliasSmoothStep(float value, float pixelCount)
{
	float width = max(abs(dFdx(value)), abs(dFdy(value))) * pixelCount;
	return smoothstep(0.0, width, value);
}

float random(vec2 texCoord)
{
	float dotProduct = dot(texCoord, vec2(12.9898, 78.233));
	return fract(sin(dotProduct) * 43758.5453);
}


vec2 offsetCloudUvByWorldDistance(vec2 startUv, vec2 worldDistance)
{
	return startUv + vec2(worldDistance.y / 40070000.0, 0.5 * worldDistance.x / 40070000.0);
}

uniform vec2 cloudDisplacementMeters;

#define M_PI 3.1415926535897932384626433832795f
#define M_2PI 6.28318530718f
#define M_RCP_PI 0.31830988618f
#define M_RCP_2PI 0.15915494309f
vec2 cloudUvFromPosRelPlanet(vec3 positionRelPlanetPlanetAxes)
{
	vec2 pos2d = normalize(positionRelPlanetPlanetAxes.yx);
	float psi = atan(pos2d.x, pos2d.y);
	float theta = asin(normalize(positionRelPlanetPlanetAxes).z);
	vec2 uv = vec2(psi * M_RCP_2PI + 0.5, theta * M_RCP_PI + 0.5);
	return offsetCloudUvByWorldDistance(uv, cloudDisplacementMeters);
}

// From https://www.geeks3d.com/hacklab/20190225/demo-checkerboard-in-glsl/
float checker(vec2 uv, float repeats) 
{
	float cx = floor(repeats * uv.x);
	float cy = floor(repeats * uv.y); 
	float result = mod(cx + cy, 2.0);
	return sign(result);
}


uniform vec3 wrappedNoiseOrigin;
uniform float wrappedNoisePeriod;

vec3 calcWrappedNoiseCoord(vec3 position)
{
	return (position - wrappedNoiseOrigin) / wrappedNoisePeriod;
}



uniform vec3 cameraPosition;

uniform vec3 planetCenter;
uniform mat4 planetMatrixInv;
uniform float innerRadius;

uniform vec3 lightDirection;
uniform vec3 ambientLightColor;


in vec3 vertCameraWorldDir;
in float cameraAltitude;
in vec2 v_uv;

out vec4 colorOut;

// Cloud geometry heights for each cloud type
const float cloudLayerMaxHeight = 7000;
const float cloudLayerMinHeight = 2000;
const vec2 cloudTopZeroDensityHeight = vec2(2500, 7000);
const vec2 cloudBottomZeroDensity = vec2(2000, 2000);

const vec2 cloudOcclusionStrength = vec2(0.25, 0.5);
const vec2 cloudDensityMultiplier = vec2(0.005, 0.02);

const vec3 noiseFrequencyScale = vec3(0.00015);

const int iterations = 1000;
const float initialStepSize = 100;
const float maximumStepSize = 500;
const float stepSizeGrowthFactor = 1.002;
const float maxRenderDistance = 300000;

uniform sampler2D globalAlphaSampler;
uniform sampler2D coverageDetailSampler;
uniform sampler3D noiseVolumeSampler;

float mipMapLevel(sampler2D sampler, vec2 uv) {
  vec2 texCoord = uv * textureSize(sampler, 0);
  vec2 dtc_dx = dFdx(texCoord);
  vec2 dtc_dy = dFdy(texCoord);

  // Take the average of the x and y dimensions.
  // This provides a balance between sharpness and aliasing at grazing angles.
  float deltaMaxSqr = 0.5 * (dot(dtc_dx, dtc_dx) + dot(dtc_dy, dtc_dy));
  return 0.5 * log2(max(1.0, deltaMaxSqr));
}



float texelsPerPixelAtDistance(float distance)
{
	return distance/120000.0;
}

const vec3 RANDOM_VECTORS[6] = vec3[6](
  vec3( 0.38051305f,  0.92453449f, -0.02111345f),
	vec3(-0.50625799f, -0.03590792f, -0.86163418f),
	vec3(-0.32509218f, -0.94557439f,  0.01428793f),
	vec3( 0.09026238f, -0.27376545f,  0.95755165f),
	vec3( 0.28128598f,  0.42443639f, -0.86065785f),
	vec3(-0.16852403f,  0.14748697f,  0.97460106f)
);
const float SAMPLE_DISTANCES[6] = float[6](1, 2, 4, 8, 16, 32);
const float SAMPLE_SEGMENT_LENGTHS[6] = float[6](1, 1, 2, 4, 8, 16);

const int lightSampleCount = 6;
const vec3 albedo = vec3(0.95);

const float powderStrength = 0.1; // TODO: get this looking right
const float scatterSampleDistanceScale = 25;
const float scatterDistanceMultiplier = 0.5; // less then 1 to fake multi-scattering


uniform float cloudCoverageFraction;

float sampleBaseCloudCoverage(sampler2D cloudSampler, vec2 uv) {
#ifdef USE_CLOUD_COVERAGE_MAP
  float c = clamp(
    textureLod(cloudSampler, uv, 0).r * 1.3 - 0.001,
    0.0,
    1.0);
  return pow(c, 0.45); // apply fudge factor so that high detail cloud coverage matches map
#else
  return cloudCoverageFraction;
#endif
}

float sampleCoverageDetail(sampler2D coverageDetailSampler, vec2 uv, float lod, out float cloudType) {
  vec4 c = textureLod(coverageDetailSampler, uv.xy * vec2(300.0, 150.0), lod);
  float coverageDetail = c.r;
  cloudType = c.b*c.b; // 蓝色通道保存云的类型的平方根
  return coverageDetail;
}

// Returns an apprxomate semicircle over x in range [0, 1]
float semicircle(float x) {
  float a = clamp(x * 2.0 - 1.0, -1.0, 1.0);
  return 1 - a*a;
}

float biasedSemiCircle(float x, float bias) {
  float a = pow((bias - x) / (1.0 - bias), 3.0);
  float b = pow((x - bias) / (bias), 3.0);
  return 1.0 + (x > bias ? a : b);
}

// 根据云的类型计算该云在该高度下的密度倍数，[0, 1]
float calcHeightMultiplier(float height, float cloudType) {
  // 总共两种云的类型
  // cloudBottomZeroDensity 保存两种云类型在底部密度为 0 时的高度
  // cloudTopZeroDensityHeight 保存两种云类型在顶部密度为 0 时的高度
  float bottom = mix(cloudBottomZeroDensity.x, cloudBottomZeroDensity.y, cloudType);
  float top = mix(cloudTopZeroDensityHeight.x, cloudTopZeroDensityHeight.y, cloudType);
  float h = remapNormalized(height, bottom, top);
  // return semicircle(h);
  return biasedSemiCircle(h, 0.2);
}

float coverageModulation(float coverage, float detail, float filterWidth) {
  float f = 1.0 - coverage;
  return clamp(remapNormalized(detail*(1.0 - filterWidth) + filterWidth, f, f + filterWidth), 0.0, 1.0);
}

vec2 coverageModulation(vec2 coverage, vec2 detail, vec2 filterWidth) {
  vec2 f = vec2(1.0) - coverage;
  return clamp(remapNormalized(detail*(vec2(1.0) - filterWidth) + filterWidth, f, f + filterWidth), vec2(0.0), vec2(1.0));
}

float cloudCoverageModulationFilterWidth = 0.6;

float calcCloudDensityHull(float coverageBase, float coverageDetail, float heightMultiplier) {
  float coverage = coverageBase * heightMultiplier;
  return coverageModulation(coverage, coverageDetail, cloudCoverageModulationFilterWidth);
}

vec2 calcCloudDensityLowRes(float coverageBase, float coverageDetail, float heightMultiplier) {
  vec2 coverage = vec2(coverageBase * heightMultiplier);

  // 用高频的纹理腐蚀低频的密度，以减少云的覆盖度
  // Eroding the low detail density by high detail textures results in reduction of cloud coverage.
	// We need to subtract this amount from the low detail coverage to match the high detail result.
  float erosionCompensation = 0.14;
  coverage.x = max(0.0, coverage.x - erosionCompensation);

  return coverageModulation(coverage, vec2(coverageDetail), vec2(cloudCoverageModulationFilterWidth));
}

vec2 sampleDensityLowRes(vec2 uv, float height, out float cloudType, float lod) {
  // 采样全球的覆盖率作为云的低频覆盖率
  float coverageBase =  sampleBaseCloudCoverage(globalAlphaSampler, uv);
  // 
  float coverageDetail = sampleCoverageDetail(coverageDetailSampler, uv, lod, cloudType);
  cloudType *= coverageBase; // aesthetic hack
  float heightMultiplier = calcHeightMultiplier(height, cloudType);
  // return calcCloudDensityHull(coverageBase, coverageDetail, heightMultiplier);
  return calcCloudDensityLowRes(coverageBase, coverageDetail, heightMultiplier);
}

const float cloudChaos = 0.8;

// !@param lod is 0 for maximum detail. Detail falls off with log2 (as per mipmap lod level)
float calcDensity(vec3 pos, vec2 uv, float lod, float height, out float cloudType) {
  vec2 densityAtLods = sampleDensityLowRes(uv, height, cloudType, lod);
  float density = densityAtLods.r;

  // Apply detail
  float detailBlend = lod * 0.3;
  if (detailBlend < 1.0) {
    float noise = textureLod(noiseVolumeSampler, pos * noiseFrequencyScale, 0).r;
    float filteredNoise = min(0.9, cloudChaos * noise);
    float highDensity = clamp(remapNormalized(densityAtLods.g, filteredNoise, 1.0), 0.0, 1.0);
    density = mix(density, highDensity, 1.0 - detailBlend);

#ifdef ENABLE_HIGH_DETAIL_CLOUDS
    float highDetailBlend = lod * 10.0;
    if (highDetailBlend < 1.0) {
      noise = 0.9 * textureLod(noiseVolumeSampler, pos * noiseFrequencyScale * 3.0, 0).r;
      float highResDensity = clamp(remapNormalized(density, noise, 1.0), 0.0, 1.0);
      density = mix(density, highDensity, 1.0 - highDetailBlend);
    }
#endif
  }

  return density * mix(cloudDensityMultiplier.x, cloudDensityMultiplier.y, cloudType);
}

// 计算低频云发出的 radiance？
vec3 radianceLowRes(vec3 pos, vec2 uv, vec3 lightDir, float hg, vec3 sunIrradiance, float lod, float height, float cloudType) {
  float Texponent = 0.0;
  float powderTexponent = 0.0;

  for (int i = 0; i < lightSampleCount; ++i) {
    float scatterDistance = SAMPLE_SEGMENT_LENGTHS[i] * scatterSampleDistanceScale;

    vec3 lightSamplePos = pos + (lightDir + RANDOM_VECTORS[i] * 0.3) * SAMPLE_DISTANCES[i] * scatterSampleDistanceScale;
    float sampleHeight = length(lightSamplePos) - innerRadius;
    vec2 sampleUv = cloudUvFromPosRelPlanet(lightSamplePos);
    float sampleOutCloudType;
    float density = calcDensity(lightSamplePos, sampleUv, lod, sampleHeight, sampleOutCloudType);
    Texponent -= density * scatterDistance * scatterDistanceMultiplier;
    powderTexponent -= density * scatterDistance * 2.0;
  }

  float T = exp(Texponent);
  float powderT = exp(powderTexponent);

  vec2 occlusionByType = clamp(
    remap(vec2(height), cloudBottomZeroDensity, cloudTopZeroDensityHeight, vec2(0.0), vec2(1.0)),
    vec2(0.0),
    vec2(1.0));
  occlusionByType = occlusionByType * cloudOcclusionStrength + (1.0 - cloudOcclusionStrength);
  float occlusion = mix(occlusionByType.x, occlusionByType.y, cloudType);

  vec3 radiance = vec3(0.0);
  for (int N = 0; N < 3; ++N) { // multi scattering octaves
    float albedoMScat = pow(0.5, N);
    float extinctionMScat = albedoMScat;
    float hgMScat = albedoMScat;

    float beer = pow(T, extinctionMScat);
    float powder = 1.0 - powderStrength * pow(powderT, extinctionMScat);

    radiance += occlusion * sunIrradiance * albedo * albedoMScat * beer * powder * mix(oneOnFourPi, hg, hgMScat);

  }
  radiance += ambientLightColor;
  return radiance;
}



float effectiveZeroT = 0.01;
float effectiveZeroDensity = 0.00001;

vec4 march(vec3 start, vec3 dir, float stepSize, float tMax, float rayStartTexelsPerPixel, vec3 lightDir, vec3 sunIrradiance, out float meanCloudFrontDistance) {
  float cosAngle = dot(lightDir, dir);
  float hg = watooHenyeyGreenstein(cosAngle);

  // The color that is accumulated during raymarching
  vec3 totalIrradiance = vec3(0.0);
  float T = 1.0;

  // As we march, also calculate the transmittance-weigthed mean cloud scene depth.
	// Based on "Physically Based Sky, Atmosphere and Cloud Rendering in Frostbite"
	// section 5.9.1 Aerial perspective affecting clouds
	// https://media.contentapi.ea.com/content/dam/eacom/frostbite/files/s2016-pbs-frostbite-sky-clouds-new.pdf
  float sumTransmissionWeightedDistance = 0.0;
	float sumTransmissionWeights = 0.0;

  // Do the actual raymarching
  float t = 0.0;
  bool lastStep = false;
  for (int i = 0; i < iterations; i++) {
    vec3 pos = start + dir * t;
    float lod = log2(max(1.0, rayStartTexelsPerPixel + texelsPerPixelAtDistance(t)));

    vec2 uv = cloudUvFromPosRelPlanet(pos);
    float height = length(pos) - innerRadius;
    float outCloudType;

    // 根据云的坐标、uv、LOD、高度，计算云的密度，并输出云的类型
    float density = calcDensity(pos + vec3(cloudDisplacementMeters.xy, 0.0), uv, lod, height, outCloudType);

    // 如果云的密度大于 0（有效密度）
    if (density > effectiveZeroDensity) {
      vec3 radiance = radianceLowRes(pos, uv, lightDir, hg, sunIrradiance, lod, height, outCloudType) * density;

#ifdef ENERGY_CONSERVING_INTEGRATION

    // totalIrradiance += T * integScatt;
#else
      totalIrradiance += radiance * stepSize * T;
      T *= exp(-density * stepSize);
#endif

      sumTransmissionWeightedDistance += T * t;
      sumTransmissionWeights += T;
    } else {
      // 否则认为该坐标为空，没有云
      // Skip empty space
      discard;
    }


    // early ray termination
    if (T <= effectiveZeroT) break;

    if (lastStep) {
      break;
    }

    stepSize *= stepSizeGrowthFactor;
    t += stepSize;
    if (t > tMax) {
      t = tMax;
      lastStep = true;
    }
  }

  meanCloudFrontDistance = (sumTransmissionWeights > 0) ? (sumTransmissionWeightedDistance / sumTransmissionWeights) : -1.0;

  return vec4(totalIrradiance, 1.0 - max(0.0, remapNormalized(T, effectiveZeroT, 1.0)));
}

// Simulates colour change duet to scattering
vec3 desaturate(vec3 c) {
  return vec3(length(c));
}

const float lowResBrightnessMultiplier = 1.0; // fudge factor to make low res clouds match brightness of high res clouds, nneded because low res does not simulate any scattering

vec4 evaluateGlobalLowResColor(vec2 cloudsUv, vec3 irradiance, float rayFar) {
  // textureQueryLod：4.0 才有的函数，
  // y 分量是计算出来的相对于 base level 的 level，
  // x 分量是实际将要访问 level。
  // Calculate texture LOD level.
	// There is a singularity when the U coordinate wraps from 1 to 0, which we avoid by querying LOD
	// at U and fract(U) and taking the minimum. We also tried Tarini's method (https://www.shadertoy.com/view/7sSGWG) which alleviated the seam but did not completely remove it.
  vec2 lod2d = textureQueryLod(globalAlphaSampler, cloudsUv.xy);
  float lod = max(lod2d.x, lod2d.y);
  lod2d = textureQueryLod(globalAlphaSampler, vec2(fract(cloudsUv.x), 0.0));
  lod = min(lod, max(lod2d.x, lod2d.y));

  float alpha = textureLod(globalAlphaSampler, cloudsUv, lod).r;
  vec3 color = irradiance * alpha * oneOnFourPi;
  color = mix(color, desaturate(color), 0.15); // desaturate color to simulate scattering. This makes sunsets less extreme.
  return vec4(color, alpha);
}

void main() {

  vec2 uv = v_uv;
  vec3 rayDir = normalize(vertCameraWorldDir);
  // colorOut = vec4(rayDir, 1.0);
  // return;

  float rayNear;
  float rayFar;

  bool hitPlanet = (raySphereFirstIntersection(cameraPosition, rayDir, vec3(0.0), 6600000) >= 0.0);
  // if (hitPlanet) {
  //   colorOut = vec4(normalize(cameraPosition), 1.0);
  // } else {
  //   colorOut = vec4(vec3(0.0), 1.0);
  // }
  // return;

  float alpha = 1.0;

  // 相机当前海拔小于云层最底部时
  if (cameraAltitude < cloudLayerMinHeight) {
    if (hitPlanet) {
      colorOut = vec4(0.0);
      // depthOut = 1.0;
      return;
    }

    rayNear = raySphereSecondIntersection(cameraPosition, rayDir, planetCenter, innerRadius + cloudLayerMinHeight);
    rayFar = raySphereSecondIntersection(cameraPosition, rayDir, planetCenter, innerRadius + cloudLayerMaxHeight);
    rayFar = min(rayFar, maxRenderDistance);
  } else if (cameraAltitude < cloudLayerMaxHeight) { // 相机海拔在云层内部时
    rayNear = 0.0;
    if (hitPlanet) {
      // Far is the intersection with the cloud base
      // 相机看向地面时，远处交点位于云层底部上
      rayFar = raySphereFirstIntersection(cameraPosition, rayDir, planetCenter, innerRadius + cloudLayerMinHeight);
    } else {
      // Far is the intersection with the cloud top
      // 相机看向天空时，远处交点位于云层顶部上
			rayFar = raySphereSecondIntersection(cameraPosition, rayDir, planetCenter, innerRadius + cloudLayerMaxHeight);
    }
  } else { // 相机海拔高于云层顶部时
    float t0;
    float t1;
    raySphereIntersections(cameraPosition, rayDir, planetCenter, innerRadius + cloudLayerMaxHeight, t0, t1);
    rayNear = t0;

    if (hitPlanet) {
      // Far is the intersection with the cloud base
			rayFar = raySphereFirstIntersection(cameraPosition, rayDir, planetCenter, innerRadius + cloudLayerMinHeight);
    } else {
      // Far is the intersection with the cloud top
			rayFar = t1;
    }

    // Fade out clouds towards horizon a few pixels to fake antialiasing on rim of planet
		vec3 midPoint = cameraPosition + rayDir * (t0 + t1) * 0.5;
		float alt = distance(planetCenter, midPoint);
		
		float signal = (innerRadius + cloudLayerMaxHeight - alt); // signal is 0 at horizon and positive below horizon
		alpha = antiAliasSmoothStep(signal, 5.0);
  }

  // 射向没有和云层相交
  if (rayNear < 0.0) {
    colorOut = vec4(0.0);
    // depthOut = 1.0;
    return;
  }

  float stepSize = initialStepSize;
	stepSize = min(stepSize + rayNear / 4000.0, maximumStepSize);

  rayNear += stepSize * random(gl_FragCoord.xy);

  vec3 positionRelCameraWS = rayNear * rayDir;

  // 相机位置（相对于星球中心）
  vec3 cameraPositionRelPlanet = cameraPosition - planetCenter;
  // 射线-球体的相交点（相对于星球中心）
	vec3 positionRelPlanet = positionRelCameraWS + cameraPositionRelPlanet;
  // 射线-球体的相交点（星球坐标系下）
	vec3 positionRelPlanetPlanetAxes = mat3(planetMatrixInv) * positionRelPlanet;
  // 射线方向（星球坐标系下）
	vec3 rayDirPlanetAxes = mat3(planetMatrixInv) * rayDir;
  // 光照方向（星球坐标系下）
	vec3 lightDirPlanetAxes = mat3(planetMatrixInv) * lightDirection;
  // 把射线-球体的相交点往上抬一点，防止因精度问题导致求得的相交点位于球体内？
	vec3 positionRelPlanetSafe = positionRelPlanet*1.0003;


  // vec3 skyIrradiance;
	// vec3 sunIrradiance = GetSunAndSkyIrradiance(positionRelPlanetSafe, lightDirection, skyIrradiance);
  // // Add sky radiance to sun radiance, to simulate multi scattering
	// vec3 directIrradiance = (sunIrradiance + skyIrradiance);
  vec3 directIrradiance = vec3(1.0);

  vec2 cloudsUv = cloudUvFromPosRelPlanet(positionRelPlanetPlanetAxes);

  // 根据 dFd{x,y} 确定每个像素使用的 3D 纹理层级
  float lod = mipMapLevel(coverageDetailSampler, cloudsUv.xy * vec2(300.0, 150.0));
  lod = mix(0.0, lod, min(1.0, 0.2 * cameraAltitude / cloudLayerMaxHeight));

  float lowResBlend = (lod - 2.0) * 0.25;

  float meanCloudFrontDistance;  // negative value means no samples
  if (lowResBlend < 1.0) {
    // colorOut = vec4(0, 1, 0, 1);return;
    // 该 lod 下纹素的 size
    float rayStartTexelsPerPixel = pow(2.0, lod);
    // float cosAngle = dot(lightDirPlanetAxes, rayDirPlanetAxes);
    // float hg = watooHenyeyGreenstein(cosAngle);
    // colorOut = vec4(vec3(hg), 1);return;
    colorOut = march(
      positionRelPlanetPlanetAxes,
      rayDirPlanetAxes,
      stepSize,
      rayFar - rayNear,
      rayStartTexelsPerPixel,
      lightDirPlanetAxes,
      directIrradiance,
      meanCloudFrontDistance
    );
    meanCloudFrontDistance += rayNear;
  } else {
    // colorOut = vec4(1.0, 0, 0, 1);return;
    meanCloudFrontDistance = -1.0;
  }

  bool hasSample = (meanCloudFrontDistance >= 0.0);

  float logZ;
  if (hasSample) {
  } else {
    float precisionBias = 0.99; // need to avoid z fighting at extrame distances
    // logZ = calcLogZDdc
  }

  if (lowResBlend > 0.0) {
    vec4 lowResColor = evaluateGlobalLowResColor(cloudsUv, directIrradiance, rayFar) * lowResBrightnessMultiplier;
    colorOut = mix(colorOut, vec4(lowResColor), min(1.0, lowResBlend));
  }

  colorOut *= alpha;
  colorOut.rgba = sqrt(colorOut.rgba);
  // depthOut = logZ;

	// colorOut.rgb = vec3(checker(cloudsUv*vec2(2,1), 100.0));
	// colorOut.rgb = normalize(positionRelPlanetPlanetAxes);
	// colorOut.rgb = vec3(cloudsUv, 0.0);
	// colorOut.rgb = texture(coverageDetailSampler, cloudsUv).rgb;
	// colorOut.rgb = vec3(lod / 10.0);

  // colorOut.a = 1.0;

  // colorOut = vec4(0);
  // if (hitPlanet) {
  //   colorOut.rgb = rayDir;
  // }
}
