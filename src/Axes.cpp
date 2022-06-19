// #include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Axes.h"
#include "LoadShaders.h"
#include "Camera.h"

namespace {
float vertexAttributes[6][6] = {
  // X axis
  // position         color
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
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexAttributes), vertexAttributes, GL_STATIC_DRAW);

  ShaderInfo shaders[] = {
    { GL_VERTEX_SHADER, "shaders/axes/axes.vert" },
    { GL_FRAGMENT_SHADER, "shaders/axes/axes.frag" },
    { GL_NONE, nullptr },
  };
  program = LoadShaders(shaders);

  GLint posLoc = glGetAttribLocation(program, "in_position");
  GLint colorLoc = glGetAttribLocation(program, "in_color");

  glEnableVertexAttribArray(posLoc);
  GLint64 offset = 0;
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertexAttributes[0]), (GLvoid*)offset);

  glEnableVertexAttribArray(colorLoc);
  offset = sizeof(float) * 3;
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertexAttributes[0]), (GLvoid*)offset);

  modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
  viewProjLoc = glGetUniformLocation(program, "viewProj");
  farClipDistanceLoc = glGetUniformLocation(program, "farClipDistance");

  glBindVertexArray(0);

  modelMatrix = glm::identity<glm::dmat4>();
  modelMatrix = glm::scale(modelMatrix, glm::dvec3(scale));
}

void Axes::Draw(Camera &camera)
{
  glUseProgram(program);
  glUniformMatrix4fv(modelMatrixLoc, 1, false, glm::value_ptr(glm::fmat4(modelMatrix)));
  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
  glUniformMatrix4fv(viewProjLoc, 1, false, glm::value_ptr(glm::fmat4(viewProj)));
  glUniform1f(farClipDistanceLoc, (float)camera.getFar());

  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, 6);
}

Axes::~Axes()
{
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  GLint count;
  glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
  GLuint shaders[count];
  glGetAttachedShaders(program, count, &count, shaders);
  for (int i = 0; i < count; i++)
  {
    glDeleteShader(shaders[i]);
  }
  glDeleteProgram(program);
}
