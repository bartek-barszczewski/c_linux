#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char * get_line_alloc(int * plen) {
	if(plen) {
		plen = malloc(sizeof(*plen));
		if(!plen) {
			return NULL;
		}
	}
}


int main() {
	char * line = NULL;
	int len = 255;
	int idx = 0;

	do {
		idx++;
		free(line);
		line = get_line_alloc(&len);	

		if(line) {
			printf("[%d] %s\n", idx, line);
		}else {
			printf("Can not allocate memory\n");
			break;
		}

	}while(len && strcmp(line, "END"));

	return 0;
}
