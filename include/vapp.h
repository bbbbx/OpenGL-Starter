#ifndef __VAPP_H__
#define __VAPP_H__

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <chrono>


class VApplication
{
protected:
  inline VApplication(void) : window_width(1280), window_height(720) {}
  virtual ~VApplication(void) {}
  static VApplication* s_app;
  GLFWwindow* pWindow;
  int window_width;
  int window_height;

  // struct timeval appStartTime;
  std::chrono::system_clock::time_point appStartTime;

  static void window_resize_callback(GLFWwindow* window, int width, int height);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void char_callback(GLFWwindow* window, unsigned int codepoint);
  static void scrollr_callback(GLFWwindow* window, double xoffset, double yoffset);
  static void cursorpos_callback(GLFWwindow* window, double xoffset, double yoffset);
  static void mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
  double app_time();

public:
  void MainLoop(void);

  virtual void Initialize(int argc, char** argv, const char* title = 0);

  virtual void Display(bool auto_redraw = true)
  {
    glfwSwapBuffers(pWindow);
  }

  virtual void Finalize(void) {}

  virtual void Resize(int width, int height)
  {
    glViewport(0, 0, width, height);
  }

  virtual void OnKey(int key, int scancode, int action, int mods) {}
  virtual void OnChar(unsigned int codepoint) {}
  virtual void OnScroll(double xoffset, double yoffset) {}
  virtual void OnCursorPos(double xpos, double ypos) {}
  virtual void OnMouseButton(int button, int action, int mods) {}
};


#define BEGIN_APP_DECLARATION(appclass)                     \
class appclass : public VApplication                        \
{                                                           \
public:                                                     \
  typedef class VApplication base;                          \
  static VApplication* Create(void)                         \
  {                                                         \
    return (s_app = new appclass);                          \
  }

#define END_APP_DECLARATION()                               \
};


#define MAIN_DECL int main(int argc, char** argv)

#define DEFINE_APP(appclass, title)                                 \
VApplication* VApplication::s_app;                                  \
                                                                    \
void VApplication::MainLoop(void)                                   \
{                                                                   \
  do                                                                \
  {                                                                 \
    Display();                                                      \
    glfwPollEvents();                                               \
  } while (!glfwWindowShouldClose(pWindow));                        \
}                                                                   \
                                                                    \
MAIN_DECL                                                           \
{                                                                   \
  VApplication* app = appclass::Create();                           \
  app->Initialize(argc, argv, title);                               \
  app->MainLoop();                                                  \
  app->Finalize();                                                  \
  return 0;                                                         \
}

#endif // __VAPP_H__
