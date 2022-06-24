#ifndef __PLANET_H__
#define __PLANET_H__

#include "Renderable.h"

class Planet : public Renderable
{
private:
  int numVertex;
public:
  Planet(double radius);
  ~Planet();

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;
};

#endif __PLANET_H__
