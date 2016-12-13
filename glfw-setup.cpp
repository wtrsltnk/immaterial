
#include <GL/glextl.h>

#include "glfw-setup.h"
#include <GLFW/glfw3.h>

GlfwProgram* GlfwProgram::program = nullptr;

GlfwProgram::GlfwProgram()
    : width(640), height(480), title("GlfwWindow"), window(nullptr), _input(nullptr)
{
    GlfwProgram::program = this;
}

GlfwProgram::~GlfwProgram()
{ }

int GlfwProgram::Run(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        this->args.push_back(argv[i]);
    }

    this->_input = new Input();

    if (glfwInit() == GLFW_FALSE)
        return -1;

    this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
    if (this->window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(this->window, GlfwProgram::KeyActionCallback);
    glfwSetMouseButtonCallback(this->window, GlfwProgram::MouseActionCallback);
    glfwSetCursorPosCallback(this->window, GlfwProgram::MouseMoveCallback);
    glfwSetFramebufferSizeCallback(this->window, GlfwProgram::ResizeCallback);

    glfwMakeContextCurrent(this->window);

    glExtLoadAll((PFNGLGETPROC*)glfwGetProcAddress);

    if (this->SetUp())
    {
        GlfwProgram::ResizeCallback(this->window, this->width, this->height);

        while (glfwWindowShouldClose(this->window) == 0 && this->keepRunning)
        {
            glfwPollEvents();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            this->Render();

            glfwSwapBuffers(this->window);

        }
        this->CleanUp();
    }

    glfwTerminate();

    delete this->_input;
    this->_input = nullptr;

    return 0;
}

IInput* GlfwProgram::input()
{
    return this->_input;
}

float GlfwProgram::elapsed() const
{
    return glfwGetTime();
}

void GlfwProgram::KeyActionCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
    if(GlfwProgram::program)
    {
        //GlfwProgram::program->_input;
    }
}

void GlfwProgram::MouseActionCallback(GLFWwindow*, int button, int action, int mods)
{
    if(GlfwProgram::program)
    {
        //GlfwProgram::program->_input;
    }
}

void GlfwProgram::MouseMoveCallback(GLFWwindow*, double x, double y)
{
    if(GlfwProgram::program)
    {
        //GlfwProgram::program->_input;
    }
}

void GlfwProgram::ResizeCallback(GLFWwindow*, int width, int height)
{
    if(GlfwProgram::program)
    {
        //GlfwProgram::program->_input;
    }
}
