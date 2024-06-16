#include <stdio.h>
#include <stdlib.h>


int main() {
	int * array_integers = (int *) malloc(10 * sizeof(int));
	
	*(array_integers) = 123;
	*(array_integers + 1) = 124;
	*(array_integers + 2) = 125;
	*(array_integers + 3) = 126;
	
	printf("array address: %p\n", array_integers);	
	printf("array[0] = %d\n", *(array_integers));
	printf("array[1] = %d\n", *(array_integers +1));
	printf("array[2] = %d\n", *(array_integers +2));

	free(array_integers);	
	return 0;
}
