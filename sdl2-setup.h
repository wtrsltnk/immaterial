#ifndef _SDL_SETUP_HPP_
#define _SDL_SETUP_HPP_

#include <string>
#include <vector>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "input.h"

class SDLProgram
{
public:
    SDLProgram();
    virtual ~SDLProgram();

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
    SDL_Window* _window;
    SDL_GLContext _context;
    Input* _input;

};

#endif // _SDL_SETUP_HPP_
