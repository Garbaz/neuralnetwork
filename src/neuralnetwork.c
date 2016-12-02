/*
PROJECT: A simple and efficient neural network implementation
AUTHOR: Garbaz
EMAIL: garbaz@t-online.de
LICENCE:
	The MIT License (MIT)
	
	Copyright (c) 2016 
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

/*
TODO:
 - Implement different training systems
 - Implement backpropagating variation
 - Move some comment to doc
*/

#include <stdlib.h>
#include "debug.h"
#include "utilities.h"
#include "neuralnetwork.h"

#define DEBUG 0 // Enables/Disables debug printing


//NODE

void setNode(Node* n, const unsigned long INPUT_SIZE, double *input, double * weight, double * result)
{
	n->INPUT_SIZE = INPUT_SIZE;
	n->input = input;
	n->weight = weight;
	n->result = result;
}

/**
Allocates memory for a node and initializes it.
Returns pointer to node.
DON'T FORGET TO CALL "destroyNode"!
*/
Node* createNode(const unsigned long INPUT_SIZE, double *input, double * result)
{
	Node* ret=        malloc(sizeof(Node));
	double * weight = malloc(sizeof(double) * INPUT_SIZE);
	setNode(ret, INPUT_SIZE, input, weight, result);
	return ret;
}
/**
Frees everything associated with the node and the node itself.
*/
void destroyNode(Node* n)
{
	if(n != NULL)
	{
		if(n->weight != NULL) free(n->weight);
		free(n);
	}
}

/**
Clones the node src into dest (Both have to be fully allocated and initialized!).
THIS WILL NOT COPY EVERYTHING, only the "relevant" parts.
Look at documentation for more details.
*/
void cloneNode(Node *dest, const Node *src)
{
	for(unsigned long i = 0; i < dest->INPUT_SIZE && i < src->INPUT_SIZE; i++)
	{
		dest->weight[i] = src->weight[i];
	}
	*(dest->result) = *(src->result);
}

//LAYER

void setLayer(Layer* l, const unsigned long NODE_SIZE, Node* node, double *result)
{
	l->NODE_SIZE = NODE_SIZE;
	l->node = node;
	l->result = result;
}
/**
Allocates memory for a layer and its nodes and initializes everything.
DON'T FORGET TO CALL "destroyLayer"!
*/
Layer* createLayer(const unsigned long NODE_SIZE, const unsigned long INPUT_SIZE, double *input)
{
	Layer* ret=(Layer *) malloc(sizeof(Layer));
	
	// Allocate and initialize the nodes. Calling "createNode" would be cleaner, but very inefficient and the nodes wouldn't be continuous in memory, making acessing more difficult.
	Node * node = malloc(sizeof(Node) * NODE_SIZE);
	double * weight = malloc(sizeof(double) * NODE_SIZE * INPUT_SIZE);
	double * result = malloc(sizeof(double) * NODE_SIZE);
	for(unsigned long i = 0; i < NODE_SIZE; i++)
	{
		setNode(&node[i], INPUT_SIZE, input, weight, &result[i]);
	}

	setLayer(ret, NODE_SIZE, node, result);
	return ret;
}
/**
Frees everything associated with the layer and the layer itself.
!!! This function should ONLY be used when the layer has been created using the "createLayer" function. !!!
(Otherwise we don't know how the memory was allocated and wouldn't know what we have to free.)
If the nodes were created manually using "createNode", call "destroyNode" for each of them.
*/
void destroyLayer(Layer * l)
{
	if(l != NULL)
	{
		if(l->node != NULL)
		{
			//This depends on the weights being continuous memory!
			if((l->node)->weight != NULL)
			{
				free((l->node)->weight);
			}
			free(l->result);
			free(l->node);
		}
		free(l);
	}
}

/**
Clones the layer src into dest (Both have to be fully allocated and initialized!).
THIS WILL NOT COPY EVERYTHING, only the "relevant" parts.
Look at documentation for more details.
*/
void cloneLayer(Layer *dest, const Layer *src)
{
	for(unsigned long i = 0; i < dest->NODE_SIZE && i < src->NODE_SIZE; i++)
	{
		cloneNode(&dest->node[i], &src->node[i]);
	}
}


//NETWORK

