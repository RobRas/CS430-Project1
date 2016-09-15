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

int getWidthAndHeight() {
	char value[20];
	
	while ((value[0] = fgetc(fh)) == '#') {	// Line is a comment
		while (fgetc(fh) != '\n');	// Ignore the line
	}
	
	if (!isdigit(value[0])) {
		fprintf(stderr, "Image width is not valid.");
		return 1;
	}
	
	int i = 1;
	while ((value[i] = fgetc(fh)) != ' ') {
		if (!isdigit(value[i])) { 
			fprintf(stderr, "Image width is not valid.");
			return 1;
		}
		i++;
	}
	value[i] = '\0';
	width = atoi(value);
	
	i = 0;
	while ((value[i] = fgetc(fh)) != '\n') {
		if (!isdigit(value[i])) {
			fprintf(stderr, "Image width is not valid.");
			return 1;
		}
		i++;
	}
	value[i] = '\0';
	height = atoi(value);
}

int getMaxColorValue() {
	char value[4];
	
	for (int i = 0; i < 4; i++) {
		value[i] = fgetc(fh);
		if (value[i] == '\n') {
			value[i] = '\0';
			break;
		} else if (!isdigit(value[i])) {
			fprintf(stderr, "Value must be a digit.");
			return 1;
		}
	}
	
	maxColorValue = atoi(value);
	
	if (maxColorValue > 255) {
		fprintf(stderr, "Not a PPM file. Max color value too high.\n");
		return 1;
	} else if (maxColorValue < 1) {
		fprintf(stderr, "Not a PPM file. Max color value too low.\n");
		return 1;
	}
	
	return 0;
}

int getP3Value(unsigned char* outValue) {
	unsigned char value[4];
	int rgbValue;
	
	for (int i = 0; i < 4; i++) {
		value[i] = fgetc(fh);
		if (value[i] == '\n') {
			value[i] = '\0';
			break;
		} else if (!isdigit(value[i])) {
			fprintf(stderr, "Value must be a digit.");
			return 1;
		}
	}
	
	rgbValue = atoi(value);
	if (rgbValue > maxColorValue) {
		fprintf(stderr, "Color value exceeding max.");
		return 1;
	}
	
	*outValue = rgbValue;
	return 0;
}

int parseP3() {
	for (int i = 0; i < width * height; i++) {	
		if (getP3Value(&pixmap[i].r)) {
			return 1;
		}
		if (getP3Value(&pixmap[i].g)) {
			return 1;
		}
		if (getP3Value(&pixmap[i].b)) {
			return 1;
		}
	}
	
	return 0;
}

int parseP6() {
	fread(pixmap, sizeof(Pixel), width * height, fh);
	return 0;
}

int loadPPM() {
	if (getWidthAndHeight()) { return 1; }
	pixmap = malloc(sizeof(Pixel) * width * height);
	if (getMaxColorValue()) { return 1; }
	if (convertFrom == '3') {
		if (parseP3()) { return 1; }
	} else if (convertFrom == '6') {
		if (parseP6()) { return 1; }
	}
	return 0;
}

int writeP3() {
	fprintf(fh, "P3\n# Converted with Robert Rasmussen's ppmrw\n%d %d\n%d\n", width, height, maxColorValue);
	for (int i = 0; i < width * height; i++) {
		fprintf(fh, "%d\n%d\n%d\n", pixmap[i].r, pixmap[i].g, pixmap[i].b);
	}
	return 0;
}

int writeP6() {
	fprintf(fh, "P6\n# Converted with Robert Rasmussen's ppmrw\n%d %d\n%d\n", width, height, maxColorValue);
	fwrite(pixmap, sizeof(Pixel), width*height, fh);
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: ppmrw target input output.\n");
		return 1;
	}
	
	if (getConversionType(argv[1])) { return 1; }
	
	fh = fopen(argv[2], "r");
	if (fh == NULL) {
		fprintf(stderr, "Input file not found.");
		return 1;
	}
	
	if (getPPMFileType()) { return 1; }
	
	if (loadPPM()) { return 1; }
	
	fclose(fh);
	
	if (convertTo == '3') {
		fh = fopen(argv[3], "w");
	} else if (convertTo == '6') {
		fh = fopen(argv[3], "wb");
	}
	if (fh == NULL) {
		fprintf(stderr, "Output file not found.");
		return 1;
	}
	
	if (convertTo == '3') {
		if (writeP3()) { return 1; }
	} else if (convertTo = '6') {
		if (writeP6()) { return 1; }
	}
	
	fclose(fh);
	free(pixmap);
	
	return 0;
}