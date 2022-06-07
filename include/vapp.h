#ifndef __VAPP_H__
#define __VAPP_H__

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <chrono>


class VApplication
{
protected:
  inline VApplication(void) {}
  virtual ~VApplication(void) {}
  static VApplication* s_app;
  GLFWwindow* pWindow;

  // struct timeval appStartTime;
  std::chrono::system_clock::time_point appStartTime;

  static void window_resize_callback(GLFWwindow* window, int width, int height);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void char_callback(GLFWwindow* window, unsigned int codepoint);
  float app_time();
  
public:
  void MainLoop(void);

  virtual void Initialize(const char* title = 0);

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
  app->Initialize(title);                                           \
  app->MainLoop();                                                  \
  app->Finalize();                                                  \
  return 0;                                                         \
}

#endif // __VAPP_H__
