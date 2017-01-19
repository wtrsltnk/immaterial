
#include <GL/glextl.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "sdl2-setup.h"

SDLProgram::SDLProgram()
    : width(640), height(480), title("SDLWindow"), keepRunning(true)
{ }

SDLProgram::~SDLProgram()
{ }

void handleInput(SDL_Event& event, Input* input);

int SDLProgram::Run(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        this->args.push_back(argv[i]);
    }

    this->_input = new Input();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        std::cout << "Unable to initialize SDL" << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    this->_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (this->_window == 0)
    {
        std::cout << "Unable to create Window" << std::endl;
        return -1;
    }

    this->_context = SDL_GL_CreateContext(this->_window);
    if (this->_context == 0)
    {
        std::cout << "Unable to create GL context" << std::endl;
        return -1;
    }

    glExtLoadAll(reinterpret_cast<PFNGLGETPROC*>(SDL_GL_GetProcAddress));

    if (this->SetUp())
    {
        this->OnResize(width, height);

        SDL_Event event;
        std::cout << "Graphics initialized" << std::endl;
        while (this->keepRunning)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_WINDOWEVENT)
                {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        this->OnResize(event.window.data1, event.window.data2);
                        this->width = event.window.data1;
                        this->height = event.window.data2;
                    }
                    else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                        this->keepRunning = false;
                }
                else
                {
                    handleInput(event, this->_input);
                }
            }
            SDL_PumpEvents();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            this->Render();

            // Swap front and back rendering buffers
            SDL_GL_SwapWindow(this->_window);
        }
        this->CleanUp();
    }

    SDL_Quit();

    return 0;
}

IInput* SDLProgram::input()
{
    return this->_input;
}

float SDLProgram::elapsed() const
{
    return SDL_GetTicks() / 1000.0f;
}

void handleInput(SDL_Event& event, Input* input)
{
    if (event.type == SDL_MOUSEMOTION)
    {
        input->OnMouseMove(double(event.motion.x), double(event.motion.y));
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
    {
        auto state = event.type == SDL_MOUSEBUTTONDOWN ? ActionStates::Pressed : ActionStates::Released;
        switch (event.button.button)
        {
        case SDL_BUTTON_LEFT:   input->OnMouseAction(MouseButtons::Left, state); break;
        case SDL_BUTTON_RIGHT:  input->OnMouseAction(MouseButtons::Right, state); break;
        case SDL_BUTTON_MIDDLE: input->OnMouseAction(MouseButtons::Middle, state); break;
        case SDL_BUTTON_X1:     input->OnMouseAction(MouseButtons::Button4, state); break;
        case SDL_BUTTON_X2:     input->OnMouseAction(MouseButtons::Button5, state); break;
        }
    }
    else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
        auto state = event.type == SDL_KEYDOWN ? ActionStates::Pressed : ActionStates::Released;
        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
            input->OnKeyAction(static_cast<KeyboardKeys>(int(KeyboardKeys::Character_A) + int(event.key.keysym.sym - SDLK_a)), state);
        else
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_BACKSPACE:    input->OnKeyAction(KeyboardKeys::Backspace, state); break;
            case SDLK_RETURN:       input->OnKeyAction(KeyboardKeys::Return, state); break;
            case SDLK_SPACE:        input->OnKeyAction(KeyboardKeys::Character_Space, state); break;
            case SDLK_ESCAPE:       input->OnKeyAction(KeyboardKeys::Escape, state); break;
            case SDLK_TAB:          input->OnKeyAction(KeyboardKeys::Tab, state); break;
            }
        }
    }
}
