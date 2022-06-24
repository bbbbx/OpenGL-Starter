#include "CameraController.h"

CameraController::CameraController(Camera& camera) : mCamera(camera) { }

CameraController::CameraController(Camera* camera) : mCamera(*camera) { }

CameraController::~CameraController()
{
}

void CameraController::Move(glm::dvec3 direction, double amount) {
  const auto cameraPosition = mCamera.getPosition();
  mCamera.setPosition( cameraPosition + direction * amount );

  // adjustOrthographicFrustum(true)
}

void CameraController::MoveForward(double amount) {
  glm::dvec3 cameraDirection = mCamera.getDirection();
  Move( cameraDirection, amount );
}

void CameraController::MoveBackward(double amount) {
  glm::dvec3 cameraDirection = mCamera.getDirection();
  Move( cameraDirection, - amount );
}

void CameraController::MoveUp(double amount) {
  glm::dvec3 cameraUp = mCamera.getUp();
  Move( cameraUp, amount );
}

void CameraController::MoveDown(double amount) {
  glm::dvec3 cameraUp = mCamera.getUp();
  Move( cameraUp, - amount );
}

void CameraController::MoveLeft(double amount) {
  glm::dvec3 cameraRight = mCamera.getRight();
  Move( cameraRight, - amount );
}

void CameraController::MoveRight(double amount) {
  glm::dvec3 cameraRight = mCamera.getRight();
  Move( cameraRight, amount );
}

void CameraController::Look(glm::dvec3 axis, double angle) {
  glm::dquat quaterion = glm::angleAxis( - angle, axis );
  glm::dmat3 rotation = glm::toMat3( quaterion );

  auto direction = mCamera.getDirection();
  auto up = mCamera.getUp();
  auto right = mCamera.getRight();

  direction = rotation * direction;
  up = rotation * up;
  right = rotation * right;

  glm::dquat orientation = glm::toQuat( glm::dmat3( right, up, - direction ) );

  mCamera.setOrientation( orientation );
}

void CameraController::LookDown(double angle) {
  glm::dvec3 axis = mCamera.getRight();
  Look( axis, angle );
}

void CameraController::LookUp(double angle) {
  glm::dvec3 axis = mCamera.getRight();
  Look( axis, - angle );
}

void CameraController::LookLeft(double angle) {
  glm::dvec3 axis = mCamera.getUp();
  Look( axis, - angle );
}

void CameraController::LookRight(double angle) {
  glm::dvec3 axis = mCamera.getUp();
  Look( axis, angle );
}
