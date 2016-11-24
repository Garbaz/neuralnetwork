/*
All kinds of utilities not part of any specific part of the program. 
*/

#include <stdlib.h>

/**
Generates a random double between limInf and limSup.
*/
double randomDouble(double limInf, double limSup)
{
	return limInf + ((double)rand() / RAND_MAX) * (limSup - limInf);
}


/**
The function gets a data array and a weight array. It mulitplies each value in data by the corresponding weight, and calculates the average.
data and weight have to consist of at least DATA_SIZE doubles each.
*/
double weighted_avg(double *data, double *weight, const unsigned long DATA_SIZE)
{
	double ret = 0;
	for(unsigned long i = 0; i < DATA_SIZE; i++)
	{
		ret += weight[i] * data[i];
	}

	ret /= DATA_SIZE;

	return ret;
}
