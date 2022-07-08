#include "LoadShaders.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

static const GLchar* ReadShader( const char* filename )
{
#if defined(__MINGW32__)
  FILE* infile = fopen( filename, "rb" );
#elif (defined(WIN32))
  FILE* infile;
  // warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. 
  // To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
  fopen_s( &infile, filename, "rb" );
#else
  FILE* infile = fopen( filename, "rb" );
#endif // WIN32

  if ( !infile )
  {
    std::cerr << "Unable to open file '" << filename << "'" << std::endl;
    return nullptr;
  }

  fseek( infile, 0, SEEK_END );
  int len = ftell( infile );
  fseek( infile, 0, SEEK_SET );

  GLchar* source = new GLchar[len+1];

  fread( source, 1, len, infile );
  fclose( infile );

  source[len] = 0;

  return const_cast<const GLchar*>(source);
}

std::string loadFileToString(const std::string& filepath) {
  std::filesystem::path locatedFilepath = std::filesystem::absolute( filepath );

  std::string outString;
  std::ifstream stream(locatedFilepath, std::ios::in);
  if (stream.is_open()) {
    std::string line = "";
    while (getline(stream, line)) {
      outString += "\n" + line;
    }
    stream.close();
  } else {
    std::cerr << "Could not open file: " + filepath << std::endl;
    // throw std::runtime_error("");
  }

  return outString;
}

std::string preprocessIncludes(const std::string& source, const std::string& includePath, int level = 0) {
  if (level > 32) {
    throw std::runtime_error("header inclusion depth limit reached, might be caused by circle header inclusion");
  }
  using namespace std;

  static const std::regex re("^[ \t]*#[ ]*include[ ]+[\"<](.*)[\">].*");
  stringstream input;
  stringstream output;
  input << source;

  size_t line_number = 1;
  std::smatch matches;

  string line;
  while (std::getline(input, line))
  {
    if (std::regex_match(line, matches, re)) {
      std::string includeFile = matches[1];
      std::filesystem::path includeFileFullPath = std::filesystem::absolute(includePath + "/" + includeFile);
      std::string includeSource = loadFileToString(includeFileFullPath.string());

      // output << preprocessIncludes(includeSource, includePath, level + 1) << endl;
      output << preprocessIncludes(includeSource, includeFileFullPath.parent_path().string(), level + 1) << endl;
    } else {
      output << line << endl;
    }
    ++line_number;
  }
  return output.str();
}

GLuint readShaderFromString(GLenum type, const std::string& source, const std::optional<std::string> includeDirPath) {
  std::string processedSource = includeDirPath ? preprocessIncludes( source, *includeDirPath ) : source;

  GLuint shader = glCreateShader( type );
  const GLchar* cSource = processedSource.c_str();
  glShaderSource( shader, 1, &cSource, nullptr );
  glCompileShader( shader );

  GLint success;
  glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
  if ( !success )
  {
    GLsizei len;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

    GLchar* log = new GLchar[len+1];
    glGetShaderInfoLog( shader, len, &len, log );
    std::cerr << ("Shader compilation failed: \n" + std::string(log)) << std::endl;
    throw std::runtime_error("Shader compilation failed: \n" + std::string(log) + "\n");
  }

  return shader;
}

ShaderInfo* readShaderFile(GLenum type, const std::string& filename) {
  std::string source = loadFileToString(filename);
  std::string includeDirPath = std::filesystem::path(filename).parent_path().string();

  GLuint shader = readShaderFromString( type, source, includeDirPath );
  ShaderInfo* shaderInfo = new ShaderInfo();
  shaderInfo->filename = filename.c_str();
  shaderInfo->shader = shader;
  shaderInfo->type = type;

  return shaderInfo;
}

GLuint CreateGLShader(GLenum type, const std::string& str, const std::string& filename) {
  if ( str.empty() ) return 0;

  GLuint shader = glCreateShader( type );
  const char* shaderSource = str.c_str();
  glShaderSource( shader, 1, &shaderSource, nullptr );
  glCompileShader( shader );

  GLint success;
  glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
  if ( !success )
  {
    GLsizei len;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

    GLchar* log = new GLchar[len+1];
    glGetShaderInfoLog( shader, len, &len, log );
    std::cerr << "Shader '" << ((filename.empty()) ? "file name is empty" : filename) << "' compilation failed: \n" << log << std::endl;
    return 0;
  }

  return shader;
}

GLuint CreateGLProgram(GLuint* shaders, size_t size) {
  GLuint program = glCreateProgram();

  for (size_t i = 0; i < size; i++)
  {
    GLuint shader = *(shaders + i);
    glAttachShader( program, shader );
  }

  glLinkProgram( program );

  GLint success;
  glGetProgramiv( program, GL_LINK_STATUS, &success );
  if ( !success )
  {
    GLsizei len;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

    GLchar* log = new GLchar[len+1];
    glGetProgramInfoLog( program, len, &len, log );
    std::cerr << "Program linking failed: " << std::string( log ) << std::endl;
    throw std::runtime_error( "Program linking failed: " + std::string( log ) );
    return 0;
  }
  return program;
}

GLuint LoadShaders( ShaderInfo* shaders )
{
  if ( shaders == nullptr ) { return 0; }

  GLuint program = glCreateProgram();

  ShaderInfo* entry = shaders;
  while ( entry->type != GL_NONE )
  {
    GLuint shader = glCreateShader( entry->type );
    entry->shader = shader;

    const GLchar* source = ReadShader( entry->filename );
    if ( source == nullptr )
    {
      for ( entry = shaders; entry->type != GL_NONE; ++entry )
      {
        glDeleteShader( entry->shader );
        entry->shader = 0;
      }
      return 0;
    }

    glShaderSource( shader, 1, &source, nullptr );
    delete [] source;

    glCompileShader( shader );

    GLint success;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
      GLsizei len;
      glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

      GLchar* log = new GLchar[len+1];
      glGetShaderInfoLog( shader, len, &len, log );
      std::cerr << "Shader '" << entry->filename << "' compilation failed: \n" << log << std::endl;
      delete [] log;
      return 0;
    }

    glAttachShader( program, shader );

    ++entry;
  }

  glLinkProgram( program );

  GLint success;
  glGetProgramiv( program, GL_LINK_STATUS, &success );
  if ( !success )
  {
    GLsizei len;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

    GLchar* log = new GLchar[len+1];
    glGetProgramInfoLog( program, len, &len, log );
    std::cerr << "Program linking failed: " << log << std::endl;
    delete [] log;

    for ( entry = shaders; entry->type != GL_NONE; ++entry )
    {
      glDeleteShader( entry->shader );
      entry->shader = 0;
    }

    return 0;
  }

  return program;
}
