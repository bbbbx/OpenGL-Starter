#include "VolumeClouds.h"

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <iostream>
#include <memory>

namespace
{
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


VolumeClouds::VolumeClouds(/* args */)
{
  glGenVertexArrays(1, &vao);
  glGenBuffers( 1, &vbo );

  program = new Program("./shaders/ray_march/clouds.vert", "./shaders/ray_march/clouds.frag");

// textures
  int x = 8192, y = 4096, n = 4;
  stbi_uc* globalAlphaSamplerData = stbi_load("./data/cloud_combined_8192.png", &x, &y, &n, n);
  if (globalAlphaSamplerData == nullptr) {
    throw std::runtime_error("Failed to load global alpha sampler file");
  }
  glGenTextures(1, &globalAlphaSampler);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, globalAlphaSampler);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, globalAlphaSamplerData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // GL_NEAREST_MIPMAP_LINEAR
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(globalAlphaSamplerData);


  // readFile("./data/CloudNoise-512x512.uint8", coverageDetailSamplerData.data());
  x = 512;
  y = 512;
  n = 4;
  stbi_uc* coverageDetailSamplerData = stbi_load("./data/Noise.png", &x, &y, &n, n);
  if (coverageDetailSamplerData == nullptr) {
    throw std::runtime_error("Failed to load global alpha sampler file");
  }
  glGenTextures(1, &coverageDetailSampler);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, coverageDetailSampler);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, coverageDetailSamplerData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(coverageDetailSamplerData);

  // readFile("./data/CloudVolumeBase-128x128x128.uint8", noiseVolumeSamplerData.data());
  x = 128;
  y = 128 * 128;
  n = 4;
  stbi_uc* noiseVolumeSamplerData = stbi_load("./data/CloudVolumeBase.png", &x, &y, &n, n);
  if (noiseVolumeSamplerData == nullptr) {
    throw std::runtime_error("Failed to load global alpha sampler file");
  }
  glGenTextures(1, &noiseVolumeSampler);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, noiseVolumeSampler);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 128, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseVolumeSamplerData);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, 0);
  stbi_image_free(noiseVolumeSamplerData);

// transmittance
  std::vector<float> transmittanceData(256 * 64 * 4);
  // std::unique_ptr<float[]> transmittanceData = std::make_unique<float[]>(256 * 64 * 4);
  std::string filename( "./data/transmittance.dat" );
  readFile( filename.c_str(), transmittanceData.data() );

  glGenTextures(1, &transmittance);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, transmittance);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 64, 0, GL_RGBA, GL_FLOAT, transmittanceData.data());

  glBindTexture(GL_TEXTURE_2D, 0);

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

  glGenTextures(1, &irradiance);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, irradiance);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 64, 16, 0, GL_RGBA, GL_FLOAT, irradianceData.data());

  glBindTexture(GL_TEXTURE_2D, 0);
}

VolumeClouds::~VolumeClouds()
{
}

void VolumeClouds::Update(Camera &camera, double time, double deltaTime) {
  double julianDateSeconds = time * 10.0;// mJulianDate * 24.0 * 60.0 * 60.0;
  // Move clouds
  {
    float windSpeed = 10.0;
    cloudDisplacementMeters.y = -20000.0 + std::fmod( julianDateSeconds, 40000 ) * windSpeed;
  }
}

void VolumeClouds::Draw(Camera &camera) {
  glm::dvec3 cameraPosition = camera.getPosition();

  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
  glm::dmat4 viewProjInv = glm::inverse( viewProj );

  glm::dmat4 planetMatrixInv = glm::inverse( planetMatrix );

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


  glDisable( GL_DEPTH_TEST );

  glBindVertexArray( vao );

  program
    ->Use()
    ->BindVec3 ( "cameraPosition",            cameraPosition.x, cameraPosition.y, cameraPosition.z )
    // ->BindMat4( "viewProj", glm::value_ptr(glm::fmat4(viewProj)) )
    // ->BindMat4( "viewProjInv", glm::value_ptr(glm::fmat4(viewProjInv)) )
    ->BindVec3 ( "planetCenter",              planetCenter.x, planetCenter.y, planetCenter.z )
    ->BindFloat( "innerRadius",               innerRadius )
    ->BindMat4 ( "planetMatrixInv",           glm::value_ptr(glm::fmat4(planetMatrixInv)) )
    ->BindVec3 ( "lightDirection",            lightDirection.x, lightDirection.y, lightDirection.z )
    ->BindVec2 ( "cloudDisplacementMeters",   cloudDisplacementMeters.x, cloudDisplacementMeters.y )
    ->BindTexture2D( "globalAlphaSampler",    globalAlphaSampler, 0 )
    ->BindTexture2D( "coverageDetailSampler", coverageDetailSampler, 1 )
    ->BindTexture3D( "noiseVolumeSampler",    noiseVolumeSampler, 2 )
    ->BindFloat    ( "cloudCoverageFraction", 0.5 )

    ->BindVec3( "bottomLeftDir",  dir00.x, dir00.y, dir00.z )
    ->BindVec3( "bottomRightDir", dir10.x, dir10.y, dir10.z )
    ->BindVec3( "topLeftDir",     dir01.x, dir01.y, dir01.z )
    ->BindVec3( "topRightDir",    dir11.x, dir11.y, dir11.z )

    ->BindTexture2D( "transmittance_texture",         transmittance, 3 )
    ->BindTexture3D( "scattering_texture",            scattering,    4 )
    ->BindTexture3D( "single_mie_scattering_texture", scattering,    5 )
    ->BindTexture2D( "irradiance_texture",            irradiance,    6 )

// Atmosphere uniforms
    ->BindFloat("bottom_radius",        6369344.0                     )
    ->BindFloat("top_radius",           6537590.0                     )
    ->BindVec3 ("solar_irradiance",     1.474000, 1.850400, 1.911980 )
    ->BindFloat("sun_angular_radius",   0.004675                      )
    ->BindVec3 ("rayleigh_scattering",  0.005802, 0.013558, 0.033100 )
    ->BindVec3 ("mie_scattering",       0.003996, 0.003996, 0.003996 )
    ->BindFloat("mie_phase_function_g", 0.8                           )
    ->BindFloat("mu_s_min",             -0.207912                     )

    ->BindInt( "DEBUG", DEBUG )
    ;

  glDrawArrays( GL_TRIANGLES, 0, 6 );
}
