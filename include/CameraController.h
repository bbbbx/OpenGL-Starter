#ifndef __CAMERA_CONTROLLER_H__
#define __CAMERA_CONTROLLER_H__

#include <glm/glm.hpp>
#include "Camera.h"

class CameraController
{
protected:
  Camera& mCamera;
public:
  CameraController(Camera& camera);
  CameraController(Camera* camera);
  ~CameraController();

  void Move(glm::dvec3 direction, double amount);
  void MoveForward(double amout);
  void MoveBackward(double amout);
  void MoveLeft(double amout);
  void MoveRight(double amout);
  void MoveUp(double amout);
  void MoveDown(double amout);

  void Look(glm::dvec3 axis, double angle);

  void LookDown(double angle);
  void LookUp(double angle);
  void LookLeft(double angle);
  void LookRight(double angle);

  virtual void Update(double deltaTime) = 0;

  virtual void HandleKeyDown(int key, int modifier) = 0;
  virtual void HandleKeyUp(int key, int modifier) = 0;
  virtual void HandleCursorMove(double x, double y) = 0;
  virtual void HandleCursorDown(int button) = 0;
};

#endif // __CAMERA_CONTROLLER_H__
