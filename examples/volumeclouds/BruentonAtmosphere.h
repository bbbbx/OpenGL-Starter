#ifndef __BRUENTON_ATMOSPHERE_H__
#define __BRUENTON_ATMOSPHERE_H__

#include "Renderable.h"

class BruentonAtmosphere : public Renderable
{
public:
  BruentonAtmosphere(double bottom, double top);
  ~BruentonAtmosphere();

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;

  inline void SetTransmittanceSampler(GLuint _transmittance) { transmittance = _transmittance; }
  inline void SetScatteringSampler(GLuint _scattering) { scattering = _scattering; }
  inline void SetIrradianceSampler(GLuint _irradiance) { irradiance = _irradiance; }
  inline void SetSunDirection(glm::dvec3 dir) { sunDirection = dir;}

private:
  int numVertex = 0;

  // Samplers
  GLuint transmittance = 0;
  GLuint scattering = 0;
  GLuint irradiance = 0;

  glm::dvec3 sunDirection;

  double bottomRadius = 0.0;
  double topRadius = 0.0;
};


#endif // __BRUENTON_ATMOSPHERE_H__
