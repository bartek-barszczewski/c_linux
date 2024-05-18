#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int main() {
	unsigned int MAX_FILENAME = 255;
	int ch = 0;
	int scanf_result = 0;
	char file_name[MAX_FILENAME];
	FILE * fp = NULL;
	
	printf("Enter the name of the file: ");

	if( !(scanf_result = scanf("%255s", file_name) ) ) {
		printf("Error scanf I/O\n");
		return 1;
	}

	bool isNotCorrectLength = !(strlen(file_name) < MAX_FILENAME);

	if( isNotCorrectLength ) {
		printf("Error file name is too long!\n");
		return 2;
	}

	fp = fopen(file_name, "r");
	bool isNotOpenFile = !fp;
	
	if( isNotOpenFile ) {
		printf("Error file is not open\n");
		return 3;
	} 

	printf("================\n");

	while((ch = getc(fp)) != EOF)
		putchar(ch);

	printf("================\n");
	
	return 0;	
}
