#include <iostream>
#include <array>
#include <vector>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>

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
#include "Ellipsoid.h"
#include "Planet.h"
#include "VolumeClouds.h"
#include "BruentonAtmosphere.h"
#include "ScreenQuad.h"

#include "FirstPersonCameraController.h"

#include "text_renderer.h"
#include "TextureDebugger.h"
#include "Transforms.h"

#include "GLUtils.h"

#include "GUI.h"
#include <imgui.h>

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


void readFile(const char* filename, void* data) {
#ifdef WIN32
  FILE *file;
  fopen_s( &file, filename, "rb" );
#else
  FILE *file = fopen(filename, "rb");
#endif

  if (!file) {
    std::cerr << "Failed to open file" << std::endl;
    throw std::runtime_error("Failed to open file");
    return;
  }

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);

  fread(data, 1, size, file);
  fclose(file);
}
} // namespace anonymous


BEGIN_APP_DECLARATION(VolumeCloudsDemo)
  void Initialize(int argc, char** argv, const char* title = 0);
  void Display(bool auto_redraw);
  void Finalize();

  void OnKey(int key, int scancode, int action, int mods);
  void OnScroll(double xoffset, double yoffset);
  void OnCursorPos(double xpos, double ypos);

  void UpdateScene(Camera& camera, double time, double dt);
  void DrawScene(Camera& camera);

  const int WindowWidth = 780;
  const int WindowHeight = 512;

  // glm::dmat4 planetMatrix = glm::dmat4(1.0);
  glm::dvec3 lightDirection = glm::dvec3( -1.0, 0.0, 0.0 );
  // glm::dvec2 cloudDisplacementMeters(0.0);

  double innerRadius = 6369344.0;
  // glm::dvec3 planetCenter(0.0, 0.0, 0.0);

  double previousTime = 0.0;

  Camera camera = Camera( double(WindowWidth) / double(WindowHeight) );
  FirstPersonCameraController cameraController = FirstPersonCameraController( camera );

  Axes *axes;
  Planet *planet;
  VolumeClouds *volumeClouds;
  BruentonAtmosphere *atmosphere;
  ScreenQuad *screenQuad = nullptr;
  ScreenQuad *finalCompositeScreenQuad = nullptr;
  TextureDebugger *textureDebugger;
  int DEBUG = 0;
  double FPS = 0.0;
  std::unordered_map<int, GLuint> debugTextureMap;

  std::unique_ptr<TextRenderer> textRenderer;

  GUI* gui = nullptr;

  GLuint framebuffer = 0;
  GLuint colorTexture = 0;
  GLuint depthTexture = 0;

  GLuint transmittance = 0;
  GLuint scattering = 0;
  GLuint irradiance = 0;

END_APP_DECLARATION()

DEFINE_APP(VolumeCloudsDemo, "Volume Clouds Demo")

Axes *debugModelMatrix;

