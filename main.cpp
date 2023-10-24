#include <iostream>
#include <fstream>
#include <cstdio>
#include <SDL2/SDL.h>

using namespace std;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

struct rom { // Computer Read Only Memory (ROM), it stores instructions and variables
    u8 data[65536] = {};
    void initializeRom() {
        ifstream input("rom.bin", ios::binary);
        if (input) {
            input.read(reinterpret_cast<char*>(data), sizeof(data));
            input.close();
        } else {
            cout << "There was an error trying to load the ROM" << endl;
            cout << "Error Code: 1" << endl;
        }
    }
};

struct ram { // Computer Random Access Memory (RAM), volatile data, gets lost when the computer turns off
    u8 data[65536] = {};
};

struct display {
    void update(SDL_Renderer* renderer, u8 X, u8 Y, u8 RGB) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        u8 R = (RGB >> 4) & 0b00001111;
        u8 G = (RGB >> 2) & 0b00001111;
        u8 B = RGB & 0b00001111;
        R = R * 85;
        G = G * 85;
        B = B * 85;
        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
        SDL_Rect pixelRect = {X * 4, Y * 4, 4, 4};
        SDL_RenderFillRect(renderer, &pixelRect);
        SDL_RenderPresent(renderer);
    }

};

struct cpu { // The computer's heart, where the magic happens
    u8 registers[3] = {};
    u8 carry;
    u8 zero;
    u8 negative;
    u16 PC; // Program Counter
    u16 IR; // Instruction Register
    rom cpuRom;
    ram cpuRam;
    display cpuDisplay;
    u8 temp_1, temp_2, temp_3;
    u8* temp_p;
    u8 charData[3] = {0x4A, 0xAE, 0xA0};
    bool interruptEnable;
    bool interruptPending;
    SDL_Event event;
    u8 interruptKey;
    u16 SP;

    cpu() : PC(256), carry(0), zero(0), negative(0), interruptEnable(false), interruptPending(false), interruptKey(0), SP(0x00FF) {}

    void handleInterrupt() {
        if (interruptEnable && interruptPending) {
            interruptPending = false;
            //handle interrupts
            getchar();
        }
    }

    void triggerKeyboardInterrupt() {
        if (interruptEnable) {
            interruptPending = true;
        }
    }

