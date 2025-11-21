#include <stdio.h>
#include <stdarg.h>
#include "mysum.h"

int main(void){
	printf("sum of 1, 2 = %d\n", mysum(2, 2, 3));
	printf("sum 1, 2, 3 = %d\n", mysum(3, 4, 6, 8));

	return 0;
}
