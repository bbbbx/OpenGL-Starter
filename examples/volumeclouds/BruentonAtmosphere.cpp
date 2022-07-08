#include "BruentonAtmosphere.h"

#include "Ellipsoid.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

BruentonAtmosphere::BruentonAtmosphere(double bottom, double top): bottomRadius(bottom), topRadius(top) {
  std::vector<float> flattedVertices = Ellipsoid::CreateGeometry( glm::dvec3( topRadius ), glm::dvec3( 1.0 ), 512, 1024 );
  numVertex = flattedVertices.size() / 3;

  program = new Program( "./shaders/bruentonatmosphere/atmosphere.vert", "./shaders/bruentonatmosphere/atmosphere.frag" );
  GLuint glProgram = program->GetGLProgram();
  GLint posLoc = glGetAttribLocation( glProgram, "in_position" );

  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );

  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, flattedVertices.size() * sizeof( float ), flattedVertices.data(), GL_STATIC_DRAW );

  glEnableVertexAttribArray(posLoc);
  GLint64 offset = 0;
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)offset);

  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

BruentonAtmosphere::~BruentonAtmosphere() {
}

void BruentonAtmosphere::Update(Camera &camera, double time, double deltaTime) {

}

void BruentonAtmosphere::Draw(Camera &camera) {
  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
  glm::dvec3 eye = camera.getPosition();

  GLint blendEnabled;
  GLint depthTestEnabled;
  GLint cullFaceEnabled;
  GLint depthWriteMask;
  glGetIntegerv( GL_BLEND, &blendEnabled );
  glGetIntegerv( GL_DEPTH_TEST, &depthTestEnabled );
  glGetIntegerv( GL_CULL_FACE, &cullFaceEnabled );
  glGetIntegerv( GL_DEPTH_WRITEMASK, &depthWriteMask );

  glEnable( GL_BLEND );
  glBlendEquation( GL_FUNC_ADD );
  glBlendFunc( GL_ONE, GL_ONE );
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glDepthMask( GL_FALSE );

  program
    ->Use()
    ->BindMat4( "viewProj", glm::value_ptr( glm::fmat4( viewProj ) ) )
    ->BindVec3( "camera", eye.x, eye.y, eye.z )
    ->BindTexture2D( "transmittance_texture", transmittance, 0 )
    ->BindTexture3D( "scattering_texture", scattering, 1 )
    ->BindTexture3D( "single_mie_scattering_texture", scattering, 1 )
    ->BindTexture2D( "irradiance_texture", irradiance, 2 )

    ->BindVec3( "sun_direction", sunDirection.x, sunDirection.y, sunDirection.z )
  ;
  glBindVertexArray( vao );

  glDrawArrays( GL_TRIANGLES, 0, numVertex );

// Restore state
  if ( ! blendEnabled ) {
    glDisable( GL_BLEND );
  }
  if ( depthTestEnabled ) {
    glEnable( GL_DEPTH_TEST );
  }
  if ( cullFaceEnabled ) {
    glEnable( GL_CULL_FACE );
  }
  if ( depthWriteMask ) {
    glDepthMask( GL_TRUE );
  }
}