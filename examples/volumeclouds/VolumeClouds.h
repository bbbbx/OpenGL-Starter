#ifndef __VOLUME_CLOUDS_H__
#define __VOLUME_CLOUDS_H__

#include <glad/glad.h>

#include "Camera.h"
#include "Program.h"

class VolumeClouds
{
private:
  GLuint vao = 0;
  GLuint vbo = 0;

  Program *program = nullptr;

  GLuint globalAlphaSampler = 0;
  GLuint coverageDetailSampler = 0;
  GLuint noiseVolumeSampler = 0;
  GLuint transmittance = 0;
  GLuint scattering = 0;
  GLuint irradiance = 0;

  glm::dmat4 planetMatrix = glm::dmat4(1.0);
  glm::dvec2 cloudDisplacementMeters = glm::dvec2(0.0);

  double innerRadius = 6371000.0;
  glm::dvec3 planetCenter = glm::dvec3(0.0, 0.0, 0.0);

  int DEBUG = 0;

public:
  glm::dvec3 lightDirection = glm::dvec3(1, 0, 0);

  VolumeClouds(/* args */);
  ~VolumeClouds();

  void Draw(Camera &camera);
  void Update(Camera &camera, double time, double deltaTime = 0.0);
};

#endif // __VOLUME_CLOUDS_H__