void setNetwork(Network *w, const unsigned long NETWORK_DEPTH, Layer * layer, double (*func)(double *, double *, const unsigned long))
{
	w->NETWORK_DEPTH = NETWORK_DEPTH;
	w->layer = layer;
	w->func = func;
}
/**
Allocates memory for a network, it's layers and their nodes and initializes everything.
The network will have ND (=NETWORK_DEPTH) layers with 2^(ND-1) nodes each.
DON'T FORGET TO CALL "destroyNetwork"!
*/
Network * createNetwork(const unsigned long NETWORK_DEPTH, const unsigned long INPUT_SIZE, double *input, unsigned long * OUTPUT_SIZE, double **output, double (*func)(double *, double *, const unsigned long))
{
	DBGPRNT("createNetwork(%lu, %ld, %lX):\n", NETWORK_DEPTH, INPUT_SIZE, input);
	const unsigned long NPL = 1<<(NETWORK_DEPTH-1);
	DBGPRNT("  Mallocing...\n");
	Network * ret =   malloc(sizeof(Network)); // One Network
//	DBGPRNT("    ret =    0x%lx\n", ret);
	Layer * layer =   malloc(sizeof(Layer) * NETWORK_DEPTH); // ND (=NETWORK_DEPTH) layers
//	DBGPRNT("    layer =  0x%lx\n", layer);
	Node * node   =   malloc(sizeof(Node) * NETWORK_DEPTH * NPL); // 2^(ND-1) nodes 
//	DBGPRNT("    node =   0x%lx\n", node);
	double * result = malloc(sizeof(double) * NETWORK_DEPTH * NPL); // One result per node
//	DBGPRNT("    result = 0x%lx\n", result);
	double * weight = malloc(sizeof(double) * NPL * (INPUT_SIZE + ((1<<NETWORK_DEPTH)-2))); // First layer needs INPUT_SIZE weights. All subsequent layers need 2^(ND-k) weights (See documentation and comment to SUM_2_N_INDEX for more details)
//	DBGPRNT("    weight = 0x%lx\n", weight);
	DBGPRNT("  Setting up Network...\n");
	setNetwork(ret, NETWORK_DEPTH, layer, func);
	
	DBGPRNT("  Setting up Layers...\n");
	//Special case: Layer 0, gets "input" as input instead of previous results.
	setLayer(&layer[0], NPL, &node[0], &result[0]);
	for(unsigned long j = 0; j < NPL; j++)
	{
		setNode(&node[j], INPUT_SIZE, input, &weight[j], &result[j]);
		result[j] = 0;
	}
	for(unsigned long i = 1; i < NETWORK_DEPTH; i++)
	{
		setLayer(&layer[i], NPL, &node[NPL * i], &result[NPL * i]);
		for(unsigned long j = 0; j < NPL; j++)
		{
			/* Setting up the nodes is quite ugly, the input pointer is just the results of some of the nodes in the above layer.
			 * Where the weights are isn't relevant, but calling malloc for each node would be slow and thereby undesirable.
			 * To find where the weight array of each node lays in the continuous block of memory we allocated above, we use SUM_2_N_INDEX (For details, see comment to SUM_2_N_INDEX).*/
//			DBGPRNT("SUM_2_N_INDEX(%lu, %lu, %lu) = %lu\n", i, j, NETWORK_DEPTH, SUM_2_N_INDEX(i,j,NETWORK_DEPTH));
			setNode(&node[NPL * i + j], 1<<(NETWORK_DEPTH-i), &result[NPL * (i-1) + (1<<(NETWORK_DEPTH-i))*(j/(1<<(NETWORK_DEPTH-i)))], &weight[SUM_2_N_INDEX(i,j,NETWORK_DEPTH)], &result[i * NPL + j]);
			result[j] = 0;
		}
	}
	if(output != NULL) *output = &result[(NETWORK_DEPTH-1)*NPL];
	DBGPRNT("  DONE\n");
	return ret;
}
/**
Frees everything associated with the network and the network itself.

!!! This function should ONLY be used when the layer has been created using the "createNetwork" function. !!!

(Otherwise we don't know how the memory was allocated and wouldn't know what we have to free.)
If the Layers were created manually using "createLayer", call "destroyLayer" for each of them instead.
*/
void destroyNetwork(Network *w)
{
	DBGPRNT("destroyNetwork(0x%lx):\n", w);
	if(w != NULL)
	{
		DBGPRNT("  w->layer = 0x%lx\n", w->layer);
		if(w->layer != NULL)
		{
			DBGPRNT("  w->layer->node = 0x%lx\n", w->layer->node);
			//This depends on the nodes being continuous memory!
			if(w->layer->node != NULL)
			{
				DBGPRNT("  w->layer->node->weight = 0x%lx\n", w->layer->node->weight);
				//This depends on the weights being continuous memory!
				if(w->layer->node->weight != NULL)
				{
					free(w->layer->node->weight);
				}
				free(w->layer->node);
			}
			DBGPRNT("  w->layer->result = 0x%lx\n", w->layer->result);
			//This depends on the weights being continuous memory!
			if(w->layer->result != NULL)
			{
				free(w->layer->result);
			}
			free(w->layer);
		}
		free(w);
	}
	DBGPRNT("   DONE\n");
}

