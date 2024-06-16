#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


struct Rectangle {
	int a;
	int b;
	unsigned int result;
};

void init_rectangle(struct Rectangle * rect, int a, int b);
void count_rectangle_area(struct Rectangle * rect);


int main(int argc, char *argv[]) {

	struct Rectangle rect;	
	
	init_rectangle(&rect, 12, 12);
	count_rectangle_area(&rect);

	printf("Rectangle area = %u\n", rect.result);

	return 0;
}


void init_rectangle(struct Rectangle * rect, int a, int b) {
	
	bool IS_NULL_OR_LESS_THEN_ZERO = (bool) (a < 0 || b < 0); 

	if(IS_NULL_OR_LESS_THEN_ZERO){
		rect->a = -1;
		rect->b = -1;
		printf("Error parameters cant be less then zero\n");
		return;
	}

	rect->a = a;
	rect->b = b;
}


void count_rectangle_area(struct Rectangle * rect) {
	bool IS_MINUS_ONE = rect->a == -1 || rect->b == -1;

	if(IS_MINUS_ONE){
		printf("Error a or b is less then zero\n");
		return;
	}

	rect->result = rect->a * rect->b;
}


