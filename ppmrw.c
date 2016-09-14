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

int getP3Value(unsigned char* outValue) {
	char value[4];
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

int parseP3(int index) {
	if (getP3Value(&pixmap[index].r)) {
		return 1;
	}
	if (getP3Value(&pixmap[index].g)) {
		return 1;
	}
	if (getP3Value(&pixmap[index].b)) {
		return 1;
	}
	
	return 0;
}

int parseP6() {
	fread(pixmap, sizeof(Pixel), width * height, fh);
	return 0;
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

int loadPPM() {
	char* widthAndHeight;
	widthAndHeight = parseWidthAndHeight();
	if (getWidthAndHeight(widthAndHeight)) { return 1; }
	pixmap = malloc(sizeof(Pixel) * width * height);
	if (getMaxColorValue()) { return 1; }
	if (convertFrom == '3') {
		for (int i = 0; i < width * height; i++) {
			if (parseP3(i)) { return 1; }
		}
	} else {
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

int main(int argc, char* argv[]) {
	int returnValue = 0;
	
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
	
	fh = fopen(argv[3], "w");
	if (fh == NULL) {
		fprintf(stderr, "Output file not found.");
		return 1;
	}
	
	if (convertTo == '3') {
		if (writeP3()) {
			return 1;
		}
	} else if (convertTo = '6') {
		if (writeP6()) {
			return 1;
		}
	}
	
	fclose(fh);
	free(pixmap);
	
	return returnValue;
}