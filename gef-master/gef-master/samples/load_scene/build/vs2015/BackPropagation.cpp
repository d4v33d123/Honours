#include "BackPropagation.h"
#include "system\debug_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// initialise EBP
BProp::BProp(int nl, int npl[])
{
	num_layers = 0;
	layers = 0;
	dMSE = 0;
	dMAE = 0;
	dEta = 0.25; //0.25
	dAlpha = 0.9; // 0.9
	dGain = 1.0; // 1.0
	dAvgTestError = 0.0; 
	
	int i, j;

	num_layers = nl;
	layers = new Layer[nl];

	for (i = 0; i < nl; i++)
	{
		layers[i].num_Neurons = npl[i];
		layers[i].neurons = new Neuron[npl[i]];

		for (j = 0; j < npl[i]; j++)
		{
			layers[i].neurons[j].output = 0.5;
			layers[i].neurons[j].error = 0.0;
			layers[i].neurons[j].bias = 0.0;
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


BProp::~BProp()
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

// set the network weights to random values between -0.5 and 0.5
void BProp::RandomWeights()
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

// pass the signal through  the network
void BProp::PropagateSignal()
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
			sum += layers[i].neurons[j].bias;
			// activation funciton
			layers[i].neurons[j].output = (1.0 / (1.0 + exp(-dGain * sum)));
		}
	}
}

// compute the output error
void BProp::ComputeOutputError(double* target)
{
	int i;
	dMAE = 0;
	dMSE = 0;
	for (i = 0; i < layers[num_layers - 1].num_Neurons; i++)
	{
		double output = layers[num_layers - 1].neurons[i].output;
		double delta = (target[i] - output);
		layers[num_layers - 1].neurons[i].error = ((dGain * output * (1.0 - output)) * delta);

		//gef::DebugOut("layer: %i  neuron: %i     output: %f    target: %f\n", num_layers - 1, i, output, target[i]);

		dMSE += (delta*delta);
		dMAE += fabs(delta);
	}
	// error quadrative 
	dMSE /= (double)layers[num_layers - 1].num_Neurons;
	// error absolute
	dMAE /= (double)layers[num_layers - 1].num_Neurons;


}

// EBP 
void BProp::BackPropagateError()
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
			layers[i].neurons[j].error = (dGain * (output *(1.0 - output))* error);
		}
	}
}

// weight adjustments
void BProp::AdjustWeights()
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
				layers[i].neurons[j].weight[k] += (dEta * output * error )+( dAlpha * preweight); // -= is training better than += ?? WIT
				layers[i].neurons[j].pre_Weight[k] = (dEta * output * error);
			}
		}
	}
}

// simulate the network
void BProp::Simulate(double* input, double* output, double* target, bool training)
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


// train the network
int BProp::Train(const char* fname, int datasize, double** TrainDat)
{
	int count = 0;
	int nbi = 0;
	int nbt = 0;
	double* input = NULL;
	double* output = NULL;
	double* target = NULL;
	//FILE* fp;
	//errno_t err;

	//if ((err = fopen_s(&fp, fname, "r")) != 0)
	//{
	//	printf("couldn't open file");
	//	return 0;
	//}


	input = new double[layers[0].num_Neurons];
	output = new double[layers[num_layers - 1].num_Neurons];
	target = new double[layers[num_layers - 1].num_Neurons];

	if (!input) return 0;
	if (!output) return 0;
	if (!target) return 0;

	while (count < datasize)
	{
		
		
		if (nbi < layers[0].num_Neurons)
			input[nbi++] = TrainDat[count][nbi];
		else if (nbt < layers[num_layers - 1].num_Neurons)
			target[nbt++] = TrainDat[count][(nbi + nbt)];

		if ((nbi == layers[0].num_Neurons) && (nbt == layers[num_layers - 1].num_Neurons))
		{
			Simulate(input, output, target, true);
			nbi = 0;
			nbt = 0;
			count++;
		}
		
	}

	//if (fp) fclose(fp);
	/*for (int i = 0; i < datasize; i++)
	{
		delete[] TrainDat[i];
	}
	if (TrainDat) delete[] TrainDat;*/
	if (input) delete[] input;
	if (output) delete[] output;
	if (target) delete[] target;

	return count;
}

