#include <stdio.h>
#include <math.h>
#include "NeuralNetwork.h"
#include "RMGS.h"

double** MakeMatrix(int rows, int cols, double v) // helper for ctor, Train
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

double* MakeVector(int len, double v) // helper for Train
{
	double* result = new double[len];
	for (int i = 0; i < len; ++i)
		result[i] = v;
	return result;
}

int main()
{
	int layers[4] = { 1, 4, 1 };

	RMGS* rnn = new RMGS(3, layers);

	rnn->Train("ApproxData.txt", 21);
	//rnn->Train("traindat26.txt", 26620);

	double* input = MakeVector(1,0);
	double* outputs = MakeVector(1, 0);
 
	input[0] = 0.3;
	//input[1] = 0.5;
	//input[2] = 0.5;
	//input[3] = 0.5;


	rnn->SetInputSignal(input);

	rnn->PropagateSignal();

	rnn->GetOutputSignal(outputs);

	//printf("\nOUT 1: %f     2: %f      3: %f      4: %f", outputs[0], outputs[1], outputs[2], outputs[3]);
	printf("\nOUT 1: %f ", outputs[0]);

	while (true)
	{

	}
	return 0;
}


