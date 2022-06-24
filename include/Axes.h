#ifndef __AXES_H__
#define __AXES_H__

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Camera.h"
#include "Program.h"
#include "Renderable.h"

class Axes : public Renderable
{
private:
    GLuint vao;
    GLuint vbo;
    Program *program = nullptr;

public:
    glm::dmat4 modelMatrix;

    Axes() : Axes(1.0) {};
    Axes(double scale);
    ~Axes();

    inline void Update(Camera &camera, double time, double deltaTime) {}
    void Draw(Camera &camera);
};


#endif // __AXES_H__
