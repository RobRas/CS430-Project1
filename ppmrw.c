#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;

char convertFrom;
char convertTo;

int width, height;

int maxColorValue;

Pixel* pixmap;
FILE* fh;

char* parseWidthAndHeight() {
	char checkAgain = 1;
	char* comments = malloc(sizeof(char) * 1000);
	
	while (checkAgain) {
		if (fgets(comments, 1000, fh) != NULL) {
			if (comments[0] != '#') {
				checkAgain = 0;
			}
		}
	}
	
	return comments;
}

int toDigit(char* input, int* output) {
	int i = 0;
	while(isdigit(input[i])) {
		i++;
	}
	if (input[i] != '\n' && input[i] != '\0' || i == 0) {
		fprintf(stderr, "Not a PPM file. Incompatible digit: %s\n", input);
		return 1;
	}
	
	*output = atoi(input);
	return 0;
}

int getWidthAndHeight(char* widthAndHeight) {
	// Get Width
	int i = 0;
	char w[10];
	while (isdigit(widthAndHeight[i]) && i < 10) {
		w[i] = widthAndHeight[i];
		i++;
	}
	if (i == 10) {
		fprintf(stderr, "File width too large.");
		return 1;
	} else if (widthAndHeight[i] != ' ') {
		fprintf(stderr, "Not a PPM file. Incompatible width.\n");
		return 1;
	} else {
		width = atoi(w);
		
		// Get Height
		i++;	// Skip space
		int j = 0;
		char h[8];
		while (isdigit(widthAndHeight[i]) && j < 8) {
			h[j] = widthAndHeight[i];
			i++;
			j++;
		}
		if (j == 8) {
			fprintf(stderr, "File height too large.");
			return 1;
		} else if (widthAndHeight[i] != '\n') {
			printf("Not a PPM File. Incompatible height.\n");
			return 1;
		} else {
			height = atoi(h);
		}
	}
	
	return 0;
}

int parsePPM3(int index) {
	char value[5];
	int rgbValue;
	
	if (fgets(value, 5, fh) != NULL) {
		if (toDigit(value, &rgbValue)) {
			return 1;
		}
		if (rgbValue > maxColorValue) {
			fprintf(stderr, "Color value exceeding max.");
			return 1;
		}
		pixmap[index].r = rgbValue;
	} else {
		fprintf(stderr, "File is not a PPM. Incompatible R value.\n");
		printf("%d", index);
		return 1;
	}
	
	if (fgets(value, 5, fh) != NULL) {
		if (toDigit(value, &rgbValue)) {
			return 1;
		}
		if (rgbValue > maxColorValue) {
			fprintf(stderr, "Color value exceeding max.");
			return 1;
		}
		pixmap[index].g = rgbValue;
	} else {
		fprintf(stderr, "File is not a PPM. Incompatible G value\n");
		return 1;
	}
	
	if (fgets(value, 5, fh) != NULL) {
		if (toDigit(value, &rgbValue)) {
			return 1;
		}
		if (rgbValue > maxColorValue) {
			fprintf(stderr, "Color value exceeding max.");
			return 1;
		}
		pixmap[index].b = rgbValue;
		return 0;
	} else {
		fprintf(stderr, "File is not a PPM. Incompatible B value\n");
		return 1;
	}
	
	return 0;
}

int getMaxColorValue() {
	char value[10];
	
	if (fgets(value, 10, fh) != NULL) {
		if (toDigit(value, &maxColorValue)) {
			return 1;
		}
		if (maxColorValue > 255) {
			fprintf(stderr, "Not a PPM file. Max color value too high.\n");
			return 1;
		} else if (maxColorValue < 1) {
			fprintf(stderr, "Not a PPM file. Max color value too low.\n");
			return 1;
		}
	}
	
	return 0;
}

int loadPPM3() {
	char* widthAndHeight;
	widthAndHeight = parseWidthAndHeight();
	if (getWidthAndHeight(widthAndHeight)) {
		return 1;
	}
	pixmap = malloc(sizeof(Pixel) * width * height);
	getMaxColorValue();
	int i = 0;
	while (i < width * height) {
		if (parsePPM3(i)) {
			return 1;
		}
		i++;
	}
	
	return 0;
}

int writePPM6() {
	char str[100];
	sprintf(str, "P6\n# Converted with Robert Rasmussen's ppmrw\n%d %d\n%d\n", width, height, maxColorValue);
	fwrite(str, sizeof(str), 1, fh);
	fwrite(pixmap, sizeof(Pixel), width*height, fh);
	return 0;
}

int getConversionType(char* arg) {
	int compare = strcmp(arg, "3");
	if (!compare) {
		compare = strcmp(arg, "6");
		if (!compare) {
			fprintf(stderr, "Target needs to be 3 or 6.\n");
			return 1;
		}
	}
	
	convertTo = arg[0];
	return 0;
}

int getPPMFileType() {
	char PPMFileType [4];
	
	if (fgets(PPMFileType, 4, fh) != NULL) {
		int compare = strcmp(PPMFileType, "P3\n");
		if (!compare) {
			convertFrom = '3';
			loadPPM3();
			
		} else {
			compare = strcmp(PPMFileType, "P6\n");
			if (!compare) {
				convertFrom = '6';
			} else {
				fprintf(stderr, "Not a PPM file. Incompatible file type.\n");
				return 1;
			}
		}
	}
	
	return 0;
}

int main(int argc, char* argv[]) {
	int returnValue = 0;
	
	if (argc != 4) {
		fprintf(stderr, "Usage: ppmrw target input output.\n");
		return 1;
	}
	
	if (getConversionType(argv[1])) {
		return 1;
	}
	
	fh = fopen(argv[2], "r");
	if (fh == NULL) {
		fprintf(stderr, "Input file not found.");
		return 1;
	}
	
	if (getPPMFileType()) {
		return 1;
	}
	
	fclose(fh);
	
	fh = fopen(argv[3], "w");
	if (fh == NULL) {
		fprintf(stderr, "Output file not found.");
		return 1;
	}
	
	if (convertTo = '6') {
		if (writePPM6()) {
			return 1;
		}
	}
	
	fclose(fh);
	free(pixmap);
	
	return returnValue;
}