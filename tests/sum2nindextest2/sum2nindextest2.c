#include <stdio.h>
#include <stdlib.h>
#include "../../src/utilities.h"

int main(int argc, char **argv)
{
	if(argc < 4)
	{
		printf("Usage:\n  test2 LAYER NODE NETWORK_DEPTH\n");
		return 1;
	}
	unsigned long result = SUM_2_N_INDEX(strtol(argv[1],NULL,10),strtol(argv[2],NULL,10),strtol(argv[3],NULL,10));

	printf("%lu = 0x%lx\n", result, result);
}
