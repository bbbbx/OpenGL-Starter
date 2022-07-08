#ifndef __GUI_H__
#define __GUI_H__

#include "Renderable.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>

class GUI : public Renderable
{
private:
  GLFWwindow* pWindow;
public:
  std::function<void()> drawer = []{
    std::cerr << "GUI: Please set drawer member" << std::endl;
  };
  GUI() = delete;
  GUI(GLFWwindow* window);
  ~GUI();

  void Update(Camera &camera, double time, double deltaTime) override;
  void Draw(Camera &camera) override;
};

#endif // __GUI_H__
