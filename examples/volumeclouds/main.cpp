#include <iostream>
#include <array>
#include <vector>
#include <cstdlib>

// #define GLM_MESSAGES GLM_ENABLE
#define GLM_FORCE_PRECISION_HIGHP_DOUBLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/ext/quaternion_common.hpp"
// #include "glm/gtc/quaternion.inl"
// #include "glm/gtc/quaternion.hpp"
// #include "glm/gtc/matrix_inverse.hpp"
// #include "glm/gtx/quaternion.inl"
#include "glm/gtx/quaternion.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

#include "vapp.h"
#include "Camera.h"
#include "Program.h"

#include "Axes.h"
// #include "Ellipsoid.h"
#include "Planet.h"
#include "VolumeClouds.h"

#include "FirstPersonCameraController.h"

namespace
{
#define Print(x) (std::cout << (x) << std::endl)

void PrintMat4(glm::dmat4 &m) {
  std::cout
    << "[ " << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ",\n"
    << "  " << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ",\n"
    << "  " << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ",\n"
    << "  " << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << " ]\n";
}
void PrintVec3(glm::dvec3 &v) {
  std::cout
    << "( " << v.x << ", " << v.y << ", " << v.z << " )\n";
}
} // namespace anonymous


BEGIN_APP_DECLARATION(VolumeCloudsDemo)
  void Initialize(int argc, char** argv, const char* title = 0);
  void Display(bool auto_redraw);
  void Finalize();

  void OnKey(int key, int scancode, int action, int mods);
  void OnScroll(double xoffset, double yoffset);
  void OnCursorPos(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);

  void OnDrag(double dx, double dy);

  void UpdateScene(Camera& camera, double time, double dt);
  void DrawScene(Camera& camera);

  const int WindowWidth = 1080;
  const int WindowHeight = 720;

  // glm::dmat4 planetMatrix = glm::dmat4(1.0);
  glm::dvec3 lightDirection = glm::dvec3(1, 0, 0);
  // glm::dvec2 cloudDisplacementMeters(0.0);

  double innerRadius = 6371000.0;
  // glm::dvec3 planetCenter(0.0, 0.0, 0.0);

  double previousTime = 0.0;

  Camera camera = Camera( double(WindowWidth) / double(WindowHeight) );
  FirstPersonCameraController cameraController = FirstPersonCameraController( camera );

  Axes *axes;
  Planet *planet;
  VolumeClouds *volumeClouds;
END_APP_DECLARATION()

DEFINE_APP(VolumeCloudsDemo, "Volume Clouds Demo")


void VolumeCloudsDemo::Initialize(int argc, char** argv, const char* title) {
  window_width = WindowWidth;
  window_height = WindowHeight;
  base::Initialize( argc, argv, title );

  glfwSetInputMode( pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

  // glm::dvec3 eye = glm::dvec3(innerRadius, -innerRadius*2.0, innerRadius);
  glm::dvec3 eye = glm::dvec3(-2357405.7602848653, 5346768.021286738, 2550545.893531361);
  glm::dvec3 direction = glm::dvec3(0.19708575403713477, -0.3481966525125323, 0.9164694739786378);
  glm::dvec3 target = eye + 10.0 * direction;

  auto up = glm::dvec3( 0.0, 0.0, 1.0 );
  auto m = glm::inverse( glm::lookAt( eye, target, up ) );
  // PrintMat4(m);
  glm::dquat orientation = glm::toQuat( m );
  // glm::normalize( orientation );

  // glm::quat q = glm::identity<glm::quat>();
  // q = glm::rotate(q, 1.0f, glm::vec3(0.0, 0.0, 1.0));
  camera.setPosition( eye );
  camera.setOrientation( orientation );

  // glm::quat q = glm::quat(glm::radians(glm::vec3(90.0, 0.0, 0.0)));
  // glm::quat p = camera.getOrientation();
  // glm::quat q = glm::toQuat(glm::identity<glm::mat3>());
  // printf("%f %f %f %f\n", q.w, q.x, q.y, q.z);
  // camera.setOrientation( q );

  lightDirection = glm::normalize( eye );

  axes = new Axes( innerRadius * 2.0 );
  planet = new Planet( innerRadius );
  volumeClouds = new VolumeClouds();
  volumeClouds->lightDirection = lightDirection;


  previousTime = app_time();
}

void VolumeCloudsDemo::UpdateScene(Camera& camera, double time, double dt) {

  cameraController.Update( dt );

  volumeClouds->Update( camera, time, dt );
  planet->Update( camera, time, dt );

}

void VolumeCloudsDemo::DrawScene(Camera& camera) {
  glClearDepth( 1.0 );
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  volumeClouds->Draw( camera );

  // planet->Draw( camera );

  axes->Draw( camera );
}

void VolumeCloudsDemo::Display(bool auto_redraw) {
  double time = app_time();
  double dt = time - previousTime;

  UpdateScene( camera, time, dt );
  DrawScene( camera );

  base::Display( auto_redraw );

  previousTime = time;
}

void VolumeCloudsDemo::Finalize() {
  delete planet;
  delete axes;
  delete volumeClouds;
}


void VolumeCloudsDemo::OnKey(int key, int scancode, int action, int mods) {
  // std::cout << key << " " << scancode << " " << action << " " << mods << std::endl;
  // return;

  bool isKeyDown = action == GLFW_PRESS || action == GLFW_REPEAT;
  if ( isKeyDown ) {
    cameraController.HandleKeyDown( key, mods );
  } else {
    cameraController.HandleKeyUp( key, mods );
  }

}

void VolumeCloudsDemo::OnScroll(double xoffset, double yoffset) {
  // printf("Scroll: %f, %f\n", xoffset, yoffset);

  auto cameraMatrix = glm::inverse( camera.getViewMatrix() );
  auto zAxis = glm::column(cameraMatrix, 2);
  auto dir = - glm::dvec3( zAxis );
  dir = glm::normalize(dir);

  auto eye = camera.getPosition();
  auto magnitude = glm::length(eye);
  camera.setPosition(eye + dir * yoffset * (magnitude * 0.05));
}

void VolumeCloudsDemo::OnDrag(double dx, double dy) {
  // printf("OnDrag: %f, %f\n", dx, dy);

  glm::dmat4 rotation = glm::toMat4( camera.getOrientation() );
  glm::dvec3 right = glm::dvec3( glm::column(rotation, 0) );
  glm::dvec3 up = glm::dvec3( glm::column(rotation, 1) );
  // glm::dvec3 zAxis = glm::dvec3( glm::column(rotation, 2) );

  glm::dquat quat1 = glm::angleAxis(glm::radians(-dx * 0.3), up);
  glm::dquat quat2 = glm::angleAxis(glm::radians(-dy * 0.3), right);

  camera.setOrientation( quat1 * quat2 * camera.getOrientation() );
}

void VolumeCloudsDemo::OnCursorPos(double x, double y) {

  cameraController.HandleCursorMove( x, y );

}

void VolumeCloudsDemo::OnMouseButton(int button, int action, int mods) {
}
