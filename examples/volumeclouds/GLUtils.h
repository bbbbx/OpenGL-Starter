#ifndef __GL_UTILS_H__
#define __GL_UTILS_H__

#include <vector>

#include <glad/glad.h>

namespace GLUtils
{
GLuint NewTexture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type, const void* pixels,
  GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR, GLenum wrapS = GL_CLAMP_TO_EDGE, GLenum wrapT = GL_CLAMP_TO_EDGE
);

GLuint NewFramebuffer(std::vector<GLuint> colorTextures, GLuint depthTexture = 0);
} // namespace GLUtils


#endif // __GL_UTILS_H__
