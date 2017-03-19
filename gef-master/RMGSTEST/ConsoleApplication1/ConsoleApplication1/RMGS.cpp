#include "RMGS.h"
#include <math.h>
//#include "system\debug_log.h"
#include <time.h>

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
	InititaliseRandoms((time(NULL)));
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

int RMGS::Train(const char* fnames, int ds)
{
	double** trainData = fillTrainingData(fnames, ds, layers[0].num_Neurons + layers[num_layers - 1].num_Neurons);

	int datasize = ds;

	double** MBDOutput = MakeMatrix(datasize, layers[2].num_Neurons, 0);
	double** FirstHiddenOutput = MakeMatrix(datasize, layers[1].num_Neurons, 0);

	// 1. 3 layer perceptron 2 hidden 1 output.
	// actually doing the 3 layer neuron with 1 input 1 hidden and 1 output
	// 2. initialise the weights of the first hidden layer with random weights
	RandomWeights();


	// 3. present input vectors (x1, x2....xn) and desired output vectors (d1, d2....dn)
	double** Inputdata = MakeMatrix(datasize, layers[1].num_Neurons, 0);
	for (int i = 0; i < datasize; i++)
	{
		for (int j = 0; j < layers[0].num_Neurons; j++)
		{
			layers[0].neurons[j].output = trainData[i][j];
			Inputdata[i][j] = trainData[i][j];
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
			layers[1].neurons[j].output = 1.0 / (1.0 + exp(-sum)); //1.0 - tanh(sum); //exp(-sum);//(1 - tanh(sum));// 
			FirstHiddenOutput[i][j] = layers[1].neurons[j].output;
		}
	}
	

	// 4. Adjust the weights of the second hidden layer using the MBD technique from section III
	//MBD(trainData, datasize, FirstHiddenOutput ,MBDOutput);

	// we have to transpose the MBDOutputs for the gram schmidt calculation
	/*double** TMBDout = MakeMatrix(layers[2].num_Neurons, datasize, 0);
	for (int i = 0; i < layers[2].num_Neurons; i++)
	{
		for (int j = 0; j < datasize; j++)
		{
			TMBDout[i][j] = MBDOutput[j][i];
			//printf("TMBDOut %i: %f     ", i, TMBDout[i][j]);
		}
		//printf("\n");
	}*/

	// we have to transpose the FirstHiddenoutputs for the gram schmidt calculation
	double** TFHO = MakeMatrix(layers[1].num_Neurons, datasize, 0);
	for (int i = 0; i < layers[1].num_Neurons; i++)
	{
		for (int j = 0; j < datasize; j++)
		{
			TFHO[i][j] = FirstHiddenOutput[j][i];
		}
	}

	// 5. Calcualte the actual outputs at the second hidden layer. Use Equations (1), (2), (5) and (6)


	// 6. Develop a linear system of equations for the output layer
		// Use equation (10) and convert the output nonlinear activation function to a linear function.


		// Use equations (11) and develop a linear system of equations as shown in Equation (12)


	// 7. Calaulate the weights of the output layer. Use the modified Gram-Schmidt algorithm to solve(12)

	// extract our outputs from our training data
	double** ExpectedOutputs = MakeMatrix(layers[num_layers - 1].num_Neurons, datasize, 0);
	for (int i = 0; i < layers[num_layers - 1].num_Neurons; i++)
	{
		for (int j = 0; j < datasize; j++)
		{
			ExpectedOutputs[i][j] = trainData[j][i+layers[0].num_Neurons];
		}
	}

	// perfrom gram schmidt on the output layer
	//GramSchmidt(MBDOutput, ExpectedOutputs, datasize, 2);
	GramSchmidt(FirstHiddenOutput, ExpectedOutputs, datasize, 1);

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
	i = 1;
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

void RMGS::MBD(double** trainingData, int size, double** FirstHiddenOutput, double** Out)
{

	// take the first training data input 
	int radial = (size / layers[2].num_Neurons);

	// compare it to all of the neurons which are weights of the rest of the training data inputs
	for (int i = 0; i < layers[2].num_Neurons; i++)
	{
		for (int j = 0; j < layers[1].num_Neurons; j++)
		{
				layers[2].neurons[i].weight[j] = trainingData[i*radial][j];
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
				// Total of all the  x - w s
				output += (pow((FirstHiddenOutput[d][j] - layers[2].neurons[i].weight[j]), 2) * (double(j + 1) / double(layers[1].num_Neurons)));
			}
			
			//output *= (double(i + 1) / double(layers[2].num_Neurons));
			// squared
			// mulitplied by current neuron / total neurons
			// square root of said value
			layers[2].neurons[i].output = sqrt(output);//sqrt(fabs(outputsquared));


			// put the value through the fitness function
			double nout = exp(-layers[2].neurons[i].output);//(1.0 - tanh(layers[2].neurons[i].output));//  
			layers[2].neurons[i].output = nout;

			Out[d][i] = layers[2].neurons[i].output;

		}
	}

}

