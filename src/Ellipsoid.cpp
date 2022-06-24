#include "Ellipsoid.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <vector>

namespace
{
  static constexpr double PI = 3.141592653589793;
  static constexpr double PI_OVER_TWO = 1.5707963267948966;
  static constexpr double TWO_PI = 6.283185307179586;

  glm::dvec3 phiThetaToXYZ(double phi, double theta, double radius) {
    double x0 = std::sin( phi ) * std::cos( theta ) * radius;
    double y0 = std::sin( phi ) * std::sin( theta ) * radius;
    double z0 = std::cos( phi ) * radius;

    return glm::dvec3( x0, y0, z0 );
  }

  struct Vertex
  {
    glm::dvec3 position;
  };


std::vector<Vertex> createEllipsoidGeometry(glm::dvec3 radii, glm::dvec3 innerRadii, unsigned int stackPartitions, unsigned int slicePartitions) {
  std::vector<Vertex> vertices;

  slicePartitions = slicePartitions + 1;
  stackPartitions = stackPartitions + 1;

  double stackStep = PI / (double)stackPartitions;
  double sliceStep = TWO_PI / (double)slicePartitions;

  // FILE* file = fopen("a.obj", "w");

  // std::string indices("");
  // int faceIndex = 1;

  for (double phi = 0.0; phi < PI; phi += stackStep)
  {
    for (double theta = 0; theta < TWO_PI; theta += sliceStep)
    {
      double nextPhi = std::min(phi + stackStep, PI);
      double nextTheta = std::min(theta + sliceStep, TWO_PI);

      Vertex vertex0 = { phiThetaToXYZ( phi, theta, radii.x ) };
      Vertex vertex1 = { phiThetaToXYZ( phi, nextTheta, radii.x ) };
      Vertex vertex2 = { phiThetaToXYZ( nextPhi, nextTheta, radii.x ) };
      Vertex vertex3 = { phiThetaToXYZ( nextPhi, theta, radii.x ) };

      vertices.push_back( vertex0 );
      vertices.push_back( vertex1 );
      vertices.push_back( vertex2 );

      vertices.push_back( vertex0 );
      vertices.push_back( vertex2 );
      vertices.push_back( vertex3 );

      // std::string s("");
      // s += "v " + std::to_string(vertex0.position.x) + " " + std::to_string(vertex0.position.y) + " " + std::to_string(vertex0.position.z) + "\n";
      // s += "v " + std::to_string(vertex1.position.x) + " " + std::to_string(vertex1.position.y) + " " + std::to_string(vertex1.position.z) + "\n";
      // s += "v " + std::to_string(vertex2.position.x) + " " + std::to_string(vertex2.position.y) + " " + std::to_string(vertex2.position.z) + "\n";
      // s += "v " + std::to_string(vertex3.position.x) + " " + std::to_string(vertex3.position.y) + " " + std::to_string(vertex3.position.z) + "\n";

      // const char* buf = s.c_str();
      // size_t size = s.size();
      // fwrite(buf, size, 1, file);

      // indices += "f " + std::to_string(faceIndex) + " " + std::to_string(faceIndex + 1) + " " + std::to_string(faceIndex + 2) + "\n";
      // indices += "f " + std::to_string(faceIndex) + " " + std::to_string(faceIndex + 2) + " " + std::to_string(faceIndex + 3) + "\n";

      // faceIndex += 4;
    }
  }

  // fwrite(indices.c_str(), indices.size(), 1, file);

  // fclose(file);

  return vertices;

// 
  double minimumClock = 0.0;
  double maximumClock = TWO_PI;
  double minimumCone = 0.0;
  double maximumCone = PI;

  // Add an extra slice and stack so that the number of partitions is the
  // number of surfaces rather than the number of joints
  slicePartitions = slicePartitions + 1;
  stackPartitions = stackPartitions + 1;

  if (slicePartitions < 2) {
    slicePartitions = 2;
  }
  if (stackPartitions < 2) {
    stackPartitions = 2;
  }

  // Create arrays for theta and phi. Duplicate first and last angle to
  // allow different normals at the intersections.
  std::vector<double> phis{ minimumCone };
  std::vector<double> thetas{ minimumClock };
  for (size_t i = 0; i < stackPartitions; i++)
  {
    phis.push_back( minimumCone + (i * (maximumCone - minimumCone)) / (stackPartitions - 1) );
  }
  phis.push_back( maximumCone );

  for (size_t j = 0; j < slicePartitions; j++)
  {
    thetas.push_back( minimumClock + (j * (maximumClock - minimumClock)) / (slicePartitions - 1) );
  }
  thetas.push_back( maximumClock );

  size_t numPhis = phis.size();
  size_t numThetas = thetas.size();

  // Allow for extra indices if there is an inner surface and if we need
  // to close the sides if the clock range is not a full circle
  unsigned int extraIndices = 0u;
  double vertexMultiplier = 1.0;
  bool hasInnerSurface =
    radii.x != innerRadii.x ||
    radii.y != innerRadii.y ||
    radii.z != innerRadii.z;
  bool isTopOpen = false;
  bool isBotOpen = false;
  bool isClockOpen = false;

  if ( hasInnerSurface ) {
    vertexMultiplier = 2.0;
    if (minimumCone > 0.0) {
      isTopOpen = true;
      extraIndices += slicePartitions - 1;
    }
    if (maximumCone < PI) {
      isBotOpen = true;
      extraIndices += slicePartitions - 1;
    }
    if ( (int)(maximumClock - minimumClock) % (int)TWO_PI ) {
    // if ((maximumClock - minimumClock) % (TWO_PI)) {
      isClockOpen = true;
      extraIndices += (stackPartitions - 1) * 2 + 1;
    } else {
      extraIndices += 1;
    }
  }

  size_t vertexCount = numThetas * numPhis * vertexMultiplier;
  std::vector<double> positions(vertexCount * 3);
  std::vector<bool> isInner(vertexCount, false);
  std::vector<bool> negateNormal(vertexCount, false);

  // TODO:
  // Multiply by 6 because there are two triangles per sector

}

} // namespace anonymous


