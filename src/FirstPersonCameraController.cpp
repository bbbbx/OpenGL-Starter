#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "FirstPersonCameraController.h"
#include "Transforms.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <string>

namespace
{
  
} // namespace anonymous


FirstPersonCameraController::FirstPersonCameraController(Camera& camera)
 : CameraController(camera)
{
}

FirstPersonCameraController::FirstPersonCameraController(Camera* camera)
 : CameraController(camera)
{
}

FirstPersonCameraController::~FirstPersonCameraController() { }

void FirstPersonCameraController::Update(double deltaTime) {
  if ( ! inputEnabled ) return;

  double& dt = deltaTime;

  // ---  Change Camera's Position ---
  // Apply drag
  velocity -= velocity * 10.0 * dt;

  // Moving direction
  moveDirection.x = (double)movingRight   - (double)movingLeft;
  moveDirection.y = (double)movingUp      - (double)movingDown;
  moveDirection.z = (double)movingForward - (double)movingBackward;

  double acceleration = 10000.0 *
    (movingFastly ? speedUpMultiplier : 1.0);

  // Moving forward or backward
  if ( movingForward || movingBackward ) {
    velocity.z += moveDirection.z * acceleration * dt;
  }
  // Moving left or right
  if ( movingLeft || movingRight ) {
    velocity.x += moveDirection.x * acceleration * dt;
  }
  // Moving down or up
  if ( movingDown || movingUp ) {
    velocity.y += moveDirection.y * acceleration * dt;
  }

  glm::dvec3 oldEye = mCamera.getPosition();

  // TODO: Surface normal should calculate from WGS84 ellipsoid, here just a workaround
  glm::dvec3 surfaceNormal = glm::normalize( oldEye );;
  Move( surfaceNormal, velocity.y * dt );
  // MoveUp( velocity.y * dt );

  glm::dvec3 forward = glm::cross( surfaceNormal, mCamera.getRight() );
  Move( forward, velocity.z * dt );
  // MoveForward( velocity.z * dt );

  glm::dvec3 right = glm::cross( forward, surfaceNormal );
  Move( right, velocity.x * dt );
  // MoveRight( velocity.x * dt );

  glm::dvec3 newEye = mCamera.getPosition();
  if ( glm::length( newEye ) < 6369344.0 ) {
    mCamera.setPosition( oldEye );
  }
}

void FirstPersonCameraController::HandleKeyDown(int key, int modifier) {
  switch ( key )
  {
  case GLFW_KEY_W:
    movingForward = true;
    break;
  case GLFW_KEY_S:
    movingBackward = true;
    break;
  case GLFW_KEY_A:
    movingLeft = true;
    break;
  case GLFW_KEY_D:
    movingRight = true;
    break;
  case GLFW_KEY_C:
    movingDown = true;
    break;
  case GLFW_KEY_E:
    movingUp = true;
    break;
  case GLFW_KEY_LEFT_SHIFT:
    movingFastly = true;
    break;
  default:
    break;
  }
}

void FirstPersonCameraController::HandleKeyUp(int key, int modifier) {
  switch ( key )
  {
  case GLFW_KEY_W:
    movingForward = false;
    break;
  case GLFW_KEY_S:
    movingBackward = false;
    break;
  case GLFW_KEY_A:
    movingLeft = false;
    break;
  case GLFW_KEY_D:
    movingRight = false;
    break;
  case GLFW_KEY_C:
    movingDown = false;
    break;
  case GLFW_KEY_E:
    movingUp = false;
    break;
  case GLFW_KEY_LEFT_SHIFT:
    movingFastly = false;
    break;
  default:
    break;
  }
}

void FirstPersonCameraController::HandleCursorMove(double x, double y) {
  double dx = x - previousCursorX;
  double dy = y - previousCursorY;

  previousCursorX = x;
  previousCursorY = y;
  if ( ! inputEnabled ) return;


  glm::dvec3  eye = mCamera.getPosition();
  glm::dvec3 direction = mCamera.getDirection();
  // glm::dvec3 up = mCamera.getUp();
  glm::dvec3 right = mCamera.getRight();
  glm::dmat4 cameraMatrix = glm::inverse( mCamera.getViewMatrix() );

  glm::dmat4 toFixedFrame = Transforms::eastNorthUpToFixedFrame( eye );
  glm::dmat3 cameraENU = glm::mat3( glm::inverse( toFixedFrame ) * cameraMatrix );

  // glm::dvec3 east = glm::dvec3( glm::column( toFixedFrame, 0 ) );
  // glm::dvec3 north = glm::dvec3( glm::column( toFixedFrame, 1 ) );
  glm::dvec3 up = glm::dvec3( glm::column( toFixedFrame, 2 ) );

  glm::dquat rotateRight = glm::angleAxis( - dx * 0.002, glm::dvec3( 0.0, 0.0, 1.0 ) );
  glm::dquat rotateUp = glm::angleAxis( - dy * 0.002, glm::dvec3( 1.0, 0.0, 0.0 ) );
  glm::dmat3 rotation = glm::toMat3( rotateRight * rotateUp );
  // glm::dmat3 rotationENU = rotation * glm::dmat3( toFixedFrame );

  // cameraENU, rotation rotationENU
  glm::dmat3 rotated = rotation * cameraENU;
  glm::dquat orientation = glm::toQuat( rotated );

  // glm::dmat3 O = glm::mat3( toFixedFrame );
  // glm::dmat3 rotated = rotation * O;

  // direction = rotation * direction;
  // up = rotation * up;
  // right = rotation * right;


  // glm::dquat orientation = glm::toQuat( cameraInLocal * rotation );
  // mCamera.setOrientation( orientation );

  Look( up, dx * 0.002 );
  // Look( east, dy * 0.002 );
  // LookRight( dx * 0.002 );
  LookDown( dy * 0.002 );

}
