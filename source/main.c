
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_VRAM        0x06000000
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   160
typedef u16             M3LINE[SCREEN_WIDTH];
#define m3_mem          ((M3LINE*)MEM_VRAM)

struct Rect {
	int x;
	int y;
	int width;
	int height;
	int hSpeed;
	int vSpeed;
	int xPrev;
	int yPrev;
};

void drawPixel(int x, int y, int color) {
	m3_mem[y][x] = color;
}

void drawRect(struct Rect* cRect) {
	for (int i = cRect->x; i< cRect->x + cRect->width; i++) {
		for( int j = cRect->y; j < cRect->y + cRect->height; j++) {
			drawPixel(i, j, 0x7FFF);
		}
	}
}

void clearPrevious(struct Rect* cRect) {
	for (int i = cRect->xPrev; i< cRect->xPrev + cRect->width; i++) {
		for( int j = cRect->yPrev; j < cRect->yPrev + cRect->height; j++) {
			drawPixel(i, j, 0x0000);
		}
	}
}

int main(void) {
	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);
    SetMode( MODE_3 | BG2_ON );

	// Players and ball
	struct Rect playerOne;
	playerOne.x = 8;
	playerOne.y = SCREEN_HEIGHT/2;
	playerOne.width = 6;
	playerOne.height = 24;
	playerOne.hSpeed = 0;
	playerOne.vSpeed = 0;
	playerOne.yPrev = 0;
	playerOne.xPrev = 0;

	struct Rect playerTwo;
	playerTwo.width = 6;
	playerTwo.height = 24;
	playerTwo.y = SCREEN_HEIGHT/2;
	playerTwo.x = SCREEN_WIDTH - playerTwo.width - 8;
	playerTwo.hSpeed = 0;
	playerTwo.vSpeed = 0;
	playerTwo.yPrev = 0;
	playerTwo.xPrev = 0;

	struct Rect ball;
	ball.height = 8;
	ball.width = 8;
	ball.x = SCREEN_WIDTH/2 - ball.width/2;
	ball.y = SCREEN_HEIGHT / 2;
	ball.hSpeed = 0;
	ball.vSpeed = 0;
	ball.yPrev = 0;
	ball.xPrev = 0;
	
	// Center line
	for(int j=0; j<SCREEN_HEIGHT; j++) {
		drawPixel(SCREEN_WIDTH/2, j, 0x7FFF);
	}
	
	// Render loop
	while (1) {
		VBlankIntrWait();

		// Input
		scanKeys();
		int pressedKeys = keysDown();
		int releasedKeys = keysUp();
		
		if (pressedKeys & KEY_UP) {
			playerOne.vSpeed = -2;
		}
		if (pressedKeys & KEY_DOWN) {
			playerOne.vSpeed = 2;
		}
		playerOne.y += playerOne.vSpeed;
		if ((playerOne.y >= SCREEN_HEIGHT - playerOne.height) || (playerOne.y <= 0)) {
			playerOne.vSpeed = 0;
			playerOne.y = playerOne.yPrev;
		}
		if ((releasedKeys & KEY_UP) || (releasedKeys & KEY_DOWN)) {
			playerOne.vSpeed = 0;
		}
		
		clearPrevious(&playerOne);
		clearPrevious(&playerTwo);
		clearPrevious(&ball);

		drawRect(&playerOne);
		drawRect(&playerTwo);
		drawRect(&ball);

		playerOne.yPrev = playerOne.y;
		playerOne.xPrev = playerOne.x;
	}
}


