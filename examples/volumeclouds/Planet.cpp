#include "Planet.h"
#include "Ellipsoid.h"

#include <vector>
#include <glm/gtc/type_ptr.hpp>

Planet::Planet(double radius) : Renderable()
{
  std::vector<float> flattedVertices = Ellipsoid::CreateGeometry( glm::dvec3( radius ) );
  numVertex = flattedVertices.size() / 3;

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, flattedVertices.size() * sizeof( float ), flattedVertices.data(), GL_STATIC_DRAW );

  program = new Program( "./shaders/planet/planet.vert", "./shaders/planet/planet.frag" );

  GLuint glProgram = program->GetGLProgram();
  GLint posLoc = glGetAttribLocation( glProgram, "in_position" );

  glEnableVertexAttribArray(posLoc);
  GLint64 offset = 0;
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)offset);

  glBindVertexArray( 0 );
}

Planet::~Planet() {
}

void Planet::Update(Camera &camera, double time, double deltaTime) {
  
}

void Planet::Draw(Camera &camera) {
  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();

  program
    ->Use()
    ->BindMat4( "modelMatrix", glm::value_ptr( glm::fmat4(modelMatrix) ) )
    ->BindMat4( "viewProj",  glm::value_ptr( glm::fmat4(viewProj) ) )
    ->BindFloat( "farClipDistance", (float)camera.getFar() )
    ->BindVec4( "color", 0.0, 0.5, 0.0, 1.0 )
  ;

  glBindVertexArray(vao);
  glEnable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glDrawArrays(GL_TRIANGLES, 0, numVertex );
}
