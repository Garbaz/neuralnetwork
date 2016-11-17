#include <stdio.h>
#include "neuralnetwork.h"

#define NET_DEPTH 3 // Refered to as ND in comments
#define NODES_PER_LAYER (1<<(NET_DEPTH-1))
#define INPUTDATA_SIZE 4

Network net; // One network

Layer lay[NET_DEPTH]; // ND layers
long* res[NET_DEPTH][NODES_PER_LAYER]; // Each layer has pointers to the results of its nodes

Node  nod[NET_DEPTH][NODES_PER_LAYER]; // 2^(ND-1) nodes per layer
double wei[NET_DEPTH][NODES_PER_LAYER][(1<<NET_DEPTH) -1]; // 2^0 + 2^1 + 2^2 + ... 2^ND = 2^(ND+1) -1 weights due to the amount of weights halving with every layer down

long inputdata[INPUTDATA_SIZE];  // Input data for the first layer
long* inputdataptr[INPUTDATA_SIZE]; // Pointers to input data for the first layer
int main(int argc, char ** argv)
{
	for(unsigned long i = 0; i < INPUTDATA_SIZE; i++)
	{
		inputdata[i] = 0;
		inputdataptr[i] = &inputdata[i];
	}
	//Set up first layer of nodes, gets inputdata as input
	for(unsigned long j = 0; j < NODES_PER_LAYER; j++)
	{
		setNode(&nod[0][j], INPUTDATA_SIZE, inputdataptr, &wei[0][j][SUM_2_N_INDEX(0,j,NET_DEPTH)], j);
	}
	for(unsigned long i = 0; i < INPUTDATA_SIZE; i++)
	{
		printf("%d\n", *inputdataptr[i]);
		printf("%lx\n", inputdataptr[i]);
	}
	
	//Set up all subsequent nodes, get results of layer above as input
	for(unsigned long i = 1; i < NET_DEPTH; i++)
	{
		for(unsigned long j = 0; j < (1<<NET_DEPTH); j++)
		{
			setNode(&nod[i][j], 1<<(NET_DEPTH-i), &res[i-1][(1<<(NET_DEPTH-i))*(i/(1<<(NET_DEPTH-i)))], &wei[i][j][SUM_2_N_INDEX(i,j,NET_DEPTH)],i+j);
		}
	}
	
	//Set up layers and result array
	for(unsigned int i = 0; i < NET_DEPTH; i++)
	{
		setLayer(&lay[i], NODES_PER_LAYER, nod[i], res[i]);
		for(unsigned int j = 0; j < NODES_PER_LAYER; j++)
		{
			res[i][j] = &(nod[i][j].result);
		}
	}

	setNetwork(&net, NET_DEPTH, lay);

	dumpNetwork(&net);
	for(unsigned long i = 0; i < NET_DEPTH; i++)
	{
		dumpLayer(&lay[i]);
		for(unsigned long j= 0; j < NODES_PER_LAYER; j++)
		{
			dumpNode(&nod[i][j]);
		}
	}

	return 0;
}
