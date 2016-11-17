/*
PROJECT: A simple and as univeral as possible neural network implementation
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
 - Implement different training systems, evaluation, etc.
*/


/**
A Node consists of a referece to the input list, a weight list and a result.
*/
typedef struct 
{
	unsigned long INPUT_SIZE; // Size of 'input'
	long **input; // Pointer to input array of long pointers. Must at least be INPUT_SIZE long.
	double * weight;  // Pointer to weight array. Must be at least INPUT_SIZE long.
	long result; // The result after evaluation (Might be unused)
} Node;
void setNode(Node* n, unsigned long INPUT_SIZE, long **input, double * weight, long result)
{
	n->INPUT_SIZE = INPUT_SIZE;
	n->input = input;
	n->weight = weight;
	n->result = result;
}

/**
A Layer consists of NODE_SIZE nodes.
*/
typedef struct
{
	unsigned long NODE_SIZE; // How many nodes are in the layer.
	Node * node; // Pointer to node array
	long* *result; // Pointer to array of pointers to the results of the nodes in this layer.
} Layer;
void setLayer(Layer* l, const unsigned long NODE_SIZE, Node* node, long* *result)
{
	l->NODE_SIZE = NODE_SIZE;
	l->node = node;
	l->result = result;
}

/**
A Network consists of NETWORK_DEPTH Layers.
*/
typedef struct
{
	unsigned long NETWORK_DEPTH;
	Layer * layer;	
} Network;
void setNetwork(Network *n, const unsigned long NETWORK_DEPTH, Layer * layer)
{
	n->NETWORK_DEPTH = NETWORK_DEPTH;
	n->layer = layer;
}

//TODO:Overflow is way too likely!!
void evalNode(Node *n)
{
	for(unsigned long i = 0; i < n->INPUT_SIZE; i++)
	{
		n->result += (long) (n->weight[i] * ((double) *(n->input[i])));
	}
	n->result /= n->INPUT_SIZE;
	//Could return result, might be useful for debugging...
}



/**
To save some memory in big networks, in which the amount of inputs/weights per node halves with each layer down, one can index the weights memory non-linearly. To determine the first index for each layer, one would have to add up how much memory the previous layers use. To get around this, one can use some bitshift trickery to generate the result.


 Layer | Weights of all nodes in layer     | First index in this layer | Last index in this layer
 ------+-----------------------------------+---------------------------+------------------------------------------
    0  | 2^(ND-1) * 2^(ND-1) = 2^(2*ND-2)  | 0                         | 2^(2*ND-2) -1
    1  | 2^(ND-2) * 2^(ND-1) = 2^(2*ND-3)  | 2^(2*ND-2)                | 2^(2*ND-2) + 2^(2*ND-3) -1]
    2  | 2^(ND-3) * 2^(ND-1) = 2^(2*ND-4)  | 2^(2*ND-2) + 2^(2*ND-3)   | 2^(2*ND-2) + 2^(2*ND-3) + 2^(2*ND-4) -1]
etc.

n = node index in layer
l = layer index in network
d = depth of network

Theses sums always add up to a number which, when written in binary, starts with some 0s, then has a series of 1s and ends with a buch of 0s:

                                  2*ND-(l+1)
                                      V
sum(2^(2*ND-k),2<=k<=l+1) = 0b0000011110000000 [Example: l=4, ND=6]
                                 ^
                               2*ND

This results in a total amount of 1s equal to:

(2*ND-1)-(2*ND-l-1) = -2+l+1 = l

To emulate the result of summing up the previous layers, one can use bitshifts:

(-1) = 0b1111111111111111
(-1)>>(BITS-l) = (-1)>>(BITS-l) = 0b0000000000001111  // l ones [Example: l=4]
((-1)>>(BITS-l))<<(2*ND-(l+1))  = 0b0000111100000000  // [Example: l=4, ND=6]
= ((-1)>>(BITS-l))<<(2*ND-1-l) = BINDEX(l,ND)

Given this base index of the layer, getting the 0 index of each node is easy:
BINDEX(n,l,ND) + (ND-l)*n;

This compiles into only a few assembly instructions (Which get executed only once), making it a vast improvement over going through the whole sum.

*/

/*
l = layer index in network
n = node index in layer
d = depth of network
*/
#define SUM_2_N_INDEX(l,n,d) (((((unsigned long)-1)>>(sizeof(unsigned long)-l))<<(2*d-1-l)) + (d-l)*n)

/*
//Adding up the previous layers in inefficient. The bitshift trickery above is way better.
unsigned long sum_2_n_index(unsigned long l, unsigned long n, unsigned long d)
{
	unsigned long result = 0;
	for(unsigned long i = 2; i <= l+1; i++)
	{
		result += 1<<(2*d-i); // Add up how much the above layers take up to get the first index of the layer.
	}
	result += (d-l) * n; // Go to nth node in layer.

	return result;
}
*/



//Dump functions
void dumpNode(Node *n)
{
	printf("  Node {%lx}: INPUT_SIZE=%lu; input*=%lx; weight*=%lx; result=%ld\n", n, n->INPUT_SIZE, n->input, n->weight, n->result);
	printf("  Node {%lx}: input=[", n);
	for(unsigned long i = 0; i < n->INPUT_SIZE; i++)
	{
		printf("%d, ", *n->input[i]);
	}
	printf("]\n");
	printf("  Node {%lx}: weight=[", n);
	for(unsigned long i = 0; i < n->INPUT_SIZE; i++)
	{
		printf("%.4lg, ",n->weight[i]);
	}
	printf("]\n");
}

void dumpLayer(Layer *l)
{
	printf(" Layer {%lx}: NODE_SIZE=%lu; node*=%lx; result*=%lx\n", l, l->NODE_SIZE, l->node, l->result);
}

void dumpNetwork(Network *n)
{
	printf("Network {%lx}: NETWORK_DEPTH=%lu; layer*=%lx\n", n, n->NETWORK_DEPTH, n->layer);
}