    void handleKeyboardEvent(SDL_Event& event) {
        if (event.type == SDL_KEYUP) {
            u8 key = event.key.keysym.sym & 0xFF;
            printf("Key pressed = 0x%02X\n", (unsigned char)(key & 0xFF));
            cpuRam.data[0x7000] = key;
            interruptKey = 0;
            triggerKeyboardInterrupt();
        }
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYUP) {
                triggerKeyboardInterrupt();
            }
        }
    }

    u8 add(u8 R1, u8 R2) {
        u16 Sum = static_cast<u16>(R1) + static_cast<u16>(R2);
        carry = static_cast<u8>(Sum >> 8);
        u8 Result = static_cast<u8>(Sum);
        return Result;
    }

    u8 subtract(u8 R1, u8 R2) {
        u8 Result = R1 - R2;
        
        if (Result > R1) {
            negative = 1;
        } else {
            negative = 0;
        }
        
        return Result;
    }

    u8 multiply(u8 num1, u8 num2) {
        u16 Product = static_cast<u16>(num1) * static_cast<u16>(num2);
        carry = static_cast<u8>((Product >> 8) & 0xFF);
        return static_cast<u8>(Product);
    }

    u8 divide(u8 dividend, u8 divisor) {
        u8 quotient = 0;
        if (divisor != 0) {
            quotient = dividend / divisor;
            carry = dividend % divisor;
        } else {
            carry = 0;
        }
        return quotient;
}

    void fetch() {
        IR = cpuRom.data[PC];
        printf("Instruction Register = 0x%02X\n", (unsigned char)(IR & 0xFF));
        printf("Program Counter = 0x%04X\n", (unsigned short)(PC & 0xFFFF));
        printf("Register A = 0x%02X\n", (unsigned char)(registers[0] & 0xFF));
        printf("Register B = 0x%02X\n", (unsigned char)(registers[1] & 0xFF));
        printf("Register C = 0x%02X\n", (unsigned char)(registers[2] & 0xFF));
        printf("Carry = 0x%02X\n", (unsigned char)(carry & 0xFF));
        printf("Zero = 0x%02X\n", (unsigned char)(zero & 0xFF));
        printf("Negative = 0x%02X\n", (unsigned char)(negative & 0xFF));
        PC++;
    }

    void execute(SDL_Renderer* renderer) {
        if (interruptEnable) {
            handleInterrupt();
        }

        handleEvents();

        switch(IR) {
            case 0x00:
                break;
            case 0x01:
                // 01 regA data
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] = temp_2;
                break;
            case 0x02:
                // 02 address regA
                temp_p = &cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                *temp_p = registers[temp_2];
                break;
            case 0x03:
                // 03 regA regB
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = add(registers[temp_1], registers[temp_2]);
                break;
            case 0x04:
                // 04 regA regB
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = subtract(registers[temp_1], registers[temp_2]);
                break;
            case 0x05:
                // 05 regA regB
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = multiply(registers[temp_1], registers[temp_2]);
                break;
            case 0x06:
                // 06 regA regB
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = divide(registers[temp_1], registers[temp_2]);
                break;
            case 0x07:
                // 07 jump_address
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                PC = (static_cast<u16>(temp_1) << 8) | temp_2;
                break;
            case 0x08:
                // 08 jump_address
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                if (carry) {
                    PC = (static_cast<u16>(temp_1) << 8) | temp_2;
                }
                break;
            case 0x09:
                // 09 regA regB
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                if (registers[temp_1] == registers[temp_2]) {
                    carry = 1;
                } else {
                    carry = 0;
                }
                break;
            case 0x0A:
                // 0A regA amount
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] = add(registers[temp_1], temp_2);
                break;
            case 0x0B:
                // 0B regA amount
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] = add(registers[temp_1], ~(temp_2));
                break;
            case 0x0C:
                // 0C regA regB (AND operation)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = registers[temp_1] & registers[temp_2];
                break;
            case 0x0D:
                // 0D regA regB (OR operation)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = registers[temp_1] | registers[temp_2];
                break;
            case 0x0E:
                // 0E regA regB (XOR operation)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[2] = registers[temp_1] ^ registers[temp_2];
                break;
            case 0x0F:
                // 0F regA amount (Shift Left)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] <<= temp_2;
                break;
            case 0x10:
                // 10 regA amount (Shift Right)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] >>= temp_2;
                break;
            case 0x11:
                // 11 regA data (Load Immediate)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] = temp_2;
                break;
            case 0x12:
                // 12 regA regB (Move)
                temp_1 = cpuRom.data[PC];
                PC++;
                temp_2 = cpuRom.data[PC];
                PC++;
                registers[temp_1] = registers[temp_2];
                break;
            // case 0x13:
            //     // 13 display xy
            //     temp_1 = cpuRom.data[PC];
            //     PC++;
            //     temp_2 = cpuRom.data[PC];
            //     PC++;
            //     temp_3 = cpuRom.data[PC];
            //     PC++;
            //     cpuDisplay.update(renderer, temp_1, temp_2, temp_3);
            //     break;
            case 0xF0:
                interruptEnable = true;
                break;
            case 0xF1:
                interruptEnable = false;
                break;
            default:
                break;
        }
    }

    void run(SDL_Renderer* renderer) {
        cpuRom.initializeRom();
        while (PC < 65536) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_KEYUP) {
                    handleKeyboardEvent(event);
                }
            }
            fetch();
            execute(renderer);
            getchar();
        }
    }
};

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("GR-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);

    cpu CPU;
    CPU.run(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}