#include "chip8.h"
#include <fstream>
#include <iostream>
#include <ctime>

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
    uint8_t x = (opcode & 0x0F00) >> 8;
    if(V[x] == (opcode & 0x00FF)) pc += 2;
}

// 4xkk - SNE Vx, byte
void Chip8::OP_4xkk(){ //Skip next instruction if Vx != kk.
    uint8_t x = (opcode & 0x0F00) >> 8;
    if(V[x] != (opcode & 0x00FF)) pc += 2;
}

// 5xy0 - SE Vx, Vy
void Chip8::OP_5xy0(){ //Skip next instruction if Vx = Vy.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if(V[x] == V[y]) pc += 2;
}

// 6xkk - LD Vx, byte
void Chip8::OP_6xkk(){ //Set Vx = kk.
    uint8_t x = (opcode & 0x0F00) >> 8;
    V[x] = opcode & 0x00FF;
}

// 7xkk - ADD Vx, byte
void Chip8::OP_7xkk(){ //Set Vx = Vx + kk.
    uint8_t x = (opcode & 0x0F00) >> 8;
    V[x] += opcode & 0x00FF;
}

// 8xy0 - LD Vx, Vy
void Chip8::OP_8xy0(){ //Set Vx = Vy.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    V[x] = V[y];
}

// 8xy1 - OR Vx, Vy
void Chip8::OP_8xy1(){ //Set Vx = Vx OR Vy.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    V[x] |= V[y];
}

// 8xy2 - AND Vx, Vy
void Chip8::OP_8xy2(){ //Set Vx = Vx AND Vy.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    V[x] &= V[y];
}

// 8xy3 - XOR Vx, Vy
void Chip8::OP_8xy3(){ //Set Vx = Vx XOR Vy.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    V[x] ^= V[y];
}

// 8xy4 - ADD Vx, Vy
void Chip8::OP_8xy4(){ //Set Vx = Vx + Vy, set VF = carry.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if((V[x]+V[y]) > 255){
        V[0xF] = 1;
        V[x] = (V[x]+V[y]) & 0xFF;
    }
    else{
        V[0xF] = 0;
        V[x] += V[y];
    }
}

// 8xy5 - SUB Vx, Vy
void Chip8::OP_8xy5(){ //Set Vx = Vx - Vy, set VF = NOT borrow.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if(V[x] > V[y]) V[0xF] = 1;
    else V[0xF] = 0;

    V[x] -= V[y];
}

// 8xy6 - SHR Vx {, Vy}
void Chip8::OP_8xy6(){ //Set Vx = Vx SHR 1.
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    V[0xF] = x & 0xF;
    V[x] >>= 1;
}

// 8xy7 - SUBN Vx, Vy
void Chip8::OP_8xy7(){ //Set Vx = Vy - Vx, set VF = NOT borrow.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if(V[y] > V[x]) V[0xF] = 1;
    else V[0xF] = 0;

    V[x] = V[y]-V[x];
}

// 8xyE - SHL Vx {, Vy}
void Chip8::OP_8xyE(){ //Set Vx = Vx SHL 1.
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    V[0xF] = x & 0xF;
    V[x] <<= 1;
}

// 9xy0 - SNE Vx, Vy
void Chip8::OP_9xy0(){ //Skip next instruction if Vx != Vy.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if(V[x] != V[y]) pc += 2;
}

// Annn - LD I, addr
void Chip8::OP_Annn(){ //Set I = nnn.
    I = opcode & 0x0FFF;
}

// Bnnn - JP V0, addr
void Chip8::OP_Bnnn(){ //Jump to location nnn + V0.
    pc = (opcode & 0x0FFF)+V[0];
}

// Cxkk - RND Vx, byte
void Chip8::OP_Cxkk(){ //Set Vx = random byte AND kk.
    srand(time(0));
    uint8_t x = (opcode & 0x0F00) >> 8;
    V[x] = (rand()%256) & (opcode & 0x00FF);
}

// Dxyn - DRW Vx, Vy, nibble
void Chip8::OP_Dxyn(){ //Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t sprite,pixel;

    for(int i=0;i<n;i++){
        sprite = memory[I+i];
        for(int j=0;j<8;j++){
            pixel = sprite & (0x80 >> j);
            
            if(pixel){
                if(screen[V[x]+j][V[y]+i]){
                    V[0xF] = 1;
                    screen[V[x]+j][V[y]+i] = false;
                }
                else{
                    V[0xF] = 0;
                    screen[V[x]+j][V[y]+i] = true;
                }
            }
        }
    }
}

// Ex9E - SKP Vx
void Chip8::OP_Ex9E(){ //Skip next instruction if key with the value of Vx is pressed.
    uint8_t x = (opcode & 0x0F00) >> 8;
    if(keypad[V[x]]) pc += 2;
}

// ExA1 - SKNP Vx
void Chip8::OP_ExA1(){ //Skip next instruction if key with the value of Vx is not pressed.
    uint8_t x = (opcode & 0x0F00) >> 8;
    if(!keypad[V[x]]) pc += 2;
}

// Fx07 - LD Vx, DT
void Chip8::OP_Fx07(){ //Set Vx = delay timer value.
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[x] = delay_timer;
}

// Fx0A - LD Vx, K
void Chip8::OP_Fx0A(){ //Wait for a key press, store the value of the key in Vx.
    uint8_t x = (opcode & 0x0F00) >> 8;

    for(int i=0;i<16;i++){
        if(keypad[i]){
            V[x] = i;
            return;
        }
    }
    pc -= 2;
}

// Fx15 - LD DT, Vx
void Chip8::OP_Fx15(){ //Set delay timer = Vx.
    uint8_t x = (opcode & 0x0F00) >> 8;
    delay_timer = V[x];
}

// Fx18 - LD ST, Vx
void Chip8::OP_Fx18(){ //Set sound timer = Vx.
    uint8_t x = (opcode & 0x0F00) >> 8;
    sound_timer = V[x];
}

// Fx1E - ADD I, Vx
void Chip8::OP_Fx1E(){ //Set I = I + Vx.
    uint8_t x = (opcode & 0x0F00) >> 8;
    I += V[x];
}

// Fx29 - LD F, Vx
void Chip8::OP_Fx29(){ //Set I = location of sprite for digit Vx.
    uint8_t x = (opcode & 0x0F00) >> 8;
    I = FONTSET_START_ADDRESS+(5*V[x]);
}

// Fx33 - LD B, Vx
void Chip8::OP_Fx33(){ //Store BCD representation of Vx in memory locations I, I+1, and I+2.
    uint8_t x = (opcode & 0x0F00) >> 8;

    memory[I] = V[x]/100;
    memory[I+1] = (V[x]%100)/10;
    memory[I+2] = V[x]%10;
}

// Fx55 - LD [I], Vx
void Chip8::OP_Fx55(){ //Store registers V0 through Vx in memory starting at location I.
    uint8_t x = (opcode & 0x0F00) >> 8;
    for(int i=0;i<=x;i++){
        memory[I+i] = V[i];
    }
}

// Fx65 - LD Vx, [I]
void Chip8::OP_Fx65(){ //Read registers V0 through Vx from memory starting at location I.
    uint8_t x = (opcode & 0x0F00) >> 8;
    for(int i=0;i<=x;i++){
        V[i] = memory[I+i];
    }
}