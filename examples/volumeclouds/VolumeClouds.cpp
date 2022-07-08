#include "VolumeClouds.h"
#include "GLUtils.h"

#include <glm/gtc/type_ptr.hpp>

// #ifndef STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #endif
#include "stb_image.h"

#include <stdexcept>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace
{

void createCloudResources(int width, int height, GLuint *colorTexture, GLuint *depthTexture, GLuint *cloudFb) {
  *colorTexture = GLUtils::NewTexture2D( width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
  *depthTexture = GLUtils::NewTexture2D( width, height, GL_R32F, GL_RED, GL_FLOAT, nullptr, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

  std::vector<GLuint> textures = std::vector<GLuint>( 2 );
  textures[ 0 ] = *colorTexture;
  textures[ 1 ] = *depthTexture;

  *cloudFb = GLUtils::NewFramebuffer( textures );

  // std::cout << *colorTexture << ", " << *depthTexture << ", " << *cloudFb << std::endl;
}

std::vector<float> createQuadWithUvs() {
  std::vector<float> quad;

  // vertex 0
  quad.push_back( 0.0f ); quad.push_back( 0.0f ); quad.push_back( 0.0f );
  quad.push_back( 0.0f ); quad.push_back( 0.0f );

  // vertex 1
  quad.push_back( 1.0f ); quad.push_back( 0.0f ); quad.push_back( 0.0f );
  quad.push_back( 1.0f ); quad.push_back( 0.0f );

  // vertex 2
  quad.push_back( 0.0f ); quad.push_back( 1.0f ); quad.push_back( 0.0f );
  quad.push_back( 0.0f ); quad.push_back( 1.0f );

  // vertex 3
  quad.push_back( 1.0f ); quad.push_back( 1.0f ); quad.push_back( 0.0f );
  quad.push_back( 1.0f ); quad.push_back( 1.0f );

  return quad;
}

} // namespace anonymous


VolumeClouds::VolumeClouds(/* args */)
{
  // glGenVertexArrays(1, &vao);
  // glGenBuffers( 1, &vbo );

  program = new Program("./shaders/volumeclouds/volumeclouds.vert", "./shaders/volumeclouds/volumeclouds.frag");

// textures
  // stbi_set_flip_vertically_on_load( 1 );
  int x = 8192, y = 4096, n = 4;
  stbi_uc* globalAlphaSamplerData = stbi_load("./data/cloud_combined_8192.png", &x, &y, &n, n);
  if (globalAlphaSamplerData == nullptr) {
    throw std::runtime_error("Failed to load global alpha sampler file");
  }

  // Read pixels byte by byte, avoid alignment issue
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  globalAlphaSampler = GLUtils::NewTexture2D( x, y,
    GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, globalAlphaSamplerData,
    GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE );

  stbi_image_free(globalAlphaSamplerData);


  x = 512;
  y = 512;
  n = 4;
  stbi_uc* coverageDetailSamplerData = stbi_load("./data/Noise.png", &x, &y, &n, n);
  if (coverageDetailSamplerData == nullptr) {
    throw std::runtime_error("Failed to load global alpha sampler file");
  }
  coverageDetailSampler = GLUtils::NewTexture2D( x, y,
    GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, coverageDetailSamplerData,
    GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT );

  stbi_image_free(coverageDetailSamplerData);

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


// 
  compositeProgram = new Program( "./shaders/volumeclouds/ScreenQuad.vert", "./shaders/volumeclouds/CompositeClouds.frag" );

  glGenVertexArrays( 1, &quadVao );
  if ( ! quadVao ) {
    std::cerr << "Falied to create quadVao" << std::endl;
  }
  glBindVertexArray( quadVao );

  glGenBuffers( 1, &quadVbo );
  if ( ! quadVbo ) {
    std::cerr << "Falied to create quadVbo" << std::endl;
  }
  glBindBuffer( GL_ARRAY_BUFFER, quadVbo );

  std::vector<float> quadVaoData = createQuadWithUvs();
  glBufferData( GL_ARRAY_BUFFER, quadVaoData.size() * sizeof( float ), quadVaoData.data(), GL_STATIC_DRAW );

  GLuint compositeGLProgram = compositeProgram->GetGLProgram();

  GLint inPositionLoc = glGetAttribLocation( compositeGLProgram, "in_position" );
  GLsizei stride = sizeof(float) * 5;
  if ( inPositionLoc != -1 ) {
    glEnableVertexAttribArray( inPositionLoc );
    GLint64 offset = 0;
    glVertexAttribPointer( inPositionLoc, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset );
  }

  GLint inTexCoordLoc = glGetAttribLocation( compositeGLProgram, "in_texCoord" );
  if ( inTexCoordLoc != -1 ) {
    glEnableVertexAttribArray( inTexCoordLoc );
    GLint64 offset = sizeof( float ) * 3;
    glVertexAttribPointer( inTexCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset );
  }

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );
}

VolumeClouds::~VolumeClouds() {
  glDeleteBuffers( 1, &quadVbo );
  glDeleteVertexArrays( 1, &quadVao );
}

void VolumeClouds::Update(Camera &camera, double time, double deltaTime) {
  double julianDateSeconds = time;// mJulianDate * 24.0 * 60.0 * 60.0;
  // Move clouds
  {
    // float windSpeed = 10.0;
    cloudDisplacementMeters.y = -20000.0 + std::fmod( julianDateSeconds, 40000 ) * windSpeed;
    cloudDisplacementMeters.x = cloudDisplacementMeters.y;
  }

  glGetIntegerv( GL_VIEWPORT, viewport.data() );
  float scale = 0.25;
  // std::cout << viewport[2] << ", " << viewport[3] << std::endl;
  GLint cloudWidth = viewport[ 2 ] * scale;
  GLint cloudHeight = viewport[ 3 ] * scale;
  if ( cloudWidth != cloudTexturesDimensions.x ||
       cloudHeight != cloudTexturesDimensions.y ||
       ! colorTexture ||
       ! depthTexture ||
       ! cloudFb
  ) {
    glDeleteTextures( 1, &colorTexture );
    glDeleteTextures( 1, &depthTexture );
    glDeleteFramebuffers( 1, &cloudFb );

    cloudTexturesDimensions.x = cloudWidth;
    cloudTexturesDimensions.y = cloudHeight;
    createCloudResources( cloudTexturesDimensions.x, cloudTexturesDimensions.y, &colorTexture, &depthTexture, &cloudFb );
  }

}

void VolumeClouds::PreComputeClouds(Camera &camera) {
  // std::cout << (double)cloudTexturesDimensions.x / (double)cloudTexturesDimensions.y << std::endl;
  double oldAspectRatio = camera.getAspectRatio();
  // camera.setAspectRatio( (double)cloudTexturesDimensions.x / (double)cloudTexturesDimensions.y );
  glm::dvec3 cameraPosition = camera.getPosition();

  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
  glm::dmat4 viewProjInv = glm::inverse( viewProj );

  glm::dmat4 planetMatrixInv = glm::inverse( planetMatrix );

  // transform NDC of 4 corners to world space
  // FIXME:
  glm::dvec4 c00 = viewProjInv * glm::dvec4(-1.0, -1.0, 0.5, 1.0);
  c00 /= c00.w;
  glm::dvec4 c10 = viewProjInv * glm::dvec4( 1.0, -1.0, 0.5, 1.0);
  c10 /= c10.w;
  glm::dvec4 c01 = viewProjInv * glm::dvec4(-1.0,  1.0, 0.5, 1.0);
  c01 /= c01.w;
  glm::dvec4 c11 = viewProjInv * glm::dvec4( 1.0,  1.0, 0.5, 1.0);
  c11 /= c11.w;

// FIXME: nmsl
  // glm::dvec4 cameraCenterDirection4 = viewProjInv * glm::dvec4( 0.0, 0.0, 1.0, 1.0 );
  // cameraCenterDirection4 /= cameraCenterDirection4.w;
  // glm::dvec3 cameraCenterDirection = glm::dvec3( cameraCenterDirection4 );
  // cameraCenterDirection = glm::normalize( cameraCenterDirection );
// -0.35967, 0.847329, 0.390731

  glm::dvec3 cameraCenterDirection = camera.getDirection();

  // std::cout << cameraCenterDirection.x << ", " << cameraCenterDirection.y << ", " << cameraCenterDirection.z << std::endl;

  glm::dvec3 dir00 = glm::dvec3(c00) - cameraPosition;
  dir00 = glm::normalize(dir00);
  glm::dvec3 dir10 = glm::dvec3(c10) - cameraPosition;
  dir10 = glm::normalize(dir10);
  glm::dvec3 dir01 = glm::dvec3(c01) - cameraPosition;
  dir01 = glm::normalize(dir01);
  glm::dvec3 dir11 = glm::dvec3(c11) - cameraPosition;
  dir11 = glm::normalize(dir11);

// Render clouds
// 生成云时关闭 cull 和 depth test
// 不进行 clear
// viewport 设为输出纹理的宽高
  GLint oldDrawFramebuffer = 0;
  glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFramebuffer );

glBindFramebuffer( GL_DRAW_FRAMEBUFFER, cloudFb );

  glDisable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glDisable( GL_BLEND );

// std::cout << cloudTexturesDimensions.x << ", " << cloudTexturesDimensions.y << std::endl;
  glViewport( 0, 0, cloudTexturesDimensions.x, cloudTexturesDimensions.y );
  glBindVertexArray( quadVao );

  program
    ->Use()
    // FIXME: 对数深度一定要设置 farClipDistance！！！！
    ->BindFloat( "farClipDistance", (float)camera.getFar() )
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

    ->BindVec3( "cameraCenterDirection", cameraCenterDirection.x, cameraCenterDirection.y, cameraCenterDirection.z )

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

// Misc
    ->BindInt( "DEBUG_VOLUME", (int)(uniforms.DEBUG_VOLUME) )
    ->BindInt( "ENABLE_HIGH_DETAIL_CLOUDS", (int)(uniforms.ENABLE_HIGH_DETAIL_CLOUDS) )
    ->BindVec3( "ambientLightColor", 0.0, 0.0, 0.0 )
    ->BindVec2( "cloudTopZeroDensityHeight", uniforms.cloudTopZeroDensityHeight[0], uniforms.cloudTopZeroDensityHeight[1] )
    ->BindVec2( "cloudBottomZeroDensity", uniforms.cloudBottomZeroDensity[0], uniforms.cloudBottomZeroDensity[1] )
    ->BindVec2( "cloudOcclusionStrength", uniforms.cloudOcclusionStrength[0], uniforms.cloudOcclusionStrength[1] )
    ->BindVec2( "cloudDensityMultiplier", uniforms.cloudDensityMultiplier[0], uniforms.cloudDensityMultiplier[1] )
    ->BindVec3( "noiseFrequencyScale", uniforms.noiseFrequencyScale, uniforms.noiseFrequencyScale, uniforms.noiseFrequencyScale )
    ->BindFloat( "powderStrength", uniforms.powderStrength )
    ->BindFloat( "scatterSampleDistanceScale", uniforms.scatterSampleDistanceScale )
    ->BindFloat( "scatterDistanceMultiplier", uniforms.scatterDistanceMultiplier )
    ->BindFloat( "cloudChaos", uniforms.cloudChaos )
    ;

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

  // Restore state
  camera.setAspectRatio( oldAspectRatio );

  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, oldDrawFramebuffer );
  glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
}

void VolumeClouds::Draw(Camera &camera) {
// Composite
  // camera.setAspectRatio( (double)viewport[2] / (double)viewport[3] );

  // glBindFramebuffer( GL_DRAW_FRAMEBUFFER, oldDrawFramebuffer );
// glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
  glEnable( GL_DEPTH_TEST );
  // glDisable( GL_DEPTH_TEST );
// Why? 半透明物体
  glDepthMask( GL_FALSE );

  // Premultiplied mode
  glEnable( GL_BLEND );
  glBlendEquation( GL_FUNC_ADD );
  glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

  glBindVertexArray( quadVao );

  compositeProgram
    ->Use()
    ->BindTexture2D( "colorTexture", colorTexture, 0 )
    ->BindTexture2D( "depthTexture", depthTexture, 1 )
    ;

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

// Restore
  glDepthMask( GL_TRUE );
  glDisable( GL_BLEND );
  // glEnable( GL_DEPTH_TEST );
}
