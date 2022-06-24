#include <stdexcept>
#include <iostream>
#include <glad/glad.h>

#include "LoadShaders.h"
#include "Program.h"

Program::Program(const std::string& vertShaderSourcePath, const std::string& fragShaderSourcePath) {
  ShaderInfo* vertShaderinfo = readShaderFile( GL_VERTEX_SHADER, vertShaderSourcePath );
  ShaderInfo* fragShaderinfo = readShaderFile( GL_FRAGMENT_SHADER, fragShaderSourcePath );

  glVertShader = vertShaderinfo->shader;
  glFragShader = fragShaderinfo->shader;

  GLuint shaders[] = { glVertShader, glFragShader };
  size_t size = std::size( shaders );
  glProgram = CreateGLProgram( shaders, size );
}

Program* Program::Use() {
  glUseProgram(glProgram);
  return this;
}

GLint Program::GetUniformLocation(const std::string& name) {
  GLint loc = -1;
  try {
    loc = uniformLocationMap.at( name );
  } catch(const std::out_of_range& e) {
    loc = glGetUniformLocation( glProgram, name.c_str() );
  }

  if ( loc == -1 ) {
  #ifndef NDEBUG
    std::cout << "Could not found '" << name << "' uniform" << std::endl;
  #endif
  }

  return loc;
}

Program* Program::BindFloat(const std::string& name, float value) {
  GLint loc = GetUniformLocation(name);
  if (loc != -1) {
    glUniform1f( loc, value );
  }
  return this;
}

Program* Program::BindInt(const std::string& name, int value) {
  GLint loc = GetUniformLocation(name);
  if (loc != -1) {
    glUniform1i( loc, value );
  }
  return this;
}

Program* Program::BindVec2(const std::string& name, float x, float y) {
  GLint loc = GetUniformLocation(name);
  if (loc != -1) {
    glUniform2f( loc, x, y );
  }
  return this;
}

Program* Program::BindVec3(const std::string& name, float x, float y, float z) {
  GLint loc = GetUniformLocation(name);
  if (loc != -1) {
    glUniform3f( loc, x, y, z );
  }
  return this;
}

Program* Program::BindVec4(const std::string& name, float x, float y, float z, float w) {
  GLint loc = GetUniformLocation(name);
  if (loc != -1) {
    glUniform4f( loc, x, y, z, w );
  }
  return this;
}

Program* Program::BindMat4(const std::string& name, const float* values, GLboolean transpose) {
  GLint loc = GetUniformLocation(name);
  if (loc != -1) {
    glUniformMatrix4fv( loc, 1, transpose, values);
  }
  return this;
}

void Program::BindTexture(const std::string& name, GLuint texture, int unit, GLenum target) {
  GLint loc = GetUniformLocation( name );
  if (loc != -1) {
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( target, texture );
    glUniform1i( loc, unit );
  }
}

Program* Program::BindTexture2D(const std::string& name, GLuint texture, int unit) {
  BindTexture(name, texture, unit, GL_TEXTURE_2D);
  return this;
}

Program* Program::BindTexture3D(const std::string& name, GLuint texture, int unit) {
  BindTexture(name, texture, unit, GL_TEXTURE_3D);
  return this;
}

Program::~Program() {
  glDeleteShader( glVertShader );
  glDeleteShader( glFragShader );
  glDeleteProgram( glProgram );
}