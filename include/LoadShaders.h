#ifndef __LOAD_SHADERS_H__
#define __LOAD_SHADERS_H__

#include "glad/glad.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct
{
  GLenum      type;
  const char* filename;
  GLuint      shader;
} ShaderInfo;


GLuint LoadShaders( ShaderInfo*);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif