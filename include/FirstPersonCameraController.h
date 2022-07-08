#ifndef __FIRST_PERSON_CAMERA_CONTROLLER_H__
#define __FIRST_PERSON_CAMERA_CONTROLLER_H__

#include "CameraController.h"

class FirstPersonCameraController final : public CameraController
{
private:
  int previousCursorX = 0, previousCursorY = 0;

  glm::dvec3 velocity;
  glm::dvec3 acceleration;
  glm::dvec3 moveDirection;
  bool movingForward = false;
  bool movingBackward = false;
  bool movingLeft = false;
  bool movingRight = false;
  bool movingDown = false;
  bool movingUp = false;
  bool movingFastly = false;

public:
  double speedUpMultiplier = 10.0;

  FirstPersonCameraController(Camera& camera);
  FirstPersonCameraController(Camera* camera);
  ~FirstPersonCameraController();

  bool inputEnabled = true;
  // inline void Disable() { inputEnabled = false; };
  // inline void Enable() { inputEnabled = true; };

  void Update(double deltaTime) override;

  void HandleKeyDown(int key, int modifier) override;
  void HandleKeyUp(int key, int modifier) override;
  void HandleCursorMove(double x, double y) override;
  void HandleCursorDown(int button) override {};
};

#endif // __FIRST_PERSON_CAMERA_CONTROLLER_H__
