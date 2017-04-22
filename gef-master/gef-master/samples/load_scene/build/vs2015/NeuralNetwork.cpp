#include "NeuralNetwork.h"
#include <time.h>

// set the input signal of the network
void Network::SetInputSignal(double* input)
{
	int i;
	for (i = 0; i < layers[0].num_Neurons; i++)
	{
		layers[0].neurons[i].output = input[i];
	}
}

// get the output of the network
void Network::GetOutputSignal(double* output)
{
	int i;
	if (output)
	{
		for (i = 0; i < layers[num_layers - 1].num_Neurons; i++)
		{
			output[i] = layers[num_layers - 1].neurons[i].output;
		}
	}
	
}

// save the weights and biases of the network
void Network::SaveWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				layers[i].neurons[j].saved_weight[k] = layers[i].neurons[j].weight[k];
				
			}
				
			layers[i].neurons[j].savedBias = layers[i].neurons[j].bias;
		}
	}
}

// restore the saved weights to the current weights
void Network::RestoreWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				layers[i].neurons[j].weight[k] = layers[i].neurons[j].saved_weight[k];
			}
			layers[i].neurons[j].bias = layers[i].neurons[j].savedBias;
				
		}
	}
}

// read the next number in the file
bool Network::read_number(FILE* fp, double* number)
{
	char szWord[256];
	int i = 0;
	int b;

	*number = 0;

	szWord[0] = '\0';
	while (((b = fgetc(fp)) != EOF) && (i < 255))
	{
		if ((b == '.') ||
			(b == '0') ||
			(b == '1') ||
			(b == '2') ||
			(b == '3') ||
			(b == '4') ||
			(b == '5') ||
			(b == '6') ||
			(b == '7') ||
			(b == '8') ||
			(b == '9') ||
			(b == '-'))
		{
			szWord[i++] = (char)b;
		}
		else
			if (i > 0) break;
	}
	szWord[i] = '\0';

	if (i == 0) return false;

	*number = atof(szWord);

	return true;
}

// initialise randoms
void Network::InititaliseRandoms(int rnd)
{
	srand((unsigned)time(NULL));
}

// get a random int
int Network::RandomEqualINT(int Low, int High)
{
	return rand() % (High - Low + 1) + Low;
}

// get a random double
double Network::RandomEqualREAL(double Low, double High)
{
	double randomnum = ((double)rand() / RAND_MAX) * (High - Low) + Low;
	return randomnum;
}

// make a matrix of size r,c and set values to v
double** Network::MakeMatrix(int rows, int cols, double v) // helper for ctor, Train
{
	double** result = 0;
	result = new double*[rows];

	for (int r = 0; r < rows; r++)
	{
		result[r] = new double[cols];

		for (int c = 0; c < cols; c++)
		{
			result[r][c] = v;
		}
	}
	return result;
}

// make a vector of size l and set the values to v
double* Network::MakeVector(int len, double v) // helper for Train
{
	double* result = new double[len];
	for (int i = 0; i < len; ++i)
		result[i] = v;
	return result;
}

// difference between vectors
double* Network::MinusVectors(double* Vec1, double* Vec2, int size)
{
	double* ret = MakeVector(size, 0);
	for (int i = 0; i < size; i++)
	{
		ret[i] = (Vec1[i] - Vec2[i]);
	}
	return ret;
}

// dot product calculation
double Network::DotProduct(double* Vec1, double* Vec2, int size)
{
	double ret = 0;
	for (int i = 0; i < size; i++)
	{
		double multi = (Vec1[i] * Vec2[i]);
		ret += multi;
	}
	return ret;
}

// multiply vectors
double* Network::MultiplyVector(double * Vec, double value, int size)
{
	double* ret = MakeVector(size, 0);

	for (int i = 0; i < size; i++)
	{
		ret[i] = Vec[i] * value;
	}
	return ret;
}
