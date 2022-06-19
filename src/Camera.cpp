#include "Camera.h"

Camera::Camera(double aspectRatio) :
  mAspectRatio(aspectRatio),
  mFovY(glm::radians(50.0f)),
  mNear(0.1),
  mFar(1.0e9) {
  setPosition(glm::dvec3(1.0, 0.0, 0.0));
  setOrientation(glm::identity<glm::dquat>());
  updateProjectionMatrix();
}

void Camera::setPosition(const glm::dvec3 &position) {
  mPosition = position;
  updateViewMatrix();
}
void Camera::setOrientation(const glm::dquat &orientation) {
  mOrientation = orientation;
  updateViewMatrix();
}

void Camera::updateViewMatrix() {
  glm::dmat4 rotation(1.0);
  glm::dmat4 translation(1.0);

  translation = glm::translate(translation, mPosition);
  rotation = glm::toMat4(mOrientation);

  mViewMatrix = glm::inverse(translation * rotation);
}

void Camera::updateProjectionMatrix() {
  mProjectionMatrix = glm::perspective(mFovY, mAspectRatio, mNear, mFar);
}

Camera::~Camera()
{
}
