#include <GL/glextl.h>

#include "sdl2-setup.h"
#include "font-icons.h"
#include "font.h"
#include "ui.h"

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
    FontIcons _materialIcons, _fontawesome;
    Font _font24;
};

Program::~Program() { }

bool Program::SetUp()
{
    this->_materialIcons.init("..\\immaterial\\MaterialIcons-Regular.ttf", 28.0f, 0xe000);
    this->_fontawesome.init("..\\immaterial\\fontawesome-webfont.ttf", 28.0f, 0xf000);
    this->_font24.init("..\\immaterial\\Ubuntu-B.ttf", 24.0f);

    UI::Manager().init("..\\immaterial\\Ubuntu-B.ttf", 28.0f);

    flUI("container")
            .Cntr()
            .UseColor(234, 189, 121)
            .Size(100, 110)
            .Padding(15, 15)
            .AddChild(flUI("pointsLabel")
                      .Lbl("SCORE:").UseFont(this->_font24).UseColor(234, 189, 121)
                      )
            .AddChild(flUI("completionsLabel")
                      .Lbl("COMP:").UseFont(this->_font24).UseColor(234, 189, 121)
                      )
            .AddChild(flUI("yardsLabel")
                      .Lbl("YDS:").UseFont(this->_font24).UseColor(234, 189, 121)
                      )
            .AddChild(flUI("passingRatingLabel")
                      .Lbl("QBR:").UseFont(this->_font24).UseColor(234, 189, 121)
                      )
            .AddChild(flUI("streakLabel")
                      .Lbl("STREAK:").UseFont(this->_font24).UseColor(234, 189, 121)
                      )
            .AddControl();

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
    glDisable(GL_DEPTH_TEST);

    float padding = 5.0f;

    float x = 10.0f, y = 30.0f;
    for (int i = 0; i < 1024; i++)
    {
        _materialIcons.DrawIcon(this->_proj, glm::mat4(1.0f), x, y, _materialIcons.IconOffset() + i);
        x += _materialIcons.Size() + padding;
        if (x > this->width - 30.0f) { x = 10.0f; y += _materialIcons.Size() + padding; }
    }

    x = 10.0f; y += _materialIcons.Size();

    for (int i = 0; i < 1024; i++)
    {
        _fontawesome.DrawIcon(this->_proj, glm::mat4(1.0f), x, y, _fontawesome.IconOffset() + i);
        x += _fontawesome.Size();
        if (x > this->width - 30.0f) { x = 10.0f; y += _fontawesome.Size(); }
    }

    UI::Manager().render();
}

void Program::CleanUp() { }

void Program::OnResize(int w, int h)
{
    if (h < 1) h = 1;

    glViewport(0, 0, w, h);
    UI::Manager().resize(width, height);

    this->_proj = glm::ortho(0.0f, float(w), float(h), 0.0f);
}

int main(int argc, char* argv[])
{
    return Program().Run(argc, argv);
}
