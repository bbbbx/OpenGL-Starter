#include "GLUtils.h"

#include <iostream>

namespace
{

bool isPowerOfTwo(int n) {
  return n != 0 && (n & (n - 1)) == 0;
}

} // namespace

namespace GLUtils
{

GLuint NewTexture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type, const void* pixels,
  GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT
) {
  GLuint texture = 0;
  glGenTextures( 1, &texture );

  if ( ! texture ) {
    std::cerr << "Failed to create texture" << std::endl;
  }
  
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT );
  glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels );

  // If min filter is mipmap-related, try to generate mipmap.
  if ( minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST
  ) {
    if ( isPowerOfTwo(width) && isPowerOfTwo( height ) ) {
      glGenerateMipmap( GL_TEXTURE_2D );
    } else{
      std::cerr << "Require mipmap min filter, but dimensions is not power of two" << std::endl;
    }
  } 

  glBindTexture( GL_TEXTURE_2D, 0 );

  return texture;
}

GLuint NewFramebuffer(std::vector<GLuint> colorTextures, GLuint depthTexture) {
  GLint oldFramebuffer;
  glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &oldFramebuffer );

  GLuint framebuffer;
  glGenFramebuffers( 1, &framebuffer );
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, framebuffer );

  std::vector<GLenum> buffersNeedToDraw;

  glActiveTexture( GL_TEXTURE0 );
  for (size_t i = 0; i < colorTextures.size(); i++)
  {
    GLuint colorTexture = colorTextures[ i ];
    GLenum attachment = GL_COLOR_ATTACHMENT0 + i;

    glBindTexture( GL_TEXTURE_2D, colorTexture );
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, colorTexture, 0 );

    buffersNeedToDraw.push_back( attachment );
  }

  glDrawBuffers( buffersNeedToDraw.size(), buffersNeedToDraw.data() );

  if ( depthTexture ) {
    glBindTexture( GL_TEXTURE_2D, depthTexture );
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );
  }

  GLenum success = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER );
  if ( ! success ) {
    std::cout << "Framebuffer is not completeness: " << success << std::endl;;
  }

  glBindTexture( GL_TEXTURE_2D, 0 );
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, oldFramebuffer );

  return framebuffer;
}

} // namespace GLUtils
