#include <stdlib.h>
#include <stdio.h>

typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;

Pixel* pixmap;

int width, height;

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: ppmrw target input output");
		return 1;
	}
	
	FILE* fh = fopen("output.data", "w");
	fwrite(argv[1], sizeof(char), 1, fh);
	fclose(fh);
	
	return 0;
}