#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include "neuralnetwork.h"
#include "utilities.h"

#define NET_DEPTH 10
#define NETWORKS 8
#define VARIATION 0.5
#define INPUTDATA_SIZE 2
#define DATASET_SIZE 100
#define EXPOUT_SIZE 1
#define RUNS 100

double inputdata[DATASET_SIZE][INPUTDATA_SIZE];
double expout[DATASET_SIZE][EXPOUT_SIZE];

unsigned long NET_OUTPUT_SIZE[NETWORKS];
unsigned long BESTNET_OUTPUT_SIZE;


Network *bestNet;
double  *bestnet_output;
double   bestDelta;

double tmpTopDelta;
unsigned long tmpTopDeltaIndex;

Network *net[NETWORKS];
double *net_output[NETWORKS];
double delta[NETWORKS];
pthread_t net_thread[NETWORKS];


void* networkRun(void * index);

int main(int argc, char ** argv)
{
	srand(time(NULL));
	for(unsigned int i = 0; i < DATASET_SIZE; i++)
	{
		inputdata[i][0] = rand() % 16;
		inputdata[i][1] = rand() % 16;
		expout[i][0] = inputdata[i][0] + inputdata[i][1];
	}

	for(unsigned long i = 0; i < NETWORKS ;i++)
	{
		net[i] = createNetwork(NET_DEPTH, INPUTDATA_SIZE, inputdata[0], &NET_OUTPUT_SIZE[i], &net_output[i], weighted_avg);
	}
	bestNet = createNetwork(NET_DEPTH, INPUTDATA_SIZE, inputdata[0], &BESTNET_OUTPUT_SIZE, &bestnet_output, weighted_avg);
	
	bestDelta = 1e300; // Not very nice, but well, we need an unrealistically big delta to compare against once.
	for(unsigned long i = 0; i < RUNS; i++)
	{
		for(unsigned long j = 0; j < NETWORKS; j++)
		{
			pthread_create(&net_thread[j], NULL, networkRun, (void *) j);
		}
		for(unsigned long j = 0; j < NETWORKS; j++)
		{
			pthread_join(net_thread[j], NULL);
		}
		tmpTopDelta = bestDelta;
		tmpTopDeltaIndex = -1;
		for(unsigned long j = 0; j < NETWORKS; j++)
		{
			if(delta[j] < tmpTopDelta)
			{
				tmpTopDeltaIndex = j;
				tmpTopDelta = delta[j];
			}
		}
		if(tmpTopDeltaIndex != -1)
		{
			cloneNetwork(bestNet, net[tmpTopDeltaIndex]);
			bestDelta = tmpTopDelta;
			printf("GEN(%lu): delta = %.4g\n", i, bestDelta);
		}
	}
	for(unsigned long j = 0; j < DATASET_SIZE; j++)
	{
		for(unsigned long k = 0; k < bestNet->layer[0].NODE_SIZE; k++)
		{
			bestNet->layer[0].node[k].input = inputdata[j];
		}
		runNetwork(bestNet);
		printf("%.4g + %.4g = %.4g (EXP: %.4g, DELTA: %.4g)\n", inputdata[j][0], inputdata[j][1], bestnet_output[0], expout[j][0], fabs(expout[j][0] - bestnet_output[0]));
	}
	for(unsigned long i = 0; i < NETWORKS; i++)
	{
		destroyNetwork(net[i]);
	}
	destroyNetwork(bestNet);
	return 0;
}

void* networkRun(void * index)
{
	unsigned long ind = (unsigned long) index;
	cloneNetwork(net[ind],bestNet);
	varyNetwork_const(net[ind],-VARIATION,VARIATION);
	delta[ind] = 0;
	for(unsigned long j = 0; j < DATASET_SIZE; j++)
	{
		for(unsigned long k = 0; k < net[ind]->layer[0].NODE_SIZE; k++)
		{
			net[ind]->layer[0].node[k].input = inputdata[j];
		}
		runNetwork(net[ind]);
		for(unsigned long k = 0; k < EXPOUT_SIZE && k < net[ind]->layer[net[ind]->NETWORK_DEPTH-1].NODE_SIZE; k++)
		{
			delta[ind] += fabs(expout[j][k] - net_output[ind][k]);
		}
	}

	return NULL;
}
