#include "program.h"
#include "common/font.h"

#include <GL/glextl.h>
#include <SDL.h>
#include <map>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

Application* gApp = new AssetViewer();

Font f;

AssetViewer::AssetViewer()
{ }

AssetViewer::~AssetViewer()
{ }

bool AssetViewer::InitializeApplication(System* sys)
{
    this->_sys = sys;

    return true;
}

bool AssetViewer::InitializeGraphics()
{
    std::cout << "GL_VERSION                  : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GL_RENDERER                 : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL_VENDOR                   : " << glGetString(GL_VENDOR) << std::endl;
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

//    f.InitializeFont("..\\immaterial\\MaterialIcons-Regular.ttf", 28.0f, 0xe000);
    f.InitializeFont("..\\immaterial\\fontawesome-webfont.ttf", 28.0f, 0xf000);

    return true;
}

void AssetViewer::GameLoop()
{
    static double lastTime = this->_sys->GetTime();
    static double lastUpdateTime = this->_sys->GetTime();

    double time = this->_sys->GetTime();
    double diff = time - lastTime;

    lastTime = time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float x = 10.0f, y = 30.0f;
    for (int i = 0; i < 1024; i++)
    {
        f.DrawIcon(this->_proj, glm::mat4(1.0f), x, y, f.IconOffset() + i);
        x += f.Size();
        if (x > 1260) { x = 10.0f; y += f.Size(); }
    }
//    f.DrawIcon(this->_proj, glm::mat4(1.0f), 50.0f, 20.0f, FA_BATTERY_THREE_QUARTERS);
}

bool AssetViewer::IsRunning()
{
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    return !state[SDL_SCANCODE_ESCAPE];
}

void AssetViewer:: Resize(int w, int h)
{
    if (h < 1) h = 1;

    glViewport(0, 0, w, h);

    this->_proj = glm::ortho(0.0f, float(w), float(h), 0.0f);
}

void AssetViewer::MouseButtonDown(int button, int x, int y)
{ }

void AssetViewer::MouseMove(int x, int y)
{ }

void AssetViewer::MouseButtonUp(int button, int x, int y)
{ }

void AssetViewer::MouseWheel(int x, int y)
{ }

void AssetViewer::KeyAction(int key, int action)
{
//    if (key == SDLK_SPACE && action) Setting("Viewer.PauseAnimation") = !Setting("Viewer.PauseAnimation").AsBool();
//    else if (key == SDLK_KP_PLUS && action) Setting("Viewer.Camera.Speed") = Setting("Viewer.Camera.Speed").AsFloat() + 5.0f;
//    else if (key == SDLK_KP_MINUS && action) Setting("Viewer.Camera.Speed") = Setting("Viewer.Camera.Speed").AsFloat() - 5.0f;
}

void AssetViewer::Destroy()
{ }

GLuint LoadShaderProgram(const std::string& vertShaderStr, const std::string& fragShaderStr, const std::map<GLuint, std::string>& attrLoc)
{
    if (glCreateShader == 0) std::cout << "glCreateShader not loaded" << std::endl;
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
        std::cout << &vertShaderError[0] << std::endl;
    }

    // Compile fragment shader
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
        std::cout << &fragShaderError[0] << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    for (std::map<GLuint, std::string>::const_iterator i = attrLoc.begin(); i != attrLoc.end(); ++i)
        glBindAttribLocation(program, i->first, i->second.c_str());

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError( (logLength > 1) ? logLength : 1 );
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        std::cout << &programError[0] << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}
