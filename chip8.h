#include <cstdint>
#include <SDL2/SDL.h>   

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50
#define PIXEL_SIZE 8


class Chip8{
    uint8_t V[16];
    uint8_t memory[4096];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t stack_count;
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool screen[64][32];
    bool keypad[16];
    uint16_t opcode;
    SDL_Window* pWindow{ nullptr };
    SDL_Renderer* pRenderer{ nullptr };
    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];

    public :
        void loadRom(const char* filename);
        Chip8();
        int screenInit();
        void screenQuit();
        void screenUpdate();
        void Table0();
        void Table8();
        void TableE();
        void TableF();
        void Cycle();
        void OP_NULL();
        void OP_00E0();
        void OP_00EE();
        void OP_1nnn();
        void OP_2nnn();
        void OP_3xkk();
        void OP_4xkk();
        void OP_5xy0();
        void OP_6xkk();
        void OP_7xkk();
        void OP_8xy0();
        void OP_8xy1();
        void OP_8xy2();
        void OP_8xy3();
        void OP_8xy4();
        void OP_8xy5();
        void OP_8xy6();
        void OP_8xy7();
        void OP_8xyE();
        void OP_9xy0();
        void OP_Annn();
        void OP_Bnnn();
        void OP_Cxkk();
        void OP_Dxyn();
        void OP_Ex9E();
        void OP_ExA1();
        void OP_Fx07();
        void OP_Fx0A();
        void OP_Fx15();
        void OP_Fx18();
        void OP_Fx1E();
        void OP_Fx29();
        void OP_Fx33();
        void OP_Fx55();
        void OP_Fx65();
};