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
	int layers[4] = { 2, 2, 10, 1 };

	RMGS* rnn = new RMGS(4, layers);

	rnn->Train("ApproxData2.txt", 10);

	double* input = MakeVector(2,0);
	double* outputs = MakeVector(1, 0);


	input[0] = 6;
	input[1] = 2;

	rnn->SetInputSignal(input);

	rnn->PropagateSignal();

	rnn->GetOutputSignal(outputs);

	printf("\nOUT : %f ", outputs[0]);

	while (true)
	{

	}
	return 0;
}


