class chip8 {
	public:
		chip8();
		~chip8();

		bool drawFlag;
		void emulateCycle();
		void debugRender();
		bool loadApplication(const char * filename);
		void setKeys();
		unsigned char gfx[64 * 32];
		unsigned char key[16];
	private:
		unsigned short opcode;
		unsigned char memory[4096];
		unsigned char V[16]; 		// Registers
		unsigned short I;   		// Index register I
		unsigned short pc;		// Program counter
		unsigned char delay_timer;
		unsigned char sound_timer;
		unsigned short stack[16];
		unsigned short sp;

		void init();
};


