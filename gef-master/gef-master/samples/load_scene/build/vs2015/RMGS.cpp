#include "RMGS.h"
#include <math.h>

RMGS::RMGS(int nl, int npl[])
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

RMGS::~RMGS()
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

int RMGS::Train(const char * fnames)
{
	double** trainData = fillTrainingData(fnames, layers[0].num_Neurons, layers[3].num_Neurons);
	int datasize = layers[2].num_Neurons;

	double** MBDOutput = MakeMatrix(datasize, layers[2].num_Neurons, 0);
	double** FirstHiddenOutput = MakeMatrix(datasize, layers[1].num_Neurons, 0);

	// 1. 3 layer perceptron 2 hidden 1 output.


	// 2. initialise the weights of the first hidden layer with random weights
	RandomWeights();


	// 3. present input vectors (x1, x2....xn) and desired output vectors (d1, d2....dn)
	for (int i = 0; i < datasize; i++)
	{
		for (int j = 0; j < layers[0].num_Neurons; j++)
		{
			layers[0].neurons[j].output = trainData[i][j];
		}

		for (int j = 0; j < layers[1].num_Neurons; j++)
		{
			double sum = 0;
			for (int k = 0; k < layers[0].num_Neurons; k++)
			{
				double output = layers[0].neurons[k].output;
				double weight = layers[1].neurons[j].weight[k];
				sum += weight*output;
			}
			// activation funciton
			layers[1].neurons[j].output = 1.0 / (1.0 + exp(-dGain * sum));
			FirstHiddenOutput[i][j] = layers[1].neurons[j].output;
		}

	}
	

	// 4. Adjust the weights of the second hidden layer using the MBD technique from section III
	MBD(trainData, datasize, FirstHiddenOutput ,MBDOutput);


	// 5. Calcualte the actual outputs at the second hidden layer. Use Equations (1), (2), (5) and (6)


	// 6. Develop a linear system of equations for the output layer
		// Use equation (10) and convert the output nonlinear activation function to a linear function.


		// Use equations (11) and develop a linear system of equations as shown in Equation (12)


	// 7. Calaulate the weights of the output layer. Use the modified Gram-Schmidt algorithm to solve(12)
	GramSchmidt();


	// Repeat step 6 and 7 for each neuron in the hidden layer


	return 0;
}

int RMGS::Test(const char * fname)
{

	return 0;
}

int RMGS::Evaluate()
{

	return 0;
}

void RMGS::Run(const char * fname, const int & maxiter)
{

}

void RMGS::RandomWeights()
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

void RMGS::MBD(double** trainingData, int size, double** FirstHiddenOutput, double** MBDOutput)
{

	// take the first training data input 
	//double* first_data = trainingData[0];

	// compare it to all of the neurons which are weights of the rest of the training data inputs
	for (int i = 0; i < layers[2].num_Neurons; i++)
	{
		for (int j = 0; j < layers[1].num_Neurons; j++)
		{
				layers[2].neurons[i].weight[j] = trainingData[i][j];
		}
	}
	
	
	
	// this techniques essentially uses the MLP 2nd hidden layer like a Self organising map for comparing the inputs to
	
	// for all of the test data
	for (int d = 0; d < size; d++)
	{
		for (int i = 0; i < layers[2].num_Neurons; i++)
		{
			double output = 0;
			for (int j = 0; j < layers[1].num_Neurons; j++)
			{
				//layers[2].neurons[i].weight[j] = trainingData[i + 1][j];
				// Total of all the  x - w s
				//output += layers[1].neurons[j].output - layers[2].neurons[i].weight[j];
				output += FirstHiddenOutput[d][j] - layers[2].neurons[i].weight[j];
			}
			// squared
			double outputsquared = output * output;
			// mulitplied by current neuron / total neurons
			outputsquared = outputsquared * (i / layers[2].num_Neurons);
			// square root of said value
			layers[2].neurons[i].output = sqrt(outputsquared);

			// put the value through the fitness function
			layers[2].neurons[i].output = 1 - tanh(layers[2].neurons[i].output);

			MBDOutput[d][i] = output;
		}
	}
	// for all of the neurons in the hidden layer 
	

}

void RMGS::GramSchmidt(double** firstHidden, double** secondHidden, int size)
{
	// see MGStesting solution 
	double** Q = MakeMatrix(size, layers[2].num_Neurons, 0);
	double** R = MakeMatrix(size, layers[2].num_Neurons, 0);
	double** V = MakeMatrix(size, layers[2].num_Neurons, 0);
	// QR Decomposition of matrix 
	double* Qvals = MakeVector(layers[2].num_Neurons, 0);
	
	// Firstly we need to get Q 
	for (int i = 0; i < layers[2].num_Neurons; i++)
	{
		for (int j = 0; j < size; j++)
		{
			Qvals[i] += secondHidden[j][i];
		}
		Qvals[i] = sqrt(Qvals[i]);
	}

	V = secondHidden;

	for (int i = 0; i < size; i++)
	{
		R[i][i] = Qvals[i];
		for (int j = 0; j < size; j++)
		{
			Q[j][i] = V[j][i] / Qvals[i];
		}
		
		for (int j = i + 1; j < layers[2].num_Neurons; j++)
		{
			R[i][j] = DotProduct(Q[i], V[j], size);
			double* QRMult = MakeVector(size, 0);
			QRMult = MultiplyVector(Q[i] , R[i][j], size);
			V[j] = MinusVectors(V[j], QRMult, size);
		}
	}

}


void RMGS::PropagateSignal()
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


void RMGS::ComputeOutputError(double * target)
{

}

void RMGS::BackPropagateError()
{

}

void RMGS::AdjustWeights()
{

}

void RMGS::Simulate(double * input, double * output, double * target, bool training)
{

}

double** RMGS::fillTrainingData(const char* fname, int rows, int cols)
{
	double** result = 0;
	result = new double*[rows];
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
			result[count] = new double[count];
			if (nbi < layers[0].num_Neurons)
				result[count][nbi++] = dNumber;
			else if (nbt < layers[num_layers - 1].num_Neurons)
				result[count][nbt++] = dNumber;

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
	}

	if (fp) fclose(fp);

	return result;
}
