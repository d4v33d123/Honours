#include "Network.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

void InititaliseRandoms()
{
	// srand((usinged)time(NULL));
	srand(4711);
}

int RandomEqualINT(int Low, int High)
{
	return rand() % (High - Low + 1) + Low;
}

double RandomEqualREAL(double Low, double High)
{
	return((double)rand() / RAND_MAX) * (High - Low) + Low;
}



Network::Network(int nl, int npl[]) :
num_layers(0),
layers(0),
dMSE(0),
dMAE(0),
dEta(0.25),
dAlpha(0.9),
dGain(1.0),
dAvgTestError(0.0)
{
	int i, j;

	num_layers = nl;
	layers = new Layer[nl];

	for (i = 0; i < nl; i++)
	{
		layers[i].num_Neurons = npl[i];
		layers[i].neurons = new Neuron[npl[i]];

		for (j = 0; j < npl[i]; j++)
		{
			layers[i].neurons[j].output = 1.0;
			layers[i].neurons[j].error = 0.0;

			if (i > 0)
			{
				layers[i].neurons[j].weight = new double[npl[i - 1]];
				layers[i].neurons[j].pre_Weight = new double[npl[i - 1]];
				layers[i].neurons[j].saved_weight = new double[npl[i - 1]];

			}
			else
			{
				layers[i].neurons[j].weight = NULL;
				layers[i].neurons[j].pre_Weight = NULL;
				layers[i].neurons[j].saved_weight = NULL;
			}
		}
	}

}


Network::~Network()
{
	int i, j;

	for (i = 0; i < num_layers; i++)
	{
		if (layers[i].neurons)
		{
			for (j = 0; j < layers[i].num_Neurons; j++)
			{
				if (layers[i].neurons[j].weight)
					delete layers[i].neurons[j].weight;
				if (layers[i].neurons[j].pre_Weight)
					delete layers[i].neurons[j].pre_Weight;
				if (layers[i].neurons[j].saved_weight)
					delete layers[i].neurons[j].saved_weight;
			}
		}
		delete[] layers[i].neurons;
	}
	delete[] layers;
}

void Network::RandomWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				layers[i].neurons[j].weight[k] = RandomEqualREAL(-0.5, 0.5);
				layers[i].neurons[j].pre_Weight[k] = 0.0;
				layers[i].neurons[j].saved_weight[k] = 0.0;
			}
			
		}
	}
}

void Network::SetInputSignal(double* input)
{
	int i;
	for (i = 0; i < layers[0].num_Neurons; i++)
	{
		layers[0].neurons[i].output = input[i];
	}
}

void Network::GetOutputSignal(double* output)
{
	int i;
	for (i = 0; i < layers[num_layers - 1].num_Neurons; i++)
	{
		output[i] = layers[num_layers - 1].neurons[i].output;
	}
}

void Network::SaveWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			layers[i].neurons[j].saved_weight[k] = layers[i].neurons[j].weight[k];
		}
	}
}

void Network::RestoreWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
				layers[i].neurons[j].weight[k] = layers[i].neurons[j].saved_weight[k];
		}
	}
}

void Network::PropagateSignal()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			double sum = 0;
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				double output = layers[i - 1].neurons[k].output;
				double weight = layers[i].neurons[j].weight[k];
				sum += weight*output;
			}
			// activation funciton
			layers[i].neurons[j].output = 1.0 / (1.0 + exp(-dGain * sum));
		}
	}
}

void Network::ComputeOutputError(double* target)
{
	int i;
	dMAE = 0;
	dMSE = 0;
	for (i = 0; i < layers[num_layers - 1].num_Neurons; i++)
	{
		double output = layers[num_layers - 1].neurons[i].output;
		double delta = target[i] - output;
		layers[num_layers - 1].neurons[i].error = dGain * output * (1.0 - output) * delta;
		dMSE += (delta*delta);
		dMAE += fabs(delta);
	}
	// error quadrative 
	dMSE /= (double)layers[num_layers - 1].num_Neurons;
	// error absolute
	dMAE /= (double)layers[num_layers - 1].num_Neurons;


}

void Network::BackPropagateError()
{
	int i, j, k;

	for (i = (num_layers - 2); i >= 0; i--)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			double output = layers[i].neurons[j].output;
			double error = 0;
			for (k = 0; k < layers[i + 1].num_Neurons; k++)
			{
				error += layers[i + 1].neurons[k].weight[j] * layers[i + 1].neurons[k].error;
			}
			layers[i].neurons[j].error = dGain * output *(1.0 - output)* error;
		}
	}
}

void Network::AdjustWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				double output = layers[i - 1].neurons[k].output;
				double error = layers[i].neurons[j].error;
				double preweight = layers[i].neurons[j].pre_Weight[k];
				layers[i].neurons[j].weight[k] += dEta * output * error * dAlpha * preweight;
				layers[i].neurons[j].pre_Weight[k] = dEta * output * error;
			}
		}
	}
}

void Network::Simulate(double* input, double* output, double* target, bool training)
{
	if (!input) return;
	if (!target) return;

	SetInputSignal(input);
	PropagateSignal();
	if (output) GetOutputSignal(output);

	//if (output && !training) printf("test: %.2f %.2f %.2f = %.2f\n", input[0], input[1], target[0], output[0]);

	ComputeOutputError(target);

	if (training)
	{
		BackPropagateError();
		AdjustWeights();
	}
}

bool read_number(FILE* fp, double* number)
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
			(b == '9'))
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

