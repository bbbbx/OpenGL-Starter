// #include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Axes.h"
#include "Camera.h"

namespace {
float vertexAttributes[6][6] = {
  // position         color
  // X axis
  { 0.0, 0.0, 0.0,    1.0, 0.0, 0.0 },
  { 1.0, 0.0, 0.0,    1.0, 0.0, 0.0 },
  // Y axis
  { 0.0, 0.0, 0.0,    0.0, 1.0, 0.0 },
  { 0.0, 1.0, 0.0,    0.0, 1.0, 0.0 },
  // Z axis
  { 0.0, 0.0, 0.0,    0.0, 0.0, 1.0 },
  { 0.0, 0.0, 1.0,    0.0, 0.0, 1.0 }
};
} // anonymous namespace

Axes::Axes(double scale)
{
  program = new Program( "./shaders/axes/axes.vert", "./shaders/axes/axes.frag" );

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexAttributes), vertexAttributes, GL_STATIC_DRAW);

  GLuint glProgram = program->GetGLProgram();
  GLint posLoc = glGetAttribLocation( glProgram, "in_position" );
  GLint colorLoc = glGetAttribLocation( glProgram, "in_color" );

  glEnableVertexAttribArray(posLoc);
  GLint64 offset = 0;
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertexAttributes[0]), (GLvoid*)offset);

  glEnableVertexAttribArray(colorLoc);
  offset = sizeof(float) * 3;
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertexAttributes[0]), (GLvoid*)offset);

  glBindVertexArray(0);

  modelMatrix = glm::identity<glm::dmat4>();
  modelMatrix = glm::scale(modelMatrix, glm::dvec3(scale));
}

void Axes::Draw(Camera &camera)
{
  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();

  glDisable( GL_DEPTH_TEST );

  program
    ->Use()
    ->BindMat4( "modelMatrix", glm::value_ptr(glm::fmat4(modelMatrix)) )
    ->BindMat4( "viewProj",  glm::value_ptr(glm::fmat4(viewProj)) )
    ->BindFloat( "farClipDistance", (float)camera.getFar() )
  ;

  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, 6);
}

Axes::~Axes()
{
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  delete program;
}
