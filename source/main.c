
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"

void bounceFromPlayer(struct Rect *player, struct Rect *ball) {
	int yHit = (ball->y + (ball->height/2)) - (player->y + (player->height/2));
	ball->vSpeed = ((yHit > 0) - (yHit < 0)) + ((yHit > 3) - (yHit < -3)) + ((yHit > 6) - (yHit < -6));
	ball->hSpeed = ((ball->hSpeed < 0) - (ball->hSpeed > 0)) * (1 + (1 - ((yHit > 4) || (yHit < -4))));
}

bool collision(
    int x1, int y1, int width1, int height1,
    int x2, int y2, int width2, int height2) {
    if (x1 + width1 > x2 &&
        x1 < x2 + width2 &&
        y1 + height1 > y2 &&
        y1 < y2 + height2) return true;
    return false;
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
	ball.hSpeed = 1;
	ball.vSpeed = 0;
	ball.yPrev = 0;
	ball.xPrev = 0;
	
	// Render loop
	while (1) {
		VBlankIntrWait();

		// Player One movement
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

		// Computer player movement
		if (ball.x > (SCREEN_WIDTH / 2)) {
			if ((ball.y + ball.height / 2) < playerTwo.y) {
				playerTwo.vSpeed = -2;
			}
			if ((ball.y + ball.height / 2) > (playerTwo.y + playerTwo.height)) {
				playerTwo.vSpeed = 2;
			}
		} else {
			// Return to the center of the screen
			if ((playerTwo.y + playerTwo.width / 2) > SCREEN_HEIGHT / 2) {
				playerTwo.vSpeed = -2;
			} else {
				playerTwo.vSpeed = 2;
			}
		}
		playerTwo.y += playerTwo.vSpeed;
		

		// Ball movement
		ball.y += ball.vSpeed;
		ball.x += ball.hSpeed;
		// Wall collisions
		if (ball.y <= 0 && ball.vSpeed < 0) {
			ball.vSpeed *= -1;
		} else if ((ball.y + ball.height) >= SCREEN_HEIGHT && ball.vSpeed > 0) {
			ball.vSpeed *= -1;
		}
		// Player collisions
		if (collision(playerOne.x, playerOne.y, playerOne.width, playerOne.height, ball.x, ball.y, ball.width, ball.height) && ball.hSpeed < 0) {
			bounceFromPlayer(&playerOne, &ball);
		}
		if (collision(playerTwo.x, playerTwo.y, playerTwo.width, playerTwo.height, ball.x, ball.y, ball.width, ball.height) && ball.hSpeed > 0) {
			bounceFromPlayer(&playerTwo, &ball);
		}
		// Scores
		if (ball.x > SCREEN_WIDTH) {
			// PlayerOne scored
			ball.x = SCREEN_WIDTH/2 - ball.width/2;
			ball.y = SCREEN_HEIGHT / 2;
			ball.hSpeed = -1;
			ball.vSpeed = 0;
		}

		if ((ball.x + ball.width) < 0) {
			// PlayerTwo scored
			ball.x = SCREEN_WIDTH/2 - ball.width/2;
			ball.y = SCREEN_HEIGHT / 2;
			ball.hSpeed = 1;
			ball.vSpeed = 0;
		}

		// Graphics
		clearPrevious(&playerOne);
		clearPrevious(&playerTwo);
		clearPrevious(&ball);

		// Draw Center line
		for(int j=0; j<SCREEN_HEIGHT; j++) {
			drawPixel(SCREEN_WIDTH/2, j, 0x7FFF);
		}

		drawRect(&playerOne);
		drawRect(&playerTwo);
		drawRect(&ball);

		playerOne.yPrev = playerOne.y;
		playerOne.xPrev = playerOne.x;
		playerTwo.yPrev = playerTwo.y;
		playerTwo.xPrev = playerTwo.x;
		ball.yPrev = ball.y;
		ball.xPrev = ball.x;
	}
}