std::vector<float> Ellipsoid::CreateGeometry(glm::dvec3 radii, glm::dvec3 innerRadii, unsigned int stackPartitions, unsigned int slicePartitions) {
  std::vector<Vertex> vertices = createEllipsoidGeometry( radii, innerRadii, stackPartitions, slicePartitions );
  size_t numVertex = vertices.size();

  std::vector<float> flattedVertices( numVertex * 3 );
  for (size_t i = 0; i < numVertex; i++) {
    auto position = vertices[i].position;

    flattedVertices[i*3 + 0] = position.x;
    flattedVertices[i*3 + 1] = position.y;
    flattedVertices[i*3 + 2] = position.z;
  }

  return flattedVertices;
}

Ellipsoid::Ellipsoid(glm::dvec3 radii, glm::dvec3 innerRadii, unsigned int stackPartitions, unsigned int slicePartitions) {
  std::vector<Vertex> attributes = createEllipsoidGeometry(radii, innerRadii, stackPartitions, slicePartitions);
  numVertex = attributes.size();

  std::vector<float> vertexAttributes( numVertex * 3 );
  for (size_t i = 0; i < numVertex; i++) {
    auto position = attributes[i].position;

    vertexAttributes[i*3 + 0] = position.x;
    vertexAttributes[i*3 + 1] = position.y;
    vertexAttributes[i*3 + 2] = position.z;
  }

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, numVertex * 3 * sizeof( float ), vertexAttributes.data(), GL_STATIC_DRAW );


  program = new Program( "./shaders/common/positionOnly.vert", "./shaders/common/solidColor.frag" );
  GLuint glProgram = program->GetGLProgram();
  GLint posLoc = glGetAttribLocation( glProgram, "in_position" );
  // GLint colorLoc = glGetAttribLocation( glProgram, "in_color" );

  glEnableVertexAttribArray(posLoc);
  GLint64 offset = 0;
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)offset);

  // glEnableVertexAttribArray(colorLoc);
  // offset = sizeof(float) * 3;
  // glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertexAttributes[0]), (GLvoid*)offset);

  glBindVertexArray( 0 );

  // modelMatrix = glm::identity<glm::dmat4>();
}

void Ellipsoid::Update(Camera &camera, double time, double deltaTime) {
  
}

void Ellipsoid::Draw(Camera &camera) {
  glm::dmat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();

  program
    ->Use()
    ->BindMat4( "modelMatrix", glm::value_ptr(glm::fmat4(modelMatrix)) )
    ->BindMat4( "viewProj",  glm::value_ptr(glm::fmat4(viewProj)) )
    ->BindFloat( "farClipDistance", (float)camera.getFar() )
    ->BindVec4( "color", 0.0, 0.5, 0.0, 1.0 )
  ;

  glBindVertexArray(vao);
  glEnable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glDrawArrays(GL_TRIANGLES, 0, numVertex);
}

Ellipsoid::~Ellipsoid()
{
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  delete program;
}