// fill the training data from the data file
double** BProp::fillTrainingData(const char* fname, int rows, int cols)
{
	double** result = MakeMatrix(rows, cols, 0);
	int count = 0;
	int nbi = 0;
	int nbt = 0;
	FILE* fp;
	errno_t err;

	if ((err = fopen_s(&fp, fname, "r")) != 0)
	{
		printf("couldn't open file");
		return 0;
	}

	while (!feof(fp))
	{
		double dNumber;
		if (read_number(fp, &dNumber))
		{
			//gef::DebugOut("val: %f", dNumber);
			if (nbi < layers[0].num_Neurons)
			{
				result[count][nbi] = dNumber;
				nbi++;
			}

			else if (nbt < layers[num_layers - 1].num_Neurons)
			{
				result[count][nbi + nbt] = dNumber;
				nbt++;
			}

			//gef::DebugOut("nbi: %i, nbt:%i", nbi, nbt);

			if ((nbi == layers[0].num_Neurons) && (nbt == layers[num_layers - 1].num_Neurons))
			{
				nbi = 0;
				nbt = 0;
				count++;
			}
		}
		else
		{
			break;
		}
		//gef::DebugOut("\n");
	}

	if (fp) fclose(fp);

	return result;
}

// test the network
int BProp::Test(const char* fname, int datasize, double** TrainDat)
{
	int count = 0;
	int nbi = 0;
	int nbt = 0;
	double* input = NULL;
	double* output = NULL;
	double* target = NULL;
	//FILE* fp;
	//errno_t err;

	//if ((err = fopen_s(&fp, fname, "r")) != 0)
	//{
	//	printf("couldn't open file");
	//	return 0;
	//}


	input = new double[layers[0].num_Neurons];
	output = new double[layers[num_layers - 1].num_Neurons];
	target = new double[layers[num_layers - 1].num_Neurons];

	if (!input) return 0;
	if (!output) return 0;
	if (!target) return 0;

	dAvgTestError = 0;

	while (count < datasize)
	{

		if (nbi < layers[0].num_Neurons)
			input[nbi++] = TrainDat[count][nbi];
		else if (nbt < layers[num_layers - 1].num_Neurons)
			target[nbt++] = TrainDat[count][(nbi + nbt)];

		if ((nbi == layers[0].num_Neurons) && (nbt == layers[num_layers - 1].num_Neurons))
		{
			Simulate(input, output, target, false);
			dAvgTestError += dMAE;
			nbi = 0;
			nbt = 0;
			count++;
		}

	}
	

	dAvgTestError /= count;

	if (input) delete[] input;
	if (output) delete[] output;
	if (target) delete[] target;

	return count;
}

// evaluate not used currently
int BProp::Evaluate()
{
	int count = 0;
	return count;
}


// run the network for "maxiter" iterations
void BProp::Run(const char* fname, int datasize,const int& maxiter)
{
	int countTrain = 0;
	int countLines = 0;
	bool Stop = false;
	bool firstIter = true;
	double dMinTestError = 0;

	InititaliseRandoms(200);
	RandomWeights();

	double** TrainDat = fillTrainingData(fname, datasize, (layers[0].num_Neurons + layers[num_layers - 1].num_Neurons));

	do
	{
		// run through the network and get the values
		countLines += Train(fname, datasize, TrainDat);
		Test(fname, datasize, TrainDat);
		countTrain++;

		// set the lowest error
		if (firstIter)
		{
			dMinTestError = dAvgTestError;
			firstIter = false;
		}
		// output ever 100th epoch with the current error
		if (countTrain % 100 == 0)
		{
			gef::DebugOut("%i \t Test Error: %f \n", countTrain, dAvgTestError);
		}
			
		// if the error is lower than the "lowest" erro, output the epoch and save the weights
		if (dAvgTestError < dMinTestError)
		{
			gef::DebugOut("%i \t Test Error: %f \n", countTrain, dAvgTestError);
			dMinTestError = dAvgTestError;
			SaveWeights();
		}
		else if (dAvgTestError > 1.3 * dMinTestError) // if the network is going too far in the wrong direction, just exit
		{
			gef::DebugOut(" -> stopping training and restoring weights\n");
			Stop = true;
			RestoreWeights();
		}
		else
		{
			//gef::DebugOut(" -> ok\n");
		}

	} while ((!Stop) && (countTrain < maxiter));

	printf("byeeeeeeeee\n");
}
