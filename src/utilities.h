#ifndef UTILITIES_H
#define UTILITIES_H

#include <time.h>
#include <stdio.h>

/**
To save some memory (and/or a lot of malloc calls) when creating (big) networks, in which the amount of inputs/weights per node halves with each layer down, one can index the weights memory non-linearly. To determine the first index for each layer, one would have to add up how much memory the previous layers use.
To get around having to compute this sum, one can use some bitshift trickery instead to generate the result.
An explanation (or something of that sort) about how this works:

(Layer 0 gets ignored here, since it's structure depends on the network input. In practice you'll have to add INPUT_SIZE to the resulting index.)

 Layer | Weights of all nodes in layer     | First index in this layer | Last index in this layer
 ------+-----------------------------------+---------------------------+------------------------------------------
    1  | 2^(ND-1) * 2^(ND-1) = 2^(2*ND-2)  | 0                         | 2^(2*ND-2) -1
    2  | 2^(ND-2) * 2^(ND-1) = 2^(2*ND-3)  | 2^(2*ND-2)                | 2^(2*ND-2) + 2^(2*ND-3) -1
    3  | 2^(ND-3) * 2^(ND-1) = 2^(2*ND-4)  | 2^(2*ND-2) + 2^(2*ND-3)   | 2^(2*ND-2) + 2^(2*ND-3) + 2^(2*ND-4) -1
etc.

| n = node index in layer
| l = layer index in network
| d = depth of network

These sums always add up to a number which, when written in binary, starts with some 0s, then has a series of 1s and ends with a buch of 0s:

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
| l = layer index in network
| n = node index in layer
| d = depth of network
*/
#define SUM_2_N_INDEX(l,n,d) (((((unsigned long)-1)>>(sizeof(unsigned long)*8-l))<<(2*d-1-l)) + (d-l)*n)

unsigned long tmpBenchTicks;
#define BENCH(cmd) tmpBenchTicks=clock();cmd;tmpBenchTicks=clock()-tmpBenchTicks;fprintf(stderr, "BENCH: '"#cmd"' took %lu ticks. That's %.4g secs.\n", tmpBenchTicks, ((double)tmpBenchTicks)/((double)CLOCKS_PER_SEC));
#define BENCH_START tmpBenchTicks=clock();
#define BENCH_END(text) tmpBenchTicks=clock()-tmpBenchTicks;fprintf(stderr, "BENCH: \""#text"\" took %lu ticks. That's %.4g secs.\n", tmpBenchTicks, ((double)tmpBenchTicks)/((double)CLOCKS_PER_SEC));

/**
Generates a random double between limInf and limSup.
*/
double randomDouble(double limInf, double limSup);

/**
The function gets a data array and a weight array. It mulitplies each value in data by the corresponding weight, and calculates the average of all data points.
data and weight have to consist of at least DATA_SIZE doubles each.
*/
double weighted_avg(double *data, double *weight, const unsigned long DATA_SIZE);

#endif
