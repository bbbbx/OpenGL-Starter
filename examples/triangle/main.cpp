#include "vapp.h"
#include "Program.h"

#include <memory>

BEGIN_APP_DECLARATION(Triangle)
  virtual void Initialize(int argc, char** argv, const char* title);
  virtual void Display(bool auto_redraw);
  virtual void Resize(int width, int height);
  virtual void Finalize(void);
END_APP_DECLARATION()

DEFINE_APP(Triangle, "Triangle")

GLuint VAO;
std::unique_ptr<Program> program;

void Triangle::Initialize(int argc, char** argv, const char* title)
{
  base::Initialize( argc, argv, title );

  glGenVertexArrays( 1, &VAO );

  program = std::make_unique<Program>( "shaders/triangle/triangle.vert", "shaders/triangle/triangle.frag" );
}

void Triangle::Display(bool auto_redraw)
{
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );

  program
    ->Use()
    ->BindVec4( "u_color", 0.0, 0.5, 0.0, 1.0 )
    ;

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
  glfwDestroyWindow( pWindow );
  glfwTerminate();
}
