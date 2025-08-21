#include "chip8.h"
#include <iostream>
#include <chrono>

using namespace std;

int main(int argc, char* argv[])
{
    Chip8 chip8;
    int cycleDelay = stoi(argv[1]);
    chip8.loadRom(argv[2]);
    chip8.screenInit();
    chip8.screenUpdate();
    SDL_Event events;
    bool isOpen{ true };
    auto lastCycleTime = chrono::high_resolution_clock::now();

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

        auto currentTime = chrono::high_resolution_clock::now();
        float dt = chrono::duration<float, chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay){
            lastCycleTime = currentTime;
            chip8.Cycle(); 
            chip8.screenUpdate();
        }
    }

    chip8.screenQuit();
    return 0;
}