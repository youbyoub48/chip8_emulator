#include "chip8.h"

int main(int argc, char* argv[])
{
    Chip8 chip8;
    chip8.loadRom(argv[1]);
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

        for(int compteur=0;compteur<4;compteur++) 
        { 
            chip8.Cycle(); 
        } 
        chip8.screenUpdate();
        SDL_Delay(16);
    }

    chip8.screenQuit();
    return 0;
}