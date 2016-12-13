#ifndef _GLFW_SETUP_HPP_
#define _GLFW_SETUP_HPP_

#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include "input.h"

class GlfwProgram
{
public:
    GlfwProgram();
    virtual ~GlfwProgram();

    int Run(int argc, char* argv[]);

protected:
    virtual bool SetUp() = 0;
    virtual void Render() = 0;
    virtual void CleanUp() = 0;
    virtual void OnResize(int width, int height) = 0;

    IInput* input();
    float elapsed() const;

    int width, height;
    std::string title;
    std::vector<std::string> args;
    bool keepRunning;

private:
    GLFWwindow* window;
    Input* _input;
    static GlfwProgram* program;

    static void KeyActionCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void MouseActionCallback(GLFWwindow*, int button, int action, int mods);
    static void MouseMoveCallback(GLFWwindow*, double x, double y);
    static void ResizeCallback(GLFWwindow*, int width, int height);

};

#endif // _GLFW_SETUP_HPP_
