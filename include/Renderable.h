#ifndef __RENDERABLE_H__
#define __RENDERABLE_H__

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Program.h"
#include "Camera.h"

class Renderable
{
private:
  /* data */
protected:
  GLuint vao = 0;
  GLuint vbo = 0;

  Program *program = nullptr;

public:
  glm::dmat4 modelMatrix = glm::identity<glm::dmat4>();

  Renderable() = default;
  inline ~Renderable() {
    glDeleteBuffers( 1, &vbo );
    glDeleteVertexArrays( 1, &vao );

    delete program;
  };

  virtual void Update(Camera &camera, double time, double deltaTime) = 0;
  virtual void Draw(Camera &camera) = 0;
};

#endif __RENDERABLE_H__
