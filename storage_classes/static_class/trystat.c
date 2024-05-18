#include <stdio.h>
#include <stdlib.h>

void trystat() { 
        int not_statvar = 1;
        static int statvar = 1;

        printf("not_statvar = %d \nstatvar = %d\n\n", not_statvar, statvar);
        statvar += 1;
        not_statvar += 1;
}
 
int main() { 
        printf("Run trystat: \n");
        trystat();
        trystat();
        trystat();
        return 0;
}





