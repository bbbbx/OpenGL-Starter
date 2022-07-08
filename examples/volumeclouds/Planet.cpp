#include "Planet.h"
#include "Ellipsoid.h"

#include "GLUtils.h"

#include <vector>
// #include <iostream>
#include <glm/gtc/type_ptr.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

Planet::Planet(double radius) : Renderable(), radius(radius)
{
  std::vector<float> flattedVertices = Ellipsoid::CreateGeometry( glm::dvec3( radius ), glm::dvec3( 1.0 ), 512, 1024 );
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

  int width = 8192;
  int height = 4096;
  int channel = 4;
  stbi_set_flip_vertically_on_load( 1 );
  stbi_uc* pixels = stbi_load( "./data/land_shallow_topo_8192.png", &width, &height, &channel, channel );

  blueEarthSampler = GLUtils::NewTexture2D( width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, pixels,
    GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR );

  stbi_image_free( pixels );
}

Planet::~Planet() {
}

void Planet::Update(Camera &camera, double time, double deltaTime) {
  
}

void Planet::Draw(Camera &camera) {
  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
  glm::dvec3 cameraPosition = camera.getPosition();

  glm::dmat4 planetMatrixInv = glm::inverse( modelMatrix );

  program
    ->Use()
    ->BindMat4( "modelMatrix", glm::value_ptr( glm::fmat4(modelMatrix) ) )
    ->BindMat4( "viewProj",  glm::value_ptr( glm::fmat4(viewProj) ) )
    ->BindFloat( "farClipDistance", (float)camera.getFar() )
    ->BindVec4( "color", 0.0, 0.5, 0.0, 1.0 )
    ->BindVec3( "cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z )
    ->BindVec3( "lightDirection", lightDirection.x, lightDirection.y, lightDirection.z )

// Global Uniforms
    ->BindVec3( "planetCenter", 0.0, 0.0, 0.0 )
    ->BindMat4( "planetMatrixInv", glm::value_ptr( glm::fmat4(planetMatrixInv) ) )
    ->BindFloat( "innerRadius", radius )

    ->BindTexture2D( "u_blueEarthSampler", blueEarthSampler, 0 )
    ->BindTexture2D( "transmittance_texture", transmittance, 1 )
    ->BindTexture3D( "scattering_texture", scattering, 2 )
    ->BindTexture3D( "single_mie_scattering_texture", scattering, 2 )
    ->BindTexture2D( "irradiance_texture", irradiance, 3 )
    ->BindTexture2D( "coverageDetailSampler2", cloudSampler, 4 )
    ->BindTexture2D( "coverageDetailSampler2", cloudDetail2d, 5 )
  ;

  glBindVertexArray(vao);
  glEnable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glDrawArrays(GL_TRIANGLES, 0, numVertex );
}
