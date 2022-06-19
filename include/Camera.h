#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
private:
  glm::dvec3 mPosition;
  glm::dquat mOrientation;
  double mAspectRatio;
  double mFovY;
  double mNear;
  double mFar;

  glm::dmat4 mViewMatrix;
  glm::dmat4 mProjectionMatrix;

  void updateViewMatrix();
  void updateProjectionMatrix();
public:
  Camera(double aspectRatio);
  ~Camera();

  void setNear(double near);
  void setFar(double far);
  void setAspectRatio(double aspectRatio);
  void setFovY(double fovY);

  void setPosition(const glm::dvec3 &position);
  void setOrientation(const glm::dquat &orientation);

  double getNear() const { return mNear; }
  double getFar() const { return mFar; }
  double getAspectRatio() const { return mAspectRatio; }
  double getFovY() const { return mFovY; }

  glm::dvec3 getPosition() const { return mPosition; }
  glm::dquat getOrientation() const { return mOrientation; }

  glm::dmat4 getViewMatrix() const { return mViewMatrix; }
  glm::dmat4 getProjectionMatrix() const { return mProjectionMatrix; }
};

#endif // __CAMERA_H__
