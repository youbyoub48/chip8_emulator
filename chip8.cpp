#include "chip8.h"
#include <fstream>
#include <iostream>

using namespace std;

void Chip8::loadRom(const char* filename){
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

Chip8::Chip8(){
    pc = START_ADDRESS;
    const int FONTSET_SIZE = 80;

    uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (int i=0;i<FONTSET_SIZE;i++){
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

    for(int x=0;x<64;x++){
        for(int y=0;y<32;y++) screen[x][y] = false;
    }

}

int Chip8::screenInit(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        return EXIT_FAILURE;
    }

     if (SDL_CreateWindowAndRenderer(64*PIXEL_SIZE, 32*PIXEL_SIZE, SDL_WINDOW_SHOWN, &pWindow, &pRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    return 1;
}

void Chip8::screenQuit(){
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

void Chip8::screenUpdate(){
    SDL_RenderClear(pRenderer);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_Rect pixel{ 0, 0, PIXEL_SIZE, PIXEL_SIZE };

    for(int x=0;x<64;x++){
        for(int y=0;y<32;y++){
            if(screen[x][y]) SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
            else SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
            pixel.x = x*PIXEL_SIZE;
            pixel.y = y*PIXEL_SIZE;
            SDL_RenderFillRect(pRenderer, &pixel);
        }
    }
    SDL_RenderPresent(pRenderer);  
}

//Instruction function

// 00E0 - CLS
void Chip8::OP_00E0(){ //Clear the display.
    for(int x=0;x<64;x++){
        for(int y=0;y<32;y++){
            screen[x][y] = false;
        }
    }
}

// 00EE - RET
void Chip8::OP_00EE(){ //Return from a subroutine.
    if (stack_count == 0) throw runtime_error("The Stack is empty 00EE");
    stack_count--;
    pc = stack[stack_count];
}

// 1nnn - JP addr
void Chip8::OP_1nnn(){ //Jump to location nnn.
    pc = opcode & 0x0FFF;
}

// 2nnn - CALL addr
void Chip8::OP_2nnn(){ //Call subroutine at nnn.
    stack[stack_count] = pc;
    stack_count++;
    pc = opcode & 0x0FFF;
}

// 3xkk - SE Vx, byte
void Chip8::OP_3xkk(){ //Skip next instruction if Vx = kk.
    uint8_t x = (opcode && 0x0F00) >> 8;
    if(V[x] == (opcode && 0x00FF)) pc += 2;
}

// 4xkk - SNE Vx, byte
void Chip8::OP_4xkk(){ //Skip next instruction if Vx != kk.
    uint8_t x = (opcode && 0x0F00) >> 8;
    if(V[x] != (opcode && 0x00FF)) pc += 2;
}

// 5xy0 - SE Vx, Vy
void Chip8::OP_5xy0(){ //Skip next instruction if Vx = Vy.
    uint8_t x = (opcode && 0x0F00) >> 8;
    uint8_t y = (opcode && 0x00F0) >> 4;

    if(V[x] == V[y]) pc += 2;
}

// 6xkk - LD Vx, byte
void Chip8::OP_6xkk(){ //Set Vx = kk.
    uint8_t x = (opcode && 0x0F00) >> 8;
    V[x] = opcode & 0x00FF;
}