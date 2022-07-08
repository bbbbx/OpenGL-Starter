#include "Transforms.h"

glm::dmat4 Transforms::eastNorthUpToFixedFrame(glm::dvec3 origin/*, ellipsoid, result*/) {
  glm::dmat4 mat4;

  glm::dvec3 surfaceNormal = glm::normalize( origin );
  glm::dvec3 up = surfaceNormal;
  glm::dvec3 east( - origin.y, origin.x, 0.0 );
  east = glm::normalize( east );
  glm::dvec3 north = glm::cross( up, east );

  mat4[0][0] = east.x;   mat4[0][1] = east.y;   mat4[0][2] = east.z;   mat4[0][3] = 0.0;
  mat4[1][0] = north.x;  mat4[1][1] = north.y;  mat4[1][2] = north.z;  mat4[1][3] = 0.0;
  mat4[2][0] = up.x;     mat4[2][1] = up.y;     mat4[2][2] = up.z;     mat4[2][3] = 0.0;
  mat4[3][0] = origin.x; mat4[3][1] = origin.y; mat4[3][2] = origin.z; mat4[3][3] = 1.0;

  return mat4;
}
