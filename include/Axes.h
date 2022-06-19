#ifndef __AXES_H__
#define __AXES_H__

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Camera.h"

class Axes
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint program;

    glm::dmat4 modelMatrix;

    GLuint modelMatrixLoc;
    GLuint viewProjLoc;
    GLuint farClipDistanceLoc;
public:
    Axes() : Axes(1.0) {};
    Axes(double scale);
    ~Axes();

    void Draw(Camera &camera);
};


#endif // __AXES_H__
