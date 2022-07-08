#ifndef __TEXTURE_DEBUGGER_H__
#define __TEXTURE_DEBUGGER_H__

#include <glad/glad.h>

#include "Renderable.h"

class TextureDebugger : public Renderable {
private:
  GLuint glTexture;
public:
  TextureDebugger() : TextureDebugger( 0 ) {};
  TextureDebugger(GLuint texture);
  inline ~TextureDebugger() {};

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;

  void SetTexture(GLuint texture) { glTexture = texture; }
};

#endif // __TEXTURE_DEBUGGER_H__
