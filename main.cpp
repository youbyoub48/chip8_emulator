#include "chip8.h"

int main(int argc, char* argv[])
{
    Chip8 chip8;
    chip8.screenInit();
    chip8.screenUpdate();
    SDL_Event events;
    bool isOpen{ true };

     while (isOpen)
    {
         while (SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_QUIT:
                isOpen = false;
                break;
            }
        }
    }

    chip8.screenQuit();
    return 0;
}