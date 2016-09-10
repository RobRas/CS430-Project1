#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;

Pixel* pixmap;
FILE* fh;



int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: ppmrw target input output");
		return 1;
	}
	
	char PPMFileType [3];
	
	fh = fopen(argv[2], "r");
	if (fh == NULL) {
		fprintf(stderr, "File not found.");
	} else {
		if (fgets(PPMFileType, 3, fh) != NULL) {
			int compare = strcmp(PPMFileType, "P3");
			if (compare) {
				printf("We found P3");
			} else {
				compare = strcmp(PPMFileType, "P6");
				if (compare) {
					printf("We found P6");
				} else {
					fprintf(stderr, "Not a PPM file");
				}
			}
		}
		fclose(fh);
	}
	
	return 0;
}