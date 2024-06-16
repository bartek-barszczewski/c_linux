#include <stdio.h>
#include <stdlib.h>

struct Rectangle {
	int x;
	int y;
	int z;
	int h;
};

int main() {

	char *chp;
	short *shp;
	int *ip;

	printf("SIZEOF TYPE POINTERS\n");
	printf("char %lu \n", sizeof(chp));
	printf("short %lu \n", sizeof(shp));
	printf("int %lu \n", sizeof(ip));

	return 0;
}