int Network::Train(const char* fname)
{
	int count = 0;
	int nbi = 0;
	int nbt = 0;
	double* input = NULL;
	double* output = NULL;
	double* target = NULL;
	FILE* fp;
	errno_t err;

	if ((err = fopen_s(&fp, fname, "r")) != 0)
	{
		printf("couldn't open file");
		return 0;
	}
	

	input = new double[layers[0].num_Neurons];
	output = new double[layers[num_layers - 1].num_Neurons];
	target = new double[layers[num_layers - 1].num_Neurons];

	if (!input) return 0;
	if (!output) return 0;
	if (!target) return 0;

	while (!feof(fp))
	{
		double dNumber;
		if (read_number(fp, &dNumber))
		{
			if (nbi < layers[0].num_Neurons)
				input[nbi++] = dNumber;
			else if (nbt < layers[num_layers - 1].num_Neurons)
				target[nbt++] = dNumber;

			if ((nbi == layers[0].num_Neurons) && (nbt == layers[num_layers - 1].num_Neurons))
			{
				Simulate(input, output, target, true);
				nbi = 0;
				nbt = 0;
				count++;
			}
		}
		else
		{
			break;
		}
	}

	if (fp) fclose(fp);
	if (input) delete[] input;
	if (output) delete[] output;
	if (target) delete[] target;

	return count;
}

int Network::Test(const char* fname)
{
	int count = 0;
	int nbi = 0;
	int nbt = 0;
	double* input = NULL;
	double* output = NULL;
	double* target = NULL;
	FILE* fp;
	errno_t err;

	if ((err = fopen_s(&fp, fname, "r")) != 0)
	{
		printf("couldn't open file");
		return 0;
	}


	input = new double[layers[0].num_Neurons];
	output = new double[layers[num_layers - 1].num_Neurons];
	target = new double[layers[num_layers - 1].num_Neurons];

	if (!input) return 0;
	if (!output) return 0;
	if (!target) return 0;

	dAvgTestError = 0;

	while (!feof(fp))
	{
		double dNumber;
		if (read_number(fp, &dNumber))
		{
			if (nbi < layers[0].num_Neurons)
				input[nbi++] = dNumber;
			else if (nbt < layers[num_layers - 1].num_Neurons)
				target[nbt++] = dNumber;

			if ((nbi == layers[0].num_Neurons) && (nbt == layers[num_layers - 1].num_Neurons))
			{

				Simulate(input, output, target, false);
				dAvgTestError += dMAE;
				nbi = 0;
				nbt = 0;
				count++;
			}
		}
		else
		{
			break;
		}
	}

	dAvgTestError /= count;

	if (fp) fclose(fp);
	if (input) delete[] input;
	if (output) delete[] output;
	if (target) delete[] target;

	return count;
}

int Network::Evaluate()
{
	int count = 0;
	return count;
}

void Network::Run(const char* fname, const int& maxiter)
{
	int countTrain = 0;
	int countLines = 0;
	bool Stop = false;
	bool firstIter = true;
	double dMinTestError = 0;

	InititaliseRandoms();
	RandomWeights();

	do
	{
		countLines += Train(fname);
		Test(fname);
		countTrain++;


		if (firstIter)
		{
			dMinTestError = dAvgTestError;
			firstIter = false;
		}

		printf("%i \t Test Error: %f", countTrain, dAvgTestError);

		if (dAvgTestError < dMinTestError)
		{
			printf(" -> saving weights\n");
			dMinTestError = dAvgTestError;
			SaveWeights();
		}
		else if (dAvgTestError > 1.2 * dMinTestError)
		{
			printf(" -> stopping training and restoring weights\n");
			Stop = true;
			RestoreWeights();
		}
		else
		{
			printf(" -> ok\n");
		}

	} while ((!Stop) && (countTrain < maxiter));

	printf("byeeeeeeeee\n");
}

void Network::TrainRMGS(const char* fname)
{
	/*
	1. Consider a three layer perceptron with two hidden layers. 
	2. Initialise the weights of the first hidden layer to small random values. Use the RW technique from Section III. 
	3. Present input vectors (x1, x2,...,xn) and desired output vectors (d1, d2,...,dn). 
	convert to binary vectors?
	4. Adjust the weights of the second hidden layer using MBD techniques from Section III. 
	5. Calculate actual outputs at the second hidden layer. Use Equations (1), (2), (5) and (6). 

	6. Develop a linear system of equations for the output layer. 
	Use Equation (10) and convert the output nonlinear activation function into a linear function. 
	Use Equations (11) and develop a linear system of equations as shown in Equation (12). 

	7. Calculate the weights of the output layer. Use the modified Gram-Schmidt algorithm to solve (12). 
	8. Repeat Step 6 through 7 for each neuron in the hidden layer.
	
	*/

	/*
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				layers[i].neurons[j].weight[k] = RandomEqualREAL(-0.5, 0.5);
				layers[i].neurons[j].pre_Weight[k] = 0.0;
				layers[i].neurons[j].saved_weight[k] = 0.0;
			}
			
		}
	}
	*/


	// set the first hidden layer to random small weights
	int j, k;
	for (j = 0; j < layers[1].num_Neurons; j++)
	{
		for (k = 0; k < layers[0].num_Neurons; k++)
		{
			layers[1].neurons[j].weight[k] = RandomEqualREAL(-0.5, 0.5);
			layers[1].neurons[j].pre_Weight[k] = 0.0;
			layers[1].neurons[j].saved_weight[k] = 0.0;
		}

	}
	// calculate the second layer using MBD 
	for (j = 0; j < layers[2].num_Neurons; j++)
	{
		for (k = 0; k < layers[1].num_Neurons; k++)
		{
			
		}
	}

	// Calculate output layer using modified gram schmidt 

	

}