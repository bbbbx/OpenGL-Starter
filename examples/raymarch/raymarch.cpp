#include "vapp.h"
#include "LoadShaders.h"
#include "Camera.h"
#include "Axes.h"

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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void readFile(const char* filename, void* data) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    throw std::runtime_error("Failed to open file");
    return;
  }

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);

  fread(data, 1, size, file);
  fclose(file);
}


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


BEGIN_APP_DECLARATION(RayMarch)
  virtual void Initialize(int argc, char** argv, const char* title = 0);
  virtual void Display(bool auto_redraw);
  virtual void OnKey(int key, int scancode, int action, int mods);
  virtual void OnScroll(double xoffset, double yoffset);
  virtual void OnCursorPos(double xpos, double ypos);
  virtual void OnMouseButton(int button, int action, int mods);

  void OnDrag(double dx, double dy);

  bool left_mouse_pressed = false;
  double prev_mouse_x = 0.0;
  double prev_mouse_y = 0.0;
END_APP_DECLARATION()

DEFINE_APP(RayMarch, "Ray March")

static int WindowWidth = 1080;
static int WindowHeight = 720;

static GLuint VBO, VAO;
static GLint topLeftDirLoc, topRightDirLoc, bottomLeftDirLoc, bottomRightDirLoc, cameraPositionLoc;
static GLint planetCenterLoc, innerRadiusLoc;
static GLint viewProjLoc, viewProjInvLoc;
static GLint planetMatrixInvLoc, lightDirectionLoc;
static GLint cloudDisplacementMetersLoc;

static GLint globalAlphaSamplerLoc;
static GLint coverageDetailSamplerLoc;
static GLint noiseVolumeSamplerLoc;

static GLint cloudCoverageFractionLoc;

static GLuint program;

// std::vector<uint8_t> globalAlphaSamplerData;
std::vector<uint8_t> coverageDetailSamplerData;
std::vector<uint8_t> noiseVolumeSamplerData;

static glm::dmat4 planetMatrix = glm::dmat4(1.0);
static glm::dvec3 lightDirection = glm::dvec3(0, 0, -1);
static glm::dvec2 cloudDisplacementMeters(0.0);

double innerRadius = 6600000.0;
glm::dvec3 planetCenter(0.0, 0.0, 0.0);
Camera camera = Camera(double(WindowWidth) / double(WindowHeight));
Axes *axes;


