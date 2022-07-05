#include "vapp.h"
#include <stdexcept>
#include <iostream>

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

void VApplication::scrollr_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  VApplication* pThis = (VApplication*)glfwGetWindowUserPointer( window );
  pThis->OnScroll( xoffset, yoffset );
}

void VApplication::cursorpos_callback(GLFWwindow* window, double xpos, double ypos)
{
  VApplication* pThis = (VApplication*)glfwGetWindowUserPointer( window );
  pThis->OnCursorPos( xpos, ypos );
}

void VApplication::mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{
  VApplication* pThis = (VApplication*)glfwGetWindowUserPointer( window );
  pThis->OnMouseButton( button, action, mods );
}

/**
 * @brief Return elapsed time in seconds
 * 
 * @return double 
 */
double VApplication::app_time()
{
  return (double)timer.count<std::chrono::milliseconds>() / 1000.0;
}

void errorCallback(int errorCode, const char* description) {
  std::cerr << "GLFW error callback: " << description << std::endl;
}

void VApplication::Initialize( int argc, char** argv, const char* title )
{
  timer.start();

  glfwSetErrorCallback( errorCallback );

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

  pWindow = glfwCreateWindow( window_width, window_height, title ? title : "title", nullptr, nullptr );
  if ( !pWindow ){
    throw std::runtime_error( "GLFW create window failed!\n" );
  }

  glfwSetWindowUserPointer( pWindow, this );
  glfwSetWindowSizeCallback( pWindow, window_resize_callback );
  glfwSetKeyCallback( pWindow, key_callback );
  glfwSetCharCallback( pWindow, char_callback );
  glfwSetScrollCallback( pWindow, scrollr_callback );
  glfwSetCursorPosCallback( pWindow, cursorpos_callback );
  glfwSetMouseButtonCallback( pWindow, mousebutton_callback );

  glfwMakeContextCurrent( pWindow );

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  int fb_width, fb_height;
  glfwGetFramebufferSize( pWindow, &fb_width, &fb_height );
  Resize( fb_width, fb_height );
}
