#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

void setupInput() {
}
void setupGraphics() {
}
void drawGraphics() {
}
int main(int argc, char **argv) {
	setupGraphics();
	setupInput();
	// Initialize the Chip8 system and load the game into the memory
	chip8 myChip8;
	
	// Emulation loop
	while(1) {
		// Emulate one cycle
		myChip8.emulateCycle();

		// If the draw flag is set, update the screen
		if (myChip8.drawFlag) {
			drawGraphics();
		}
		// Store key press state (Press and Release)
		myChip8.setKeys();
	}

	return 0;
}
