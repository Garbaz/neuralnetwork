#include <stdio.h>

#define ND 6

int main(int argc, char **argv)
{
	unsigned long l = argc;


	unsigned long sum2n = (((unsigned long)(-1))<<(sizeof(unsigned long)-l))<<(2*ND-1-l);
	
	
	printf("%lx\n",sum2n);
	return 0;
}