/**
Clones the network src into dest (Both have to be fully allocated and initialized!).
THIS WILL NOT COPY EVERYTHING, only the "relevant" parts.
Look at documentation for more details.
*/
void cloneNetwork(Network *dest, const Network *src)
{
	for(unsigned long i = 0; i < dest->NETWORK_DEPTH && i < src->NETWORK_DEPTH; i++)
	{
		cloneLayer(&dest->layer[i], &src->layer[i]);
	}
}


/**
Runs a given network with its function.
Goes through each layer and evaluates the all of its nodes, pushing input through the function, storing the result.
*/
void runNetwork(Network *w)
{
	for(unsigned long i = 0; i < w->NETWORK_DEPTH; i++)
	{
		for(unsigned long j = 0; j < w->layer[i].NODE_SIZE; j++)
		{
			*(w->layer[i].node[j].result) = w->func(w->layer[i].node[j].input, w->layer[i].node[j].weight,  w->layer[i].node[j].INPUT_SIZE);
		}
	}
}

/**
Goes through a given network and increases the weights of all nodes randomly by a number in the mathematical range [limInf, limSup].
*/
void varyNetwork_const(Network *w, double limInf, double limSup)
{
	for(unsigned long i = 0; i < w->NETWORK_DEPTH; i++)
	{
		for(unsigned long j = 0; j < w->layer[i].NODE_SIZE; j++)
		{
			for(unsigned long k = 0; k < w->layer[i].node[j].INPUT_SIZE; k++)
			{
				w->layer[i].node[j].weight[k] += randomDouble(limInf, limSup);
			}
		}
	}
}

void varyNetwork_backprop(Network *w, double *expout, const unsigned long EXPOUT_SIZE, double limInf, double limSup)
{
	
}

void trainNetwork(Network *w)
{
	
}

/**
Evaluates a given node with a given function.
The function has to be of the form "double function(double * data, double * weight, const unsigned long DATA_SIZE)". Any further details (Including the naming of the parameters) about the function don't matter.
*/
void evalNode(Node *n, double (*f)(double *, double *, const unsigned long))
{
	*(n->result) = f(n->input, n->weight, n->INPUT_SIZE);
	//Could return result, might be useful for debugging...
}



//DUMP FUNCTIONS


void dumpNode(Node *n)
{
	printf("| +-Node {0x%16lx}: INPUT_SIZE=%lu; input*=0x%lx; weight*=0x%lx; result=%.6g\n", n, n->INPUT_SIZE, n->input, n->weight, *(n->result));
	printf("| |                          input=[", n);
	for(unsigned long i = 0; i < n->INPUT_SIZE; i++)
	{
		printf("%.6g, ", n->input[i]);
	}
	printf("]\n");
	printf("| |                          weight=[", n);
	for(unsigned long i = 0; i < n->INPUT_SIZE; i++)
	{
		printf("%.6g, ",n->weight[i]);
	}
	printf("]\n");
}

void dumpLayer(Layer *l)
{
	printf("+-+Layer {0x%16lx}: NODE_SIZE=%lu; node*=0x%lx; result*=0x%lx\n", l, l->NODE_SIZE, l->node, l->result);
	for(unsigned long i = 0; i < l->NODE_SIZE; i++)
	{
		dumpNode(&l->node[i]);
	}
	printf("|  `\n");
}

void dumpNetwork(Network *w)
{
	printf("+Network {0x%16lx}: NETWORK_DEPTH=%lu; layer*=0x%lx; func*=0x%lx()\n", w, w->NETWORK_DEPTH, w->layer, w->func);
	for(unsigned long i = 0; i < w->NETWORK_DEPTH; i++)
	{
		dumpLayer(&w->layer[i]);
	}
	printf(" `\n");
}

void dumpNetworkResult(Network *w)
{
	printf("[");
	for(unsigned long i = 0; i < w->layer[w->NETWORK_DEPTH-1].NODE_SIZE; i++)
	{
		printf("%.6g, ", w->layer[w->NETWORK_DEPTH-1].result[i]);
	}
	printf("]\n");
}
