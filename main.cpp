#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

chip8 myChip8;

int main(int argc, char **argv) {
	if (argc < 1) {
		printf("No game");
		exit(1);
	}
	char * game = strdup(argv[1]);
	setupGraphics();
	setupInput();

	// Initialize the Chip8 system and load the game into the memory
	myChip8.initialize();
	myChip8.loadGame(game);
	
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
