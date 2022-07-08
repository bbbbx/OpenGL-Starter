#ifndef __VOLUME_CLOUDS_H__
#define __VOLUME_CLOUDS_H__

#include <vector>

#include <glad/glad.h>

#include "Camera.h"
#include "Program.h"

class VolumeClouds
{
private:
  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint quadVao = 0;
  GLuint quadVbo = 0;

  GLuint colorTexture = 0;
  GLuint depthTexture = 0;
  GLuint cloudFb = 0;

  Program *program = nullptr;
  Program *compositeProgram = nullptr;

  GLuint globalAlphaSampler = 0;
  GLuint coverageDetailSampler = 0;
  GLuint noiseVolumeSampler = 0;

  GLuint transmittance = 0;
  GLuint scattering = 0;
  GLuint irradiance = 0;

  glm::dvec2 cloudDisplacementMeters = glm::dvec2(0.0);

  std::vector<GLint> viewport = std::vector<GLint>( 4 );

public:
  glm::ivec2 cloudTexturesDimensions;

  glm::dmat4 planetMatrix = glm::dmat4(1.0);
  double innerRadius = 6369344.0;
  glm::dvec3 planetCenter = glm::dvec3(0.0, 0.0, 0.0);

  glm::dvec3 lightDirection = glm::dvec3(1, 0, 0);
  float windSpeed = 10.0;

  struct Uniforms
  {
    bool DEBUG_VOLUME;
    bool ENABLE_HIGH_DETAIL_CLOUDS;
    float cloudTopZeroDensityHeight[2];
    float cloudBottomZeroDensity[2];
    float cloudOcclusionStrength[2];
    float cloudDensityMultiplier[2];
    float noiseFrequencyScale;
    float powderStrength;
    float scatterSampleDistanceScale;
    float scatterDistanceMultiplier;
    float cloudChaos;
  };

  Uniforms uniforms = {
    false,
    false,
    { 5000.0f, 9000.0f },
    { 2000.0f, 4000.0f },
    { 0.25f, 0.5f },
    { 0.005f, 0.001f },
    0.00015f,
    0.1f,
    25.0f,
    0.5f,
    0.8f
  };

  VolumeClouds();
  ~VolumeClouds();

  inline void SetTransmittanceSampler(GLuint _transmittance) { transmittance = _transmittance; }
  inline void SetScatteringSampler(GLuint _scattering) { scattering = _scattering; }
  inline void SetIrradianceSampler(GLuint _irradiance) { irradiance = _irradiance; }

  inline GLuint GetCloudColorTexture() { return colorTexture; }
  inline GLuint GetCloudDepthTexture() { return depthTexture; }

  inline GLuint GetGlobalAlphaSampler() const { return globalAlphaSampler; }
  inline GLuint GetCoverageDetailSampler() const { return coverageDetailSampler; }
  inline GLuint GetNoiseVolumeSampler() const { return noiseVolumeSampler; }

  inline glm::dvec2 GetCloudDisplacementMeters() const { return cloudDisplacementMeters; }

  void PreComputeClouds(Camera &camera);
  void Draw(Camera &camera);
  void Update(Camera &camera, double time, double deltaTime = 0.0);
};

#endif // __VOLUME_CLOUDS_H__
