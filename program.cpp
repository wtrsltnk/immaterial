
#define GLEXTL_IMPLEMENTATION
#include <GL/glextl.h>

#include "sdl2-setup.h"
#include "common/font.h"

#include <map>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Program : public SDLProgram
{
public:
    virtual ~Program();

    virtual bool SetUp();
    virtual void Render();
    virtual void CleanUp();
    virtual void OnResize(int width, int height);

    glm::mat4 _proj;
};

Program::~Program() { }

bool Program::SetUp()
{
    std::cout << "GL_VERSION                  : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GL_RENDERER                 : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL_VENDOR                   : " << glGetString(GL_VENDOR) << std::endl;
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    return true;
}

void Program::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Program::CleanUp() { }

void Program::OnResize(int w, int h)
{
    if (h < 1) h = 1;

    glViewport(0, 0, w, h);

    this->_proj = glm::ortho(0.0f, float(w), float(h), 0.0f);
}

int main(int argc, char* argv[])
{
    return Program().Run(argc, argv);
}
