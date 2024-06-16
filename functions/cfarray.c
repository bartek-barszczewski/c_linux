#include <stdio.h>
#include <stdlib.h>

int * createIntArray(size_t);
void initIntArray(int * ptr, size_t, unsigned int);


int main() {
	size_t size = 10;
	int * int_array = NULL;
	int_array = createIntArray(size);	
	if(int_array != NULL) {
		printf("Int array created, addres= %p \n", int_array);
		initIntArray(int_array, size, 2);	

		for(int i = 0; i < size; i++) {
			printf("Element [%d] = %d\n", i, *(int_array +i));
		}

		free(int_array);
	}
	return 0;
}

int * createIntArray(size_t size) {
	int *ptr =  (int*) malloc(size * sizeof(int));
	return ptr;
}

void initIntArray(int * ptr, size_t size, unsigned int step) {
	for(size_t i = 0; i < size; i++) {
		if( i % step == 0) {
			*ptr = i;
		}else {
			*ptr = 0;
		}

		ptr++;
	}
}