void RayMarch::Initialize(int argc, char** argv, const char* title) {
  window_width = WindowWidth;
  window_height = WindowHeight;
  base::Initialize( argc, argv, title );

  glGenVertexArrays(1, &VAO);
  glBindVertexArray( VAO );

  glGenBuffers( 1, &VBO );

  ShaderInfo shaders[] = {
    // { GL_VERTEX_SHADER, "./shaders/ray_march/common.glsl" },
    { GL_VERTEX_SHADER, "./shaders/ray_march/clouds.vert" },
    // { GL_FRAGMENT_SHADER, "./shaders/ray_march/common.glsl" },
    // { GL_FRAGMENT_SHADER, "./shaders/ray_march/Remap.glsl" },
    // { GL_FRAGMENT_SHADER, "./shaders/ray_march/HenyeyGreenstein.glsl" },
    { GL_FRAGMENT_SHADER, "./shaders/ray_march/clouds.frag" },
    { GL_NONE, nullptr }
  };
  program = LoadShaders( shaders );

  glUseProgram( program );

  topLeftDirLoc = glGetUniformLocation( program, "topLeftDir" );
  topRightDirLoc = glGetUniformLocation( program, "topRightDir" );
  bottomLeftDirLoc = glGetUniformLocation( program, "bottomLeftDir" );
  bottomRightDirLoc = glGetUniformLocation( program, "bottomRightDir" );
  cameraPositionLoc = glGetUniformLocation( program, "cameraPosition" );
  planetCenterLoc = glGetUniformLocation( program, "planetCenter" );
  innerRadiusLoc = glGetUniformLocation( program, "innerRadius" );
  viewProjLoc = glGetUniformLocation( program, "viewProj" );
  viewProjInvLoc = glGetUniformLocation( program, "viewProjInv" );
  planetMatrixInvLoc = glGetUniformLocation( program, "planetMatrixInv" );
  lightDirectionLoc = glGetUniformLocation( program, "lightDirection" );
  cloudDisplacementMetersLoc = glGetUniformLocation( program, "cloudDisplacementMeters" );

  globalAlphaSamplerLoc = glGetUniformLocation( program, "globalAlphaSampler" );
  coverageDetailSamplerLoc = glGetUniformLocation( program, "coverageDetailSampler" );
  noiseVolumeSamplerLoc = glGetUniformLocation( program, "noiseVolumeSampler" );

  cloudCoverageFractionLoc = glGetUniformLocation( program, "cloudCoverageFraction" );



  glBindVertexArray( 0 );

// textures
  // globalAlphaSamplerData.resize(8192 * 4096 * 4);
  int x = 8192, y = 4096, n = 4;
  stbi_uc* globalAlphaSamplerData = stbi_load("./data/globalAlphaSamplerData_8192.png", &x, &y, &n, n);
  if (globalAlphaSamplerData == nullptr) {
    throw std::runtime_error("Failed to load global alpha sampler file");
  }
  GLuint globalAlphaSampler;
  glGenTextures(1, &globalAlphaSampler);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, globalAlphaSampler);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, globalAlphaSamplerData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


  coverageDetailSamplerData.resize( 512 * 512 );
  readFile("./data/CloudNoise-512x512.uint8", coverageDetailSamplerData.data());
  GLuint coverageDetailSampler;
  glGenTextures(1, &coverageDetailSampler);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, coverageDetailSampler);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, coverageDetailSamplerData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


  noiseVolumeSamplerData.resize( 128 * 128 * 128 );
  readFile("./data/CloudVolumeBase-128x128x128.uint8", noiseVolumeSamplerData.data());
  GLuint noiseVolumeSampler;
  glGenTextures(1, &noiseVolumeSampler);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_3D, noiseVolumeSampler);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, 128, 128, 128, 0, GL_RED, GL_UNSIGNED_BYTE, noiseVolumeSamplerData.data());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



  glm::dvec3 eye = glm::dvec3(innerRadius, -innerRadius*2.0, innerRadius);
  camera.setPosition(eye);
  auto up = glm::dvec3(0.0, 0.0, 1.0);
  auto m = glm::inverse( glm::lookAt(eye, glm::dvec3(0.0), up));
  // PrintMat4(m);
  glm::dquat q = glm::toQuat( m );
  glm::normalize(q);

  // glm::quat q = glm::identity<glm::quat>();
  // q = glm::rotate(q, 1.0f, glm::vec3(0.0, 0.0, 1.0));
  camera.setOrientation( q );

  // glm::quat q = glm::quat(glm::radians(glm::vec3(90.0, 0.0, 0.0)));
  // glm::quat p = camera.getOrientation();
  // glm::quat q = glm::toQuat(glm::identity<glm::mat3>());
  // printf("%f %f %f %f\n", q.w, q.x, q.y, q.z);
  // camera.setOrientation( q );

  axes = new Axes(innerRadius * 2.0);
}

