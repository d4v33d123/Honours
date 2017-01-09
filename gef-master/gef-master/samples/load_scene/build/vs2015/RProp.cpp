#include "RProp.h"

RProp::RProp(int nl, int npl[])
{
	num_layers = 0;
	layers = 0;
	dMSE = 0;
	dMAE = 0;
	dEta = 0.25;
	dAlpha = 0.9;
	dGain = 1.0;
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
			layers[i].neurons[j].output = 1.0;
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

RProp::~RProp()
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

int RProp::Train(const char* fnames)
{
	double** hoWeightGradsAcc = MakeMatrix(layers[1].num_Neurons, layers[2].num_Neurons, 0.0); // accumulated over all training data
	double** ihWeightGradsAcc = MakeMatrix(layers[0].num_Neurons, layers[1].num_Neurons, 0.0);
	double* oBiasGradsAcc = new double[layers[2].num_Neurons];
	double* hBiasGradsAcc = new double[layers[1].num_Neurons];

	double** hoPrevWeightGradsAcc = MakeMatrix(layers[1].num_Neurons, layers[2].num_Neurons, 0.0); // accumulated, previous iteration
	double** ihPrevWeightGradsAcc = MakeMatrix(layers[0].num_Neurons, layers[1].num_Neurons, 0.0);
	double* oPrevBiasGradsAcc = new double[layers[2].num_Neurons];
	double* hPrevBiasGradsAcc = new double[layers[1].num_Neurons];

	// must save previous weight deltas
	double** hoPrevWeightDeltas = MakeMatrix(layers[1].num_Neurons, layers[2].num_Neurons, 0.01);
	double** ihPrevWeightDeltas = MakeMatrix(layers[0].num_Neurons, layers[1].num_Neurons, 0.01);
	double* oPrevBiasDeltas = MakeVector(layers[2].num_Neurons, 0.01);
	double* hPrevBiasDeltas = MakeVector(layers[1].num_Neurons, 0.01);

	double etaPlus = 1.2; // values are from the paper
	double etaMinus = 0.5;
	double deltaMax = 50.0;
	double deltaMin = 1.0E-6;

	return 0;
}

int RProp::Test(const char* fname)
{

	return 0;
}

int RProp::Evaluate()

	return 0;
}
{

void RProp::Run(const char* fname, const int& maxiter)
{


}

void RProp::RandomWeights()
{

}

void RProp::PropagateSignal()
{

}

void RProp::AdjustWeights()
{

}

void RProp::Simulate(double* input, double* output, double* target, bool training)
{

}