// outputs must be arranged in the following [(neurons in current layer + 1) X size]
// hidden must be arranged in the following [size X (neurons in current layer)]
void RMGS::GramSchmidt(double** hidden, double** outputs, int size, int currentLayer)
{
	// see MGStesting solution 
	double** Q = MakeMatrix(size, layers[currentLayer].num_Neurons,  0);
	double** R = MakeMatrix(layers[currentLayer].num_Neurons, layers[currentLayer].num_Neurons, 0);
	double** V = MakeMatrix(size, layers[currentLayer].num_Neurons, 0);
	double* B = MakeVector(size, 0);
	// QR Decomposition of matrix 

	V = hidden;
	// http://www4.ncsu.edu/eos/users/w/white/www/white/ma580/chap3.3.PDF

	// IMPLEMENT NEW GRAM SCHMIDT METHOD MAYBE
	//lets try this again

	Q = V;

	for (int j = 0; j < layers[currentLayer].num_Neurons; j++)
	{
		double rjj = 0;
		for (int k = 0; k < size; k++)
		{
			double power = Q[k][j] * Q[k][j];
			rjj += power;
		}
		R[j][j] = (sqrt(rjj));

		for (int k = 0; k < size; k++)
		{
			Q[k][j] /= R[j][j];
		}

		for (int i = j + 1; i < layers[currentLayer].num_Neurons; i++)
		{
			R[i][j] = 0;
			for (int k = 0; k < size; k++)
			{
				R[i][j] += (V[k][i] * Q[k][j]);
			}


			for (int k = 0; k < size; k++)
			{
				double minus = (R[i][j] * Q[k][j]);
				Q[k][i] -= minus;
			}
		}
	}

	// now that we have  R and Q we can do some calcualtions, these are all the same for each of the output neurons.
	// we must firstly do Transpose(Q) * net = y
	// then we can do a back substitution of  R * w = y; to find w 

	double** TQ = MakeMatrix(layers[currentLayer].num_Neurons, size, 0);

	// get the transpose of Q
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < layers[currentLayer].num_Neurons; j++)
		{
			TQ[j][i] = Q[i][j];
		}
	}

	for (int n = 0; n < layers[currentLayer + 1].num_Neurons; n++)
	{
		//B = outputs[n];
		for (int i = 0; i < size; i++)
		{
			if (outputs[n][i] < 1)
			{
				B[i] = (log(outputs[n][i]) - log(1 - outputs[n][i]));
			}
			else
			{
				B[i] = 0;
			}
		}


		double* Y = MakeVector(layers[currentLayer].num_Neurons, 0);

		for (int i = 0; i < layers[currentLayer].num_Neurons; i++)
		{
			for (int j = 0; j < size; j++)
			{
				double multi = (Q[j][i] * B[j]);
				Y[i] += multi;
			}
		}

		// now that we have y we can do the gaussian elimination using R and Y to get W
		// since R is already in  an upper triangular matrice there is no need to change it before the calculation

		// we must now set up the augmented matrix involving R and y

		double** RYAug = MakeMatrix(layers[currentLayer].num_Neurons, layers[currentLayer].num_Neurons + 1, 0);

		for (int i = 0; i < layers[currentLayer].num_Neurons; i++)
		{
			for (int j = 0; j < (layers[currentLayer].num_Neurons + 1); j++)
			{
				if (j == layers[currentLayer].num_Neurons)
				{
					RYAug[i][j] = Y[i];
				}
				else
				{
					RYAug[i][j] = R[j][i];
				}

				
			}
		}

		double* W = MakeVector(layers[currentLayer].num_Neurons, 0);

		/*
		for (int i = layers[currentLayer].num_Neurons -1 ; i >= 0; i--)
		{
			W[i] = (RYAug[i][layers[currentLayer].num_Neurons] / RYAug[i][i]);
			
			for (int j = i - 1; j >= 0; j--)
			{

				RYAug[j][layers[currentLayer].num_Neurons] -= RYAug[j][i] * W[i];
				printf("Minus[%i]: %f\n", j, RYAug[j][i] * W[i]);
				printf("J[%i]: %f\n", j, RYAug[j][layers[currentLayer].num_Neurons]);
			}
		
			
			printf("W[%i]: %f\n", i, W[i]);
		}*/
		
		for (int i = layers[currentLayer].num_Neurons - 1; i >= 0; i--)
		{
			double minus = 0;
			for (int j = i + 1; j <= layers[currentLayer].num_Neurons - 1; j++)
			{
				minus += (RYAug[i][j] * W[j]);
			}
			RYAug[i][layers[currentLayer].num_Neurons] -= minus;

			W[i] = (RYAug[i][layers[currentLayer].num_Neurons] / RYAug[i][i]);
		}
		

		for (int i = 0; i < layers[currentLayer].num_Neurons; i++)
		{
			layers[currentLayer + 1].neurons[n].weight[i] = W[i];
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
				layers[i].neurons[j].output = (1.0 / (1.0 + exp(-sum))); //double(1) / tanh(sum); // log((sum / (1 - sum)));//// //
			
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

