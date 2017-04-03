#include<stdio.h>
#include<stdlib.h>

long asmadd(long a, long b);

int main(int argc, char** argv) {
	if(argc < 3) {
		fprintf(stderr, "usage: add INT_A INT_B\n");
		return 1;
	}
	long c = asmadd(atol(argv[1]), atol(argv[2]));
	printf("%ld\n", c);
	return 0;
}