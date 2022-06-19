#include "vapp.h"

#include <iostream>

BEGIN_APP_DECLARATION(VolumeClouds)
  virtual void Initialize(int argc, char** argv, const char* title);
  virtual void Display(bool auto_redraw);
END_APP_DECLARATION()

DEFINE_APP(VolumeClouds, "Volume Clouds")

void VolumeClouds::Initialize(int argc, char** argv, const char* title)
{
  base::Initialize(argc, argv, title);
}

void VolumeClouds::Display(bool auto_redraw)
{
  double time = app_time();
  std::cout << time << std::endl;
  base::Display(auto_redraw);
}
