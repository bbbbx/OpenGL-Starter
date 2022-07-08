#ifndef __PLANET_H__
#define __PLANET_H__

#include "Renderable.h"

#include <glm/glm.hpp>

class Planet : public Renderable
{
private:
  int numVertex;
  GLuint blueEarthSampler = 0;
public:
  // Samplers
  GLuint transmittance = 0;
  GLuint scattering = 0;
  GLuint irradiance = 0;

  GLuint cloudSampler = 0;
  GLuint cloudDetail2d = 0;

  glm::dvec3 lightDirection;
  double radius;

  Planet(double radius);
  ~Planet();

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;
};

#endif // __PLANET_H__
