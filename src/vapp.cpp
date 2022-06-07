#include "vapp.h"
#include <stdexcept>

void VApplication::window_resize_callback(GLFWwindow* window, int width, int height)
{
  VApplication* pThis = (VApplication*)glfwGetWindowUserPointer( window );
  glfwGetFramebufferSize( window, &width, &height );
  pThis->Resize( width, height );
}

void VApplication::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  VApplication* pThis = (VApplication*)glfwGetWindowUserPointer( window );
  pThis->OnKey( key, scancode, action, mods );
}

void VApplication::char_callback(GLFWwindow* window, unsigned int codepoint)
{
  VApplication* pThis = (VApplication*)glfwGetWindowUserPointer( window );
  pThis->OnChar( codepoint );
}

float VApplication::app_time()
{
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - appStartTime);
  return (double)duration.count() / 1000.0;
}

void VApplication::Initialize( const char* title )
{
  appStartTime = std::chrono::system_clock::now();

  if ( glfwInit() == GLFW_FALSE ) {
    throw std::runtime_error( "GLFW init failed!\n" );
  }

  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif

  pWindow = glfwCreateWindow( 1280, 720, title ? title : "title", nullptr, nullptr );
  if ( !pWindow ){
    throw std::runtime_error( "GLFW create window failed!\n" );
  }

  glfwSetWindowUserPointer( pWindow, this );
  glfwSetWindowSizeCallback( pWindow, window_resize_callback );
  glfwSetKeyCallback( pWindow, key_callback );
  glfwSetCharCallback( pWindow, char_callback );

  glfwMakeContextCurrent( pWindow );

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  int width, height;
  glfwGetFramebufferSize( pWindow, &width, &height );
  Resize( width, height );
}