void VolumeCloudsDemo::Initialize(int argc, char** argv, const char* title) {
  window_width = WindowWidth;
  window_height = WindowHeight;
  base::Initialize( argc, argv, title );

  glfwSetInputMode( pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
  glfwSetWindowPos( pWindow, 0, 0 );

  std::vector<GLint> viewport( 4 );
  glGetIntegerv( GL_VIEWPORT, viewport.data() );
  int width = viewport[2];
  int height = viewport[3];


  colorTexture = GLUtils::NewTexture2D( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
  depthTexture = GLUtils::NewTexture2D( width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
  std::vector<GLuint> colorTextures = { colorTexture };
  framebuffer = GLUtils::NewFramebuffer( colorTextures, depthTexture );

  glm::dvec3 eye = Ellipsoid::phiThetaToXYZ( glm::radians( 90.0 - 23.0 ), glm::radians( 113.0 ), innerRadius + 1000.0 );
  camera.setPosition( eye );

  glm::dmat4 toFixedFrame = Transforms::eastNorthUpToFixedFrame( eye );

  glm::dvec4 east =  glm::column( toFixedFrame, 0 );
  glm::dvec4 north = glm::column( toFixedFrame, 1 );
  glm::dvec4 up = glm::column( toFixedFrame, 2 );
  glm::dmat4 m = glm::dmat4( east, up, - north, glm::dvec4( 0.0, 0.0, 0.0, 1.0 ) );

  glm::dquat orientation = glm::toQuat( m );
  camera.setOrientation( orientation );

  // transmittance
  std::vector<float> transmittanceData(256 * 64 * 4);
  // std::unique_ptr<float[]> transmittanceData = std::make_unique<float[]>(256 * 64 * 4);
  std::string filename( "./data/transmittance.dat" );
  readFile( filename.c_str(), transmittanceData.data() );

  transmittance = GLUtils::NewTexture2D( 256, 64,
    GL_RGBA32F, GL_RGBA, GL_FLOAT, transmittanceData.data(),
    GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

// scattering
  std::vector<float> scatteringData(256 * 128 * 32 * 4);
  // float *scatteringData = new float[256 * 128 * 32 * 4];
  // FIXME: 👇 new at stack, has size limitation
  // float scatteringData[256 * 128 * 32 * 4];
  filename = std::string( "./data/scattering.dat" );
  readFile( filename.c_str(), scatteringData.data() );

  glGenTextures(1, &scattering);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, scattering);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, 256, 128, 32, 0, GL_RGBA, GL_FLOAT, scatteringData.data());

  glBindTexture(GL_TEXTURE_3D, 0);

// irradiance
  std::vector<float> irradianceData(64 * 16 * 4);
  filename = std::string( "./data/irradiance.dat" );
  readFile( filename.c_str(), irradianceData.data() );
  irradiance = GLUtils::NewTexture2D( 64, 16,
    GL_RGBA32F, GL_RGBA, GL_FLOAT, irradianceData.data(),
    GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

  debugModelMatrix = new Axes( 1.0 );
  debugModelMatrix->modelMatrix = glm::scale( toFixedFrame, glm::dvec3( 10000.0 ) );

  // lightDirection = glm::normalize( eye );

  axes = new Axes( innerRadius * 2.0 );

  volumeClouds = new VolumeClouds();
  volumeClouds->lightDirection = lightDirection;
  volumeClouds->SetTransmittanceSampler( transmittance );
  volumeClouds->SetScatteringSampler( scattering );
  volumeClouds->SetIrradianceSampler( irradiance );

  
  planet = new Planet( innerRadius );
  planet->transmittance = transmittance;
  planet->scattering = scattering;
  planet->irradiance = irradiance;
  planet->cloudSampler = volumeClouds->GetGlobalAlphaSampler();
  planet->cloudDetail2d = volumeClouds->GetCoverageDetailSampler();
  planet->lightDirection = lightDirection;

  double bottomRadius = innerRadius;
  double topRadius = 6537590;
  atmosphere = new BruentonAtmosphere( bottomRadius, topRadius );
  atmosphere->SetTransmittanceSampler( transmittance );
  atmosphere->SetScatteringSampler( scattering );
  atmosphere->SetIrradianceSampler( irradiance );
  atmosphere->SetSunDirection( lightDirection );

  screenQuad = new ScreenQuad( "./shaders/volumeclouds/blit.frag" );
  finalCompositeScreenQuad = new ScreenQuad( "./shaders/FinalComposite.frag" );

  textureDebugger = new TextureDebugger();

  textRenderer.reset( new TextRenderer );
  textRenderer->SetColor( 1.0, 0.0, 1.0 );

#ifdef __APPLE__
  textRenderer->SetCharScale( 2 );
#else
  textRenderer->SetCharScale( 2 );
#endif

  gui = new GUI( pWindow );
  gui->drawer = [&]() {
    auto cameraPosition = camera.getPosition();
    double cameraAltitude = glm::length( cameraPosition ) - innerRadius;
    glm::dvec3 phiThetaRadius = Ellipsoid::xyzToPhiTheta( cameraPosition );
    double longitudeDegrees = glm::degrees( phiThetaRadius.x );
    double latitudeDegrees = 90.0 - glm::degrees( phiThetaRadius.y );

    ImGui::Begin( "GUI" );
    if ( ImGui::CollapsingHeader( "Info", ImGuiTreeNodeFlags_DefaultOpen ) ) {
      ImGui::Text( "Camera Cartographic: %f, %f, %f", longitudeDegrees, latitudeDegrees, cameraAltitude );
      ImGui::Text( "FPS: %.0f", FPS );
      ImGui::Text( "Debug Texture: %d", DEBUG );
    }

    if ( ImGui::CollapsingHeader( "Uniforms", ImGuiTreeNodeFlags_DefaultOpen ) ) {
      ImGui::Checkbox( "DEBUG VOLUME", &(volumeClouds->uniforms.DEBUG_VOLUME) );
      ImGui::Checkbox( "ENABLE_HIGH_DETAIL_CLOUDS", &(volumeClouds->uniforms.ENABLE_HIGH_DETAIL_CLOUDS) );
      ImGui::SliderFloat2( "cloudTopZeroDensityHeight", volumeClouds->uniforms.cloudTopZeroDensityHeight, 2000.0f, 12000.0f );
      ImGui::SliderFloat2( "cloudBottomZeroDensity", volumeClouds->uniforms.cloudBottomZeroDensity, 2000.0f, 9000.0f );
      ImGui::SliderFloat2( "cloudOcclusionStrength", volumeClouds->uniforms.cloudOcclusionStrength, 0.0f, 1.0f );
      ImGui::SliderFloat2( "cloudDensityMultiplier", volumeClouds->uniforms.cloudDensityMultiplier, 0.0f, 1.0f );
      ImGui::SliderFloat( "noiseFrequencyScale", &volumeClouds->uniforms.noiseFrequencyScale, 0.0f, 0.01f, "%.5f" );
      ImGui::SliderFloat( "powderStrength", &(volumeClouds->uniforms.powderStrength), 0.0f, 1.0f );
      ImGui::SliderFloat( "scatterSampleDistanceScale", &(volumeClouds->uniforms.scatterSampleDistanceScale), 0.0f, 100.0f );
      ImGui::SliderFloat( "scatterDistanceMultiplier", &(volumeClouds->uniforms.scatterDistanceMultiplier), 0.0f, 4.0f );
      ImGui::SliderFloat( "cloudChaos", &(volumeClouds->uniforms.cloudChaos), 0.0f, 4.0f );
    }

    if ( ImGui::CollapsingHeader( "Params", ImGuiTreeNodeFlags_DefaultOpen ) ) {
      ImGui::SliderFloat( "windSpeed", &(volumeClouds->windSpeed), 10.0f, 400.0f );
    }

    ImGui::End();
  };

  previousTime = app_time();
}

void VolumeCloudsDemo::UpdateScene(Camera& camera, double time, double dt) {
  // Update pre second
  if ( std::floor(time) - std::floor(previousTime) ) {
    FPS = std::floor( 1.0 / dt );
  }

  cameraController.Update( dt );

  volumeClouds->Update( camera, time, dt );

  debugTextureMap[0] = 0;
  debugTextureMap[1] = volumeClouds->GetCloudColorTexture();
  debugTextureMap[2] = volumeClouds->GetCloudDepthTexture();
  debugTextureMap[3] = depthTexture;
  textureDebugger->SetTexture( debugTextureMap[ DEBUG ] );

  planet->Update( camera, time, dt );

}

void VolumeCloudsDemo::DrawScene(Camera& camera) {
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, framebuffer );
  // glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
  glClearDepth( 1.0 );
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  // glDisable( GL_BLEND );
  // glDisable( GL_DEPTH_TEST );
  // glDisable( GL_CULL_FACE );

  volumeClouds->PreComputeClouds( camera );

  atmosphere->Draw( camera );
  planet->Draw( camera );
  axes->Draw( camera );

  debugModelMatrix->Draw( camera );
  volumeClouds->Draw( camera );
  textureDebugger->Draw( camera );

  auto cameraPosition = camera.getPosition();
  double cameraAltitude = glm::length( cameraPosition ) - innerRadius;
  std::stringstream help;
  glm::dvec3 phiThetaRadius = Ellipsoid::xyzToPhiTheta( cameraPosition );
  double longitudeDegrees = glm::degrees( phiThetaRadius.x );
  double latitudeDegrees = 90.0 - glm::degrees( phiThetaRadius.y );
  double height = phiThetaRadius.z - innerRadius;
  // help << "Camera Cartesian: " << std::to_string ( cameraPosition.x ) << ", " << std::to_string ( cameraPosition.y ) << ", " << std::to_string ( cameraPosition.z ) << "\n"
  //      << "Camera Cartographic: " << std::to_string ( longitudeDegrees ) << ", " << std::to_string ( latitudeDegrees ) << ", " << std::to_string ( height ) << "\n"
  //      << "Camera Altitude: " << cameraAltitude << "\n"
  //      << "Debug Texture: " << DEBUG << "\n"
  //      << "FPS: " << FPS << "\n"
  //      ;

  // textRenderer->DrawText( help.str(), 5, 4 );

// FIXME: can not blit to default framebuffer!!!
  // glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
  // glBindFramebuffer( GL_READ_FRAMEBUFFER, framebuffer );
  // std::vector<GLuint> buf = { GL_COLOR_ATTACHMENT0 };
  // // glDrawBuffers( 1, buf.data() );
  // glBlitFramebuffer(
  //   0, 0, WindowWidth, WindowHeight,
  //   0, 0, WindowWidth, WindowHeight,
  //   GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );
  // FIXME: Rebug 和 Release 的表现不一样！！
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
  glClearDepth( 1.0 );
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  // glDepthMask( GL_FALSE );
  // glDisable( GL_BLEND );
  // glDisable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  // auto program = screenQuad->GetProgram();
  // program
  //   ->Use()
  //   ->BindTexture2D( "u_texture", colorTexture, 0 )
  //   ;
  // screenQuad->Draw( camera );
  // return;

  auto finalCompositeProgram = finalCompositeScreenQuad->GetProgram();
  finalCompositeProgram
    ->Use()
    ->BindTexture2D( "inputSampler", colorTexture, 0 )
    ;
  finalCompositeScreenQuad->Draw( camera );

  gui->Draw( camera );

  GLenum error = glGetError();
  assert( error == GL_NO_ERROR );
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

    switch ( key )
    {
    case GLFW_KEY_H:
      DEBUG = ( DEBUG + 1 ) % debugTextureMap.size();
      break;
    case GLFW_KEY_ESCAPE:
      // int inputMode = glfwGetInputMode( pWindow, GLFW_CURSOR );
      // if ( inputMode == GLFW_CURSOR_DISABLED ) {
      //   glfwSetInputMode( pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
      // } else {
      //   glfwSetInputMode( pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
      // }
      cameraController.inputEnabled = ! cameraController.inputEnabled;

      glfwSetInputMode( pWindow, GLFW_CURSOR, cameraController.inputEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
      break;
    }

  } else {
    cameraController.HandleKeyUp( key, mods );
  }

}

void VolumeCloudsDemo::OnScroll(double xoffset, double yoffset) {
  // printf("Scroll: %f, %f\n", xoffset, yoffset);
  if ( ! cameraController.inputEnabled ) return;

  auto dir = camera.getDirection();
  auto eye = camera.getPosition();
  auto altitude = glm::length(eye) - innerRadius;
  camera.setPosition(eye + dir * yoffset * (altitude * 0.1));
}

void VolumeCloudsDemo::OnCursorPos(double x, double y) {
  cameraController.HandleCursorMove( x, y );
}
