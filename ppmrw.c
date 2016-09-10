#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: ppmrw target input output");
		return 1;
	}
	
	printf("%s ", argv[1]);
	printf("%s ", argv[2]);
	printf("%s ", argv[3]);
	
	return 0;
}