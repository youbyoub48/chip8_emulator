#include <cstdint>

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50

class Chip8{
    uint8_t V[16];
    uint8_t memory[4096];
    uint16_t index;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t stack_count;
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool screen[64*32];
    uint8_t keypad[16];
    uint16_t opcode;
    public :
        void loadRom(const char* filename);
        Chip8();
};