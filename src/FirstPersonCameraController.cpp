#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "FirstPersonCameraController.h"

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

  MoveRight( velocity.x * dt );
  MoveForward( velocity.z * dt);
  MoveUp( velocity.y * dt);
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

  LookRight( dx * 0.002 );
  LookDown( dy * 0.002 );

  previousCursorX = x;
  previousCursorY = y;
}