void UpdateUniforms() {
  glm::dvec3 cameraPosition = camera.getPosition();
  // glUniform3f(cameraPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.y); // <= 草！cameraPosition 传错了！
  glUniform3f(cameraPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
  glm::dmat4 viewProjInv = glm::inverse(viewProj);

  glUniformMatrix4fv(viewProjLoc, 1, false, glm::value_ptr(glm::fmat4(viewProj)));
  glUniformMatrix4fv(viewProjInvLoc, 1, false, glm::value_ptr(glm::fmat4(viewProjInv)));

  glUniform3f(planetCenterLoc, planetCenter.x, planetCenter.y, planetCenter.z);
  glUniform1f(innerRadiusLoc, innerRadius);
  glm::dmat4 planetMatrixInv = inverse(planetMatrix);
  glUniformMatrix4fv(planetMatrixInvLoc, 1, false, value_ptr(glm::fmat4(planetMatrixInv)));

  glUniform3f(lightDirectionLoc, lightDirection.x, lightDirection.y, lightDirection.z);

  glUniform2f(cloudDisplacementMetersLoc, cloudDisplacementMeters.x, cloudDisplacementMeters.y);

  glUniform1i(globalAlphaSamplerLoc, 0);
  glUniform1i(coverageDetailSamplerLoc, 1);
  glUniform1i(noiseVolumeSamplerLoc, 2);

  glUniform1f(cloudCoverageFractionLoc, 0.5);

  // transform NDC of 4 corners to world space
  glm::dvec4 c00 = viewProjInv * glm::dvec4(-1.0, -1.0, 0.5, 1.0);
  c00 /= c00.w;
  glm::dvec4 c10 = viewProjInv * glm::dvec4( 1.0, -1.0, 0.5, 1.0);
  c10 /= c10.w;
  glm::dvec4 c01 = viewProjInv * glm::dvec4(-1.0,  1.0, 0.5, 1.0);
  c01 /= c01.w;
  glm::dvec4 c11 = viewProjInv * glm::dvec4( 1.0,  1.0, 0.5, 1.0);
  c11 /= c11.w;

  glm::dvec3 dir00 = glm::dvec3(c00) - cameraPosition;
  dir00 = glm::normalize(dir00);
  glm::dvec3 dir10 = glm::dvec3(c10) - cameraPosition;
  dir10 = glm::normalize(dir10);
  glm::dvec3 dir01 = glm::dvec3(c01) - cameraPosition;
  dir01 = glm::normalize(dir01);
  glm::dvec3 dir11 = glm::dvec3(c11) - cameraPosition;
  dir11 = glm::normalize(dir11);
  // printf("%f, %f, %f\n", dir00.x, dir00.y, dir00.z);
  // PrintVec3(cameraPosition);

  // Camera points to corners
  glUniform3f( bottomLeftDirLoc, dir00.x, dir00.y, dir00.z );
  glUniform3f( bottomRightDirLoc, dir10.x, dir10.y, dir10.z );
  glUniform3f( topLeftDirLoc, dir01.x, dir01.y, dir01.z );
  glUniform3f( topRightDirLoc, dir11.x, dir11.y, dir11.z );
}

void RayMarch::Display(bool auto_redraw) {
  glDepthRange(0.0, 1.0);
  glClearDepth(1.0);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  glBindVertexArray( VAO );
  glUseProgram(program);
  UpdateUniforms();
  glDrawArrays( GL_TRIANGLES, 0, 6 );

  glEnable(GL_DEPTH_TEST);
  axes->Draw(camera);

  base::Display( auto_redraw );
}

void RayMarch::OnKey(int key, int scancode, int action, int mods) {
  // std::cout << key << " " << scancode << " " << action <<  std::endl;
  // return;

  if (action == GLFW_PRESS) return;

  glm::dvec3 eyePos = camera.getPosition();
  double magnitude = glm::length( eyePos );
  glm::dmat4 rotation = glm::toMat4( camera.getOrientation() );
  glm::dvec3 xAxis = glm::dvec3( glm::column(rotation, 0) );
  glm::dvec3 yAxis = glm::dvec3( glm::column(rotation, 1) );
  glm::dvec3 zAxis = glm::dvec3( glm::column(rotation, 2) );
  glm::dvec3 movement = glm::dvec3( 0.0 );
  switch (key)
  {
  case GLFW_KEY_A: {
    movement = - xAxis * magnitude * 0.05;
    break;
  }
  case GLFW_KEY_D: {
    movement = xAxis * magnitude * 0.05;
    break;
  }
  case GLFW_KEY_W: {
    movement = - zAxis * magnitude * 0.05;
    break;
  }
  case GLFW_KEY_S: {
    movement = zAxis * magnitude * 0.05;
    break;
  }
  case GLFW_KEY_Q: {
    movement = - yAxis * magnitude * 0.05;
    break;
  }
  case GLFW_KEY_E: {
    movement = yAxis * magnitude * 0.05;
    break;
  }
  default:
    break;
  }

  camera.setPosition(eyePos + movement);
}

void RayMarch::OnScroll(double xoffset, double yoffset) {
  // printf("Scroll: %f, %f\n", xoffset, yoffset);

  auto cameraMatrix = glm::inverse( camera.getViewMatrix() );
  auto zAxis = glm::column(cameraMatrix, 2);
  auto dir = - glm::dvec3( zAxis );
  dir = glm::normalize(dir);

  auto eye = camera.getPosition();
  auto magnitude = glm::length(eye);
  camera.setPosition(eye + dir * yoffset * (magnitude * 0.05));
}

void RayMarch::OnDrag(double dx, double dy) {
  // printf("OnDrag: %f, %f\n", dx, dy);

  glm::dmat4 rotation = glm::toMat4( camera.getOrientation() );
  glm::dvec3 xAxis = glm::dvec3( glm::column(rotation, 0) );
  glm::dvec3 yAxis = glm::dvec3( glm::column(rotation, 1) );
  glm::dvec3 zAxis = glm::dvec3( glm::column(rotation, 2) );

  glm::dquat quat1 = glm::angleAxis(glm::radians(-dx * 0.3), yAxis);
  glm::dquat quat2 = glm::angleAxis(glm::radians(-dy * 0.3), xAxis);

  camera.setOrientation( quat1 * quat2 * camera.getOrientation() );
}

void RayMarch::OnCursorPos(double xpos, double ypos) {
  if (left_mouse_pressed) {
    double dx = xpos - prev_mouse_x;
    double dy = ypos - prev_mouse_y;

    OnDrag(dx, dy);
  }

  prev_mouse_x = xpos;
  prev_mouse_y = ypos;
}

void RayMarch::OnMouseButton(int button, int action, int mods) {
  left_mouse_pressed = action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT;
}
