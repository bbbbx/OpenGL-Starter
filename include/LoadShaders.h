#ifndef __LOAD_SHADERS_H__
#define __LOAD_SHADERS_H__

#include <optional>
#include <string>
#include "glad/glad.h"

typedef struct
{
  GLenum      type;
  const char* filename;
  GLuint      shader;
} ShaderInfo;


GLuint LoadShaders( ShaderInfo*);

GLuint CreateGLShader(GLenum type, const std::string& str, const std::string& filename = nullptr);
GLuint CreateGLProgram(GLuint* shaders, size_t size);

std::string loadFileToString(const std::string& filepath);
GLuint readShaderFromString(GLenum type, const std::string& source, const std::optional<std::string> includeDirPath);
ShaderInfo* readShaderFile(GLenum type, const std::string& filename);

#endif