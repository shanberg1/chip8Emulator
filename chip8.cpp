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
			if (V[(opocode & 0x0F00) >> 8] == opcode & 0x00FF) {
				pc += 4;
			} else {
				pc += 2;
			}
		break;
		case 0x4000: // Skips the next instruction if VX doesn't equal NN
			if (V[(opocode & 0x0F00) >> 8] != opcode & 0x00FF) {
				pc += 4;
			} else {
				pc += 2;
			}
		break;
		case 0x5000: // Skips the next instruction if VX equals VY
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) << 4]) {
				pc += 4;
			} else {
				pc += 2;
			}
		break;
		case 0x6000: // opcode 6XNN sets VX to NN
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
		break;
		case 0x7000: // opcode 7XNN Adds NN to VX
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			pc += 2;
		break;
		case 0x8000:
			switch (opcode & 0xF00F) {
				case 0x8000: // opcode 8XY0 sets VX to the value of VY
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;
				case 0x8001: // opcode 8XY1 set VX to V[x] | V[y]
					V[opcode & 0x0F00] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;
				case 0x8002: // opcode 8XY2 set VX to V[x] & V[y]
					V[opcode & 0x0F00] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;
				case 0x8003: // opcode 8XY3 set Vx to Vx xor Vy
					V[opcode & 0x0F00] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;
				case 0x8004: // opcode 8XY4 adds vy to vx.  VF is set to 1 when there's a carry and to 0 when there isn't
					int result = ((opcode & 0x0F00) >> 8) + ((opcode & 0x00F0) >> 4);
					if (result > 255) {
						V[0xF] = 1;
					} else {
						V[(opcode & 0x0F00) >> 8] = result;
					}
					pc += 2;
				break;
				case 0x8005: // opcode 8XY5 VY is subtracted from VX.  VF is set to 0 when there's a borrow, and 1 when there isn't
					// TODO fix
					int result = ((opcode & 0x0F00) >> 8) - ((opcode & 0x00F0) >> 4);
					if (result < 0) {
						V[0xF] = 0
					} else {
						V[(opcode & 0x0F00) >> 8] = (unsigned char) result;
						V[0xF] = 1;
					}
					pc += 2;
				break;
				case 0x8006: // opcode 8XY6 Shifts VX right by one.  VF is set to the value of the least significant bit of VX before the shift
					char F = 0x000F & opcode;
					V[(opcode & 0x0F00) >> 8] = opcode >> 1 & 0x0FFF;
				break;
				case 0x8007: // opcode 8XY7 Sets VX to VY minus VX.  VF is set to 0 when there's a borrow, and 1 when there isn't
					// TODO fix
					int result = ((opcode & 0x00F0) >> 8) - ((opcode & 0x0F00) >> 4);
					if (result < 0) {
						V[0xF] = 0
					} else {
						V[(opcode & 0x0F00) >> 8] = (unsigned char) result;
						V[0xF] = 1;
					}
					pc += 2;
				break;
				case 0x800E: // opcode 8XYE shifts VX left by one.  VF is set to the value of the most significant bit of VX before the shift
					char F = 0xF000 & opcode;
					V[(opcode & 0x0F00) >> 8] = opcode << 1 & 0xFFF0;
					p c+= 2;
				break;
			}
		break;
		case 0x9000: //Skips the next instruction if VX doesn't equal VY
			if (V[opcode & 0x0F00] != V[opcode & 0x00F0]) {
				pc+=4;
				break;
			}
			pc += 2;
		break;
		case 0xA000: // ANNN: Sets I to the address NNN
			// Execute opcode
			I = opcode & 0x0FFF;
			pc += 2;
		break;
		case 0xB000: // BNNN Jumps to the address NNN plus V0
			pc = V[0] + (opcode & 0x0FFF);
		break;
		case 0xC000: // CXNN Sets VX to the result of a bitwise and operation on a randon number and NN
			V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & rand();
			pc+=2;
		}
		break;
		case 0xD000: // Sprites stored in memory at location in index register (I), 8bits wide.  
			     // Wraps around the screen.  If when drawn, clears a pixel, register VF is 
			     // set to 1 otherwise it is zero.  All drawing is XOR drawing(i.e. it toggles
			     // the screen to pixels).  Sprites are drawn starting at position VX, VY.  N
			     // is the number of 8bit rows that need to be drawn.  If N is greater than 1,
			     //second line continues at position VX,VY+1, and so on.
			 // Not my code for this opcode
			unsigned short x = (opcode & 0x0F00) >> 8;
			unsigned short y = (opcode & 0x00F0) >> 4;
			unsigned short height = (opcode & 0x000F);
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++) {
			       pixel = memory[I + yline];
			       for(int xline = 0; xline < 8; xline++) {
			       		if((pixel & (0x80 >> xline)) != 0) {
						if(gfx[(x + xline + ((y + yline) * 64))] == 1)
							V[0xF] = 1;                                 
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
											  
			drawFlag = true;
			pc += 2;
			}
		break;
		case 0xE000:
			switch (opcode & 0x00FF) {
				case 0xE09E: // EX9E Skips the next instruction if the key stored in VX is pressed
					if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
						pc += 4;
						break;
					}
					pc += 2;
				break;
				case 0xE0A1: // EXA1 Skips the next instruction if the key stored in VX isn't pressed
					if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
						pc += 4;
					} else {
						pc += 2;
					}
				break;
			}
		break;
		case 0xF000:
			switch (opcode & 0xF0FF) {
				case 0xF007: // FX07 Sets VX to the value of the delay timer
					V[(opcode & 0x0F00) >> 8] = sound_timer;
					pc += 2;
				break;
				case 0xF00A: // FX0A a key press is awaited, and then sotred in VX
					bool t;
				 	for (int i = 0; i < 16; i++); {
						if (key[i] != 0) {
							V[(opcode & 0x0F00) >> 8] = i;
							t = true;
						}
					}
					if (t == false) {
						return;
					}
					pc += 2;
				break;
				case 0xF015: // FX15 Sets the delay timer to VX
					delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;
				case 0xF018: // FX18 Sets sound timer to VX
					sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;
				case 0xF01E: // FX1E Adds VX to I
					I = (opcode & 0x0F00) >> 8;
					pc += 2;
				break;
				case 0xF029: // FX29 Sets I to the location of the sprite for the character in vx.  
					     // Characters 0-F (in hexadecimal) are represented by a 4x5 font
					unsigned char character = V[(opcode & 0x0F00) >> 8];
					if (character < 0 || character > 15) {
						printf("Error in FX29: character not 0-F");
						exit(1);
					}
					I = 5 * character;
					pc += 2;
				break;
				case 0xF033: // FX33 Stores the binary-coded decimal representation of VX, 
					     // with the most significant of three digits at the address in I, 
					     // the middle digit at I plus 1, and the least significant digit at I plus 2. 
					     // (In other words, take the decimal representation of VX, place the hundreds 
					     // digit in memory at location in I, the tens digit at location I+1, and the 
					     // ones digit at location I+2.)	
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I+1] = (V[((opcode & 0x0F00) >> 8)] / 10) % 10;
					memory[I+2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
					pc += 2;
				break;
				case 0xF055: // Stores Vo to VX (including VX) in memory starting at address I.
					if (I >= 4096 || I < 80) {
						printf("I is out of range");
						exit(1);
					}
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
						memory[I + i] = V[i];
					}
					pc += 2;
				break;
				case 0xF065: // Fills V0 to VX (including VX) with values from memory starting at address I
					if (I >= 4096 || I < 80) {
						printf("I is out of range");
						exit(1);
					}
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
						V[i] = memory[I + i]
					}
					pc += 2;
				break;
			}
		break;			
			
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
	}

	// Update timers
	if (delay_timer > 0) {
		delay_timer--;
	}
	if (sound_timer == 1) {
		printf("BEEP!\n");
		sound_timer--;
	}
}
chip8::void debugRender() {
}
chip8::bool loadApplication(const char * filename){
}
