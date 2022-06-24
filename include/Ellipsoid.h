#ifndef __ELLIPSOID_H__
#define __ELLIPSOID_H__

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Camera.h"
#include "Renderable.h"

class Ellipsoid : public Renderable
{
private:
  int numVertex;

public:
  static std::vector<float> CreateGeometry(glm::dvec3 radii, glm::dvec3 innerRadii = glm::dvec3( 1.0 ), unsigned int stackPartitions = 64, unsigned int slicePartitions = 128);

  Ellipsoid() : Ellipsoid(glm::dvec3(1.0), glm::dvec3(1.0), 64, 128) {};
  Ellipsoid(glm::dvec3 radii) : Ellipsoid(radii, glm::dvec3(1.0), 64, 128) {};
  Ellipsoid(glm::dvec3 radii, glm::dvec3 innerRadii, unsigned int stackPartitions, unsigned int slicePartitions);
  ~Ellipsoid();

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;
};

#endif // __ELLIPSOID_H__
