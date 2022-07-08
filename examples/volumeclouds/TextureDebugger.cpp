#include "TextureDebugger.h"

#include <iostream>
#include <vector>

TextureDebugger::TextureDebugger(GLuint texture) : glTexture( texture ) {
  program = new Program( "./shaders/texturedebugger/texturedebugger.vert", "./shaders/texturedebugger/texturedebugger.frag" );

  glGenBuffers( 1, &vbo );
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
}

void TextureDebugger::Update(Camera &camera, double time, double deltaTime) {

}

void TextureDebugger::Draw(Camera &camera) {
  if ( glTexture == 0 ) return;

  GLint blendEnabled;
  GLint depthTestEnabled;
  GLint cullFaceEnabled;
  GLint depthWriteMask;
  glGetIntegerv( GL_BLEND, &blendEnabled );
  glGetIntegerv( GL_DEPTH_TEST, &depthTestEnabled );
  glGetIntegerv( GL_CULL_FACE, &cullFaceEnabled );
  glGetIntegerv( GL_DEPTH_WRITEMASK, &depthWriteMask );

  std::vector<GLint> viewport( 4 );
  glGetIntegerv( GL_VIEWPORT, viewport.data() );

  program
    ->Use()
    ->BindTexture2D( "u_texture", glTexture, 0 )
    ;

  glBindVertexArray( vao );

  int scale = 2;
  int width = viewport[2] / scale;
  int height = viewport[3] / scale;

  glDisable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glDepthMask( GL_FALSE );
  glViewport( viewport[2] - width, viewport[3] - height, width, height );

  glDrawArrays( GL_TRIANGLES, 0, 3 );

// Restore state
  if ( blendEnabled ) {
    glEnable( GL_BLEND );
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

  glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
}
