#ifndef __SCREEN_QUAD_H__
#define __SCREEN_QUAD_H__

#include "Renderable.h"

class ScreenQuad : public Renderable
{
private:
  /* data */
public:
  ScreenQuad();
  ScreenQuad(const std::string& fragShaderSourcePath);
  ~ScreenQuad();

  inline Program* GetProgram() const { return program; };

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;
};

#endif // __SCREEN_QUAD_H__