#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <string>
#include <array>
#include <unordered_map>

class Program
{
public:
  typedef struct
  {
    std::string name;
    GLenum type;
    GLint size;
    GLint location;
    GLuint index;
  } Uniform;

  Program() = default;
  Program(const std::string& vertShaderSourcePath, const std::string& fragShaderSourcePath);

  inline GLuint GetGLProgram() const { return glProgram; };
  inline std::unordered_map<std::string, Program::Uniform> GetUniforms() const { return uniforms; }

  Program* Use();
  Program* BindFloat(const std::string& name, float value);
  Program* BindFloat(const std::string& name, double value) { return BindFloat( name, (float)value ); };

  Program* BindInt(const std::string& name, int value);

  Program* BindVec2(const std::string& name, float x, float y);
  Program* BindVec2(const std::string& name, double x, double y) { return BindVec2( name, (float)x, (float)y ); } ;

  Program* BindVec3(const std::string& name, float x, float y, float z);
  Program* BindVec3(const std::string& name, double x, double y, double z) { return BindVec3( name, (float)x, (float)y, (float)z ); } ;

  Program* BindVec4(const std::string& name, float x, float y, float z, float w);
  Program* BindVec4(const std::string& name, double x, double y, double z, double w) { return BindVec4( name, (float)x, (float)y, (float)z, (float)w ); } ;

  Program* BindMat4(const std::string& name, const float* values, GLboolean transpose = GL_FALSE);
  Program* BindMat4(const std::string& name, const double* values, GLboolean transpose = GL_FALSE) {
    return BindMat4( name, (const float*)values, transpose );
  };

  Program* BindTexture2D(const std::string& name, GLuint texture, int unit);
  Program* BindTexture3D(const std::string& name, GLuint texture, int unit);

  ~Program();

private:
  GLuint glVertShader = 0;
  GLuint glFragShader = 0;
  GLuint glProgram = 0;

  std::unordered_map<std::string, Program::Uniform> uniforms;

  GLint GetUniformLocation(const std::string& name);

  void BindTexture(const std::string& name, GLuint texture, int unit, GLenum target);
};

#endif