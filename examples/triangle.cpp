#include "vapp.h"
#include "LoadShaders.h"

BEGIN_APP_DECLARATION(Triangle)
  virtual void Initialize(const char* title);
  virtual void Display(bool auto_redraw);
  virtual void Resize(int width, int height);
  virtual void Finalize(void);
END_APP_DECLARATION()

DEFINE_APP(Triangle, "Triangle")

GLuint VAO;

void Triangle::Initialize( const char* title )
{
  base::Initialize( title );

  glGenVertexArrays( 1, &VAO );

  ShaderInfo shaders[] = {
    { GL_VERTEX_SHADER,   "shaders/triangle/triangle.vert" },
    { GL_FRAGMENT_SHADER, "shaders/triangle/triangle.frag" },
    { GL_NONE, nullptr }
  };
  GLuint program = LoadShaders( shaders );
  glUseProgram( program );
}

void Triangle::Display(bool auto_redraw)
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear( GL_COLOR_BUFFER_BIT );

  glBindVertexArray( VAO );
  glDrawArrays( GL_TRIANGLES, 0, 3 );

  base::Display( auto_redraw );
}

void Triangle::Resize(int width, int height)
{
  glViewport( 0, 0, width, height );
}

void Triangle::Finalize(void)
{
  glfwDestroyWindow(pWindow);
  glfwTerminate();
}
