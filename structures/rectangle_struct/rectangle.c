#include <stdio.h>

struct Rectangle {
	unsigned int length;
	unsigned int breadth;
	char x;
};


int main() {

	struct Rectangle rect;
	rect.length = 1000;
	rect.breadth = 1000;

	printf("Sizeof rectangle %lu\n", sizeof(rect) );

	printf("Rectangle length %u \n", rect.length);
	printf("Rectangle breadth %u \n", rect.breadth);

	return 0;
}
