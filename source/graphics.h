#define MEM_VRAM        0x06000000
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   160

#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0x7FFF

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

/* Clear entire screen (slow) */
void clearScreen() {
    for (int i=0; i<240; i++) {
        for (int j=0; j<160; j++) {
            m3_mem[j][i] = COLOR_BLACK;
        }
    }
}