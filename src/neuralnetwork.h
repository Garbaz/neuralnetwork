#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

/**
A Node consists of a referece to the input list, a weight list and a result.
*/
typedef struct 
{
	unsigned long INPUT_SIZE; // Size of 'input'
	double *input; // Pointer to input array. Must at least be INPUT_SIZE long.
	double *weight;  // Pointer to weight array. Must be at least INPUT_SIZE long.
	double *result; // Pointer to the result after evaluation
} Node;
void setNode(Node* n, const unsigned long INPUT_SIZE, double *input, double * weight, double * result);

/**
Allocates memory for a node and initializes it.
Returns pointer to node.
DON'T FORGET TO CALL "destroyNode"!
*/
Node* createNode(const unsigned long INPUT_SIZE, double *input, double * result);

/**
Frees everything associated with the node and the node itself.
*/
void destroyNode(Node* n);

/**
Clones the node src into dest (Both have to be fully allocated and initialized!).
Look at documentation or implementation in source code for details.
*/
void cloneNode(Node *dest, const Node *src);

/**
A Layer consists of NODE_SIZE nodes.
*/
typedef struct
{
	unsigned long NODE_SIZE; // How many nodes are in the layer.
	Node * node; // Pointer to node array
	double *result; // Pointer to array of results of the nodes in this layer.
} Layer;
void setLayer(Layer* l, const unsigned long NODE_SIZE, Node* node, double *result);

/**
Allocates memory for a layer and its nodes and initializes everything.
DON'T FORGET TO CALL "destroyLayer"!
*/
Layer* createLayer(const unsigned long NODE_SIZE, const unsigned long INPUT_SIZE, double *input);

/**
Frees everything associated with the layer and the layer itself.
!!! This function should ONLY be used when the layer has been created using the "createLayer" function. !!!
(Otherwise we don't know how the memory was allocated and wouldn't know what we have to free.)
If the nodes were created manually using "createNode", call "destroyNode" for each of them.
*/
void destroyLayer(Layer * l);

/**
Clones the layer src into dest (Both have to be fully allocated and initialized!).
Look at documentation or implementation in source code for details.
*/
void cloneLayer(Layer *dest, const Layer *src);

/**
A Network consists of NETWORK_DEPTH Layers.
*/
typedef struct
{
	unsigned long NETWORK_DEPTH;
	Layer * layer;
	double (*func)(double *, double *, const unsigned long);
} Network;
void setNetwork(Network *w, const unsigned long NETWORK_DEPTH, Layer * layer, double (*func)(double *, double *, const unsigned long));

/**
Allocates memory for a network, it's layers and their nodes and initializes everything.
DON'T FORGET TO CALL "destroyNetwork"!


*/
Network * createNetwork(const unsigned long NETWORK_DEPTH, const unsigned long INPUT_SIZE, double *input, unsigned long *OUTPUT_SIZE, double ** output, double (*func)(double *, double *, const unsigned long));

/**
Frees everything associated with the network and the network itself.

!!! This function should ONLY be used when the layer has been created using the "createNetwork" function. !!!

(Otherwise we don't know how the memory was allocated and wouldn't know what we have to free.)
If the Layers were created manually using "createLayer", call "destroyLayer" for each of them instead.
*/
void destroyNetwork(Network *w);

/**
Clones the network src into dest (Both have to be fully allocated and initialized!).
Look at documentation or implementation in source code for details.
*/
void cloneNetwork(Network *dest, const Network *src);

/**
Runs a given network with its function.
Goes through each layer and evaluates the all of its nodes, pushing input through the function, storing the result.
*/
void runNetwork(Network *w);

/**
Goes through a given network and increases the weights of all nodes randomly by a number in the mathematical range [limInf, limSup].
*/
void varyNetwork_const(Network *w, double limInf, double limSup);

void varyNetwork_backprop(Network *w, double *expout, const unsigned long EXPOUT_SIZE, double limInf, double limSup);


void trainNetwork(Network *w);

/**
Evaluates a given node with a given function.
The function has to be of the form "double function(double * data, double * weight, const unsigned long DATA_SIZE)". Any further details (Including the naming of the parameters) about the function don't matter.
*/
void evalNode(Node *n, double (*f)(double *, double *, const unsigned long));

void dumpNode(Node *n);
void dumpLayer(Layer *l);
void dumpNetwork(Network *w);
void dumpNetworkResult(Network *w);

#endif
