#include "ScreenQuad.h"

#include <vector>

namespace
{
static std::vector<float> quadVaoData = {
/*position             uv */
  0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
  1.0f, 1.0f, 0.0f,    1.0f, 1.0f,
  0.0f, 1.0f, 0.0f,    0.0f, 1.0f
};
} // namespace

ScreenQuad::ScreenQuad() {}

ScreenQuad::ScreenQuad(const std::string& fragShaderSourcePath) {
  program = new Program( "./shaders/volumeclouds/ScreenQuad.vert", fragShaderSourcePath);

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, quadVaoData.size() * sizeof( float ), quadVaoData.data(), GL_STATIC_DRAW );

  GLuint glProgram = program->GetGLProgram();

  GLint inPositionLoc = glGetAttribLocation( glProgram, "in_position" );
  glEnableVertexAttribArray( inPositionLoc );
  GLint64 offset = 0;
  glVertexAttribPointer( inPositionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)offset );

  GLint inTexCoordLoc = glGetAttribLocation( glProgram, "in_texCoord" );
  glEnableVertexAttribArray( inTexCoordLoc );
  offset = sizeof( float ) * 3;
  glVertexAttribPointer( inTexCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)offset );

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );
}

void ScreenQuad::Update(Camera &camera, double time, double deltaTime) {

}

void ScreenQuad::Draw(Camera &camera) {
  glBindVertexArray( vao );

  glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
}
