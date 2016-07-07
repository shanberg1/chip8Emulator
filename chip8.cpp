#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "chip8.h"

unsigned char chip8_fontset[80] =
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

chip8::chip8(){
	// Empty
}
chip8::~chip8() {
	// Empty
}
chip8::void init() {
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	// clear display
	for (int i = 0; i < 64 * 32; i++) {
		gfx[i] = 0;
	}
	// clear stack
	for (int i = 0; i < 16; i++) {
		stack[i] = 0;
	}
	sp = 0;
	// clear registers v0-vg
	for (int i = 0; i < 16; i++) {
		V[i] = 0;
	}
	// clear memory
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}
	// Load fontset
	for (int i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[80];
	}
	// reset timers
	delay_timer = 0;
	sound_timer = 0;
}
chip8::void emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x000F) { 
				case: 0x0000 // Opcode 00E0 Clear the Screen
					for (int i = 0; i < 64 * 32; i++) {
						gfx[i] =0;
					}
					drawFlag = true;
					pc+=2;
				break;
				case: 0x000E // Returns from a subroutine
					sp--;
					pc = stack[sp];
					pc += 2;
				break;
			}
		break;
		case 0x1000: // Jumps to address NNN
			pc = opcode & 0x0FFF;
		break;
		case 0x2000: // Calls subroutine at NNN
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
		break;
		case 0x3000: // Skips the next instruction if VX doesn't equal NN
			if (V[opocode & 0x0F00 >> 8] == opcode & 0x00FF) {
				pc += 4;
			} else {
				pc += 2;
			}
		break;
		case 0x4000: // Skips the next instruction if VX doesn't equal NN
			if (V[opocode & 0x0F00 >> 8] != opcode & 0x00FF) {
				pc += 4;
			} else {
				pc += 2;
			}
		break;
		case 0x5000: // Skips the next instruction if VX equals VY
			if (V[opcode & 0x0F00 >> 8] == V[opcode & 0x00F0 << 4]) {
				pc += 4;
			} else {
				pc += 2;
			}
		break;
		case 0x6000: // opcode 6XNN sets VX to NN
			V[opcode & 0x0F00 >> 8] = opcode & 0x00FF;
			pc += 2;
		break;
		case 0x7000: // opcode 7XNN Adds NN to VX
			V[opcode & 0x0F00 >> 8] += 0x00FF;
			pc += 2;
		break;
		case 0x8000:
			switch (opcode & 0xF00F) {
				case 0x8000: // opcode 8XY0 sets VX to the value of VY
					V[opcode & 0x0F00 >> 8] = V[opcode & 0x00F0 >> 4];
					pc += 2;
				break;
				case 0x8001: // opcode 8XY1 set VX to V[x] | V[y]
					V[opcode & 0x0F00] = V[opcode & 0x0F00 >> 8] | V[opcode & 0x00F0 >> 4];
					pc += 2;
				break;
				case 0x8002: // opcode 8XY2 set VX to V[x] & V[y]
					V[opcode & 0x0F00] = V[opcode & 0x0F00 >> 8] & V[opcode & 0x00F0 >> 4];
					pc += 2;
				break;
				case 0x8003: // opcode 8XY3 set Vx to Vx xor Vy
					V[opcode & 0x0F00] = V[opcode & 0x0F00 >> 8] ^ V[opcode & 0x00F0 >> 4];
					pc += 2;
				break;
				case 0x8004: // opcode 8XY4 adds vy to vx.  VF is set to 1 when there's a carry and to 0 when there isn't
					// TODO
				break;
			
		case 0xA000: // ANNN: Sets I to the address NNN
			// Execute opcode
			I = opcode & 0x0FFF;
			pc += 2;
		break;

		//more opcodes 

		default:
			printf("Unknown opcode: 0x%X\n", opcode);
	}

	// Update timers
	if (delay_timer > 0) {
		if (sound_timer == 1) {
			printf("BEEP!\n");
			sound_timer--;
		}
	}
}
chip8::void debugRender() {
}
chip8::bool loadApplication(const char * filename){
}
