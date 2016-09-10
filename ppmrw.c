#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;

Pixel* pixmap;
FILE* fh;

char* getWidthAndHeight() {
	char checkAgain = 1;
	char* comments = malloc(sizeof(char) * 1000);
	
	while (checkAgain) {
		if (fgets(comments, 1000, fh) != NULL) {
			if (comments[0] == '#') {
				//fgets(comments, 1000, fh); // Ignore empty line
			} else {
				printf("%s", comments);
				checkAgain = 0;
			}
		}
	}
	return comments;
}

int loadPPM3() {
	char* widthAndHeight;
	widthAndHeight = getWidthAndHeight();
	printf("%s", widthAndHeight);
	return 0;
}

int main(int argc, char* argv[]) {
	int returnValue = 0;
	
	if (argc != 4) {
		fprintf(stderr, "Usage: ppmrw target input output");
		return 1;
	}
	
	char PPMFileType [3];
	
	fh = fopen(argv[2], "r");
	if (fh == NULL) {
		fprintf(stderr, "File not found.");
		return 1;
	} else {
		if (fgets(PPMFileType, 3, fh) != NULL) {
			int compare = strcmp(PPMFileType, "P3");
			if (!compare) {
				printf("We found P3\n");
				fgets(PPMFileType, 3, fh);	// Ignore empty line
				loadPPM3();
			} else {
				compare = strcmp(PPMFileType, "P6");
				if (!compare) {
					printf("We found P6\n");
				} else {
					fprintf(stderr, "Not a PPM file\n");
				}
			}
		}
		fclose(fh);
	}
	
	return returnValue;
}