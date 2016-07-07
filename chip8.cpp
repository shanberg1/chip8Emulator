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
chip8::bool drawFlag() {
}
chip8::void emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch (opcode & 0xF000) {
		// some opcodes
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
