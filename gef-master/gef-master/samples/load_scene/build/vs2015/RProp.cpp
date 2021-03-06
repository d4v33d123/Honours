#include "RProp.h"
#include <math.h>

// three helper functions to avoid having to actually write for loops or use the "sign" maths function 
void copy_array_noindex(double* array1, double* array2, int size)
{
	for (int i = 0; i < size; i++)
	{
		array2[i] = array1[i];
	}
}

void copy_array_index(double* array1, int index1, double* array2, int index2, int size)
{
	for (int i = 0; i < size; i++)
	{
		array2[i + index2] = array1[i + index1];
	}
}

int sgn(double x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

int MaxIndex(double* vector, int size) // helper for Accuracy()
{
	// index of largest value
	int bigIndex = 0;
	double biggestVal = vector[0];
	for (int i = 0; i < size; ++i)
	{
		if (vector[i] > biggestVal)
		{
			biggestVal = vector[i];
			bigIndex = i;
		}
	}
	return bigIndex;
}

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
			layers[i].neurons[j].bias = 0.1;

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

int RProp::Train(const char* fnames, int trainDataSize, int numInAndOut)
{
	InititaliseRandoms(time_t(NULL));

	double* hGradTerms = MakeVector(layers[1].num_Neurons, 0.0);
	double* oGradTerms = MakeVector(layers[2].num_Neurons, 0.0);

	double** hoWeightGradsAcc = MakeMatrix(layers[1].num_Neurons, layers[2].num_Neurons, 0.0); // accumulated over all training data
	double** ihWeightGradsAcc = MakeMatrix(layers[0].num_Neurons, layers[1].num_Neurons, 0.0);
	double* oBiasGradsAcc = MakeVector(layers[2].num_Neurons, 0.0);
	double* hBiasGradsAcc = MakeVector(layers[1].num_Neurons, 0.0);

	double** hoPrevWeightGradsAcc = MakeMatrix(layers[1].num_Neurons, layers[2].num_Neurons, 0.0); // accumulated, previous iteration
	double** ihPrevWeightGradsAcc = MakeMatrix(layers[0].num_Neurons, layers[1].num_Neurons, 0.0);
	double* oPrevBiasGradsAcc = MakeVector(layers[2].num_Neurons, 0.0);
	double* hPrevBiasGradsAcc = MakeVector(layers[1].num_Neurons, 0.0);

	// must save previous weight deltas
	double** hoPrevWeightDeltas = MakeMatrix(layers[1].num_Neurons, layers[2].num_Neurons, 0.01);
	double** ihPrevWeightDeltas = MakeMatrix(layers[0].num_Neurons, layers[1].num_Neurons, 0.01);
	double* oPrevBiasDeltas = MakeVector(layers[2].num_Neurons, 0.01);
	double* hPrevBiasDeltas = MakeVector(layers[1].num_Neurons, 0.01);

	double etaPlus = 1.2;//1.2; // values are from the paper
	double etaMinus = 0.5;//0.5;
	double deltaMax = 50.0;
	double deltaMin = 1.0E-6;
	int maxEpochs = 10000;//10000;
	float preverr = 1.0;
	float meansquarederror = 1.0;
	float meansquaredsum = 0.0;

	// get the training data
	double** trainData = fillTrainingData(fnames, trainDataSize, layers[0].num_Neurons + layers[2].num_Neurons);


	RandomWeights();

	int epoch = 0;
	while (epoch < maxEpochs)
	{
		++epoch;
		meansquaredsum = 0;
		// check to see if the error is less than the lowest one
		if (meansquarederror <= preverr)
		{
			SaveWeights();
			preverr = meansquarederror;
			gef::DebugOut("epoch: %i   error:%f \n", epoch, meansquarederror);
		}


		// update this with getting the current weights of all the layers
		if (epoch % 100 == 0 && epoch != maxEpochs)
		{
			
			gef::DebugOut("epoch: %i   error:%f \n", epoch, meansquarederror);
		}
		
		// 1. compute and accumulate all gradients
		ZeroOutMat(hoWeightGradsAcc, layers[1].num_Neurons, layers[2].num_Neurons); // zero-out values from prev iteration
		ZeroOutMat(ihWeightGradsAcc, layers[0].num_Neurons, layers[1].num_Neurons);
		ZeroOutArray(oBiasGradsAcc, layers[2].num_Neurons);
		ZeroOutArray(hBiasGradsAcc, layers[1].num_Neurons);

		double* xValues = new double[layers[0].num_Neurons]; // inputs
		double* tValues = new double[layers[2].num_Neurons]; // target values
		double* outputs = new double[layers[2].num_Neurons];
		//int add = trainDataSize / 25;

		for (int row = 0; row < trainDataSize; row++)  // walk thru all training data
		{
			// no need to visit in random order because all rows processed before any updates ('batch')
			copy_array_noindex(trainData[row], xValues, layers[0].num_Neurons); // get the inputs
			copy_array_index(trainData[row], layers[0].num_Neurons, tValues, 0, layers[2].num_Neurons); // get the target values
			ComputeOutputs(xValues, layers[0].num_Neurons, outputs); // copy xValues in, compute outputs using curr weights (and store outputs internally)
			for (int j = 0; j < layers[2].num_Neurons; j++)
			{
				meansquaredsum += ((tValues[j] - outputs[j]) * (tValues[j] - outputs[j]));//((yValues[j] - tValues[j]) * (yValues[j] - tValues[j]));
																						   //gef::DebugOut("the error %i: %f\n",j, (yValues[j] - tValues[j]));
			}


			// compute the h-o gradient term/component as in regular back-prop
			// this term usually is lower case Greek delta but there are too many other deltas below
			for (int i = 0; i < layers[num_layers - 1].num_Neurons; i++)
			{
				double derivative = (1 / (1 + exp(-outputs[i])) ) * (1 - (1 / (1 + exp(-outputs[i]))));                                  // (1 - outputs[i]) * (1 + outputs[i]); // derivative of softmax = (1 - y) * y (same as log-sigmoid)
				oGradTerms[i] = derivative * (outputs[i] - tValues[i]);                                                                    // careful with O-T vs. T-O, O-T is the most usual
			}

			// compute the i-h gradient term/component as in regular back-prop
			for (int i = 0; i < layers[1].num_Neurons; i++)
			{
				double derivative = ((1 / (1 + exp(-layers[1].neurons[i].output))) * (1 - (1 / (1 + exp(-layers[1].neurons[i].output)))));// (1 - layers[1].neurons[i].output) * (1 + layers[1].neurons[i].output); // derivative of tanh = (1 - y) * (1 + y)
				double sum = 0.0;
				for (int j = 0; j < layers[2].num_Neurons; j++) // each hidden delta is the sum of numOutput terms
				{
					double x = oGradTerms[j] * layers[2].neurons[j].weight[i];
					sum += x;
				}
				hGradTerms[i] = derivative * sum;
			}

			// add input to h-o component to make h-o weight gradients, and accumulate
			for (int i = 0; i < layers[1].num_Neurons; i++)
			{
				for (int j = 0; j < layers[2].num_Neurons; j++)
				{
					double grad = oGradTerms[j] * layers[1].neurons[i].output;
					hoWeightGradsAcc[i][j] += grad;
				}
			}

			// the (hidden-to-) output bias gradients
			for (int i = 0; i < layers[2].num_Neurons; i++)
			{
				double grad = oGradTerms[i] * 1.0; // dummy input
				oBiasGradsAcc[i] += grad;
			}

			// add input term to i-h component to make i-h weight gradients and accumulate
			for (int i = 0; i < layers[0].num_Neurons; i++)
			{
				for (int j = 0; j < layers[1].num_Neurons; j++)
				{
					double grad = hGradTerms[j] * layers[0].neurons[i].output;
					ihWeightGradsAcc[i][j] += grad;
				}
			}

			// the (input-to-) hidden bias gradient
			for (int i = 0; i < layers[1].num_Neurons; i++)
			{
				double grad = hGradTerms[i] * 1.0;
				hBiasGradsAcc[i] += grad;
			}
			//gef::DebugOut("row %i \n", row);
		} // each row
		  // end compute all gradients

		  // update all weights and biases (in any order)

		  // update input-hidden weights
		double delta = 0.0;

		for (int i = 0; i < layers[0].num_Neurons; i++)
		{
			for (int j = 0; j < layers[1].num_Neurons; j++)
			{
				if ((ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j]) > 0) // no sign change, increase delta
				{
					delta = ihPrevWeightDeltas[i][j] * etaPlus; // compute delta
					if (delta > deltaMax) delta = deltaMax; // keep it in range
					double tmp = (-sgn(ihWeightGradsAcc[i][j])) * delta; // determine direction and magnitude
					layers[1].neurons[j].weight[i] += tmp; // update weights
				}
				else if ((ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j]) < 0) // grad changed sign, decrease delta
				{
					delta = ihPrevWeightDeltas[i][j] * etaMinus; // the delta (not used, but saved for later)
					if (delta < deltaMin) delta = deltaMin; // keep it in range
					layers[1].neurons[j].weight[i] -= ihPrevWeightDeltas[i][j]; // revert to previous weight
					ihWeightGradsAcc[i][j] = 0; // forces next if-then branch, next iteration
				}
				else // this happens next iteration after 2nd branch above (just had a change in gradient)
				{
					delta = ihPrevWeightDeltas[i][j]; // no change to delta
													  // no way should delta be 0 . . . 
					double tmp = (-sgn(ihWeightGradsAcc[i][j])) * delta; // determine direction
					layers[1].neurons[j].weight[i] += tmp; // update
				}
				//Console.WriteLine(ihPrevWeightGradsAcc[i][j] + " " + ihWeightGradsAcc[i][j]); Console.ReadLine();

				ihPrevWeightDeltas[i][j] = delta; // save delta
				ihPrevWeightGradsAcc[i][j] = ihWeightGradsAcc[i][j]; // save the (accumulated) gradient
			} // j
		} // i

		  // update (input-to-) hidden biases
		for (int i = 0; i < layers[1].num_Neurons; i++)
		{
			if ((hPrevBiasGradsAcc[i] * hBiasGradsAcc[i]) > 0) // no sign change, increase delta
			{
				delta = hPrevBiasDeltas[i] * etaPlus; // compute delta
				if (delta > deltaMax) delta = deltaMax;
				double tmp = (-sgn(hBiasGradsAcc[i])) * delta; // determine direction
				layers[1].neurons[i].bias += tmp; // update
			}
			else if ((hPrevBiasGradsAcc[i] * hBiasGradsAcc[i]) < 0) // grad changed sign, decrease delta
			{
				delta = hPrevBiasDeltas[i] * etaMinus; // the delta (not used, but saved later)
				if (delta < deltaMin) delta = deltaMin;
				layers[1].neurons[i].bias -= hPrevBiasDeltas[i]; // revert to previous weight
				hBiasGradsAcc[i] = 0; // forces next branch, next iteration
			}
			else // this happens next iteration after 2nd branch above (just had a change in gradient)
			{
				delta = hPrevBiasDeltas[i]; // no change to delta

				if (delta > deltaMax) delta = deltaMax;
				else if (delta < deltaMin) delta = deltaMin;
				// no way should delta be 0 . . . 
				double tmp = (-sgn(hBiasGradsAcc[i])) * delta; // determine direction
				layers[1].neurons[i].bias += tmp; // update
			}
			hPrevBiasDeltas[i] = delta;
			hPrevBiasGradsAcc[i] = hBiasGradsAcc[i];
		}

		// update hidden-to-output weights
		for (int i = 0; i < layers[1].num_Neurons; i++)
		{
			for (int j = 0; j < layers[2].num_Neurons; j++)
			{
				if ((hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j]) > 0) // no sign change, increase delta
				{
					delta = hoPrevWeightDeltas[i][j] * etaPlus; // compute delta
					//gef::DebugOut("Delta: %f    ", delta);
					if (delta > deltaMax) delta = deltaMax;
					double tmp = (-sgn(hoWeightGradsAcc[i][j])) * delta; // determine direction
					layers[2].neurons[j].weight[i] += tmp; // update
					//gef::DebugOut("tmp: %f \n", tmp);
				}
				else if ((hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j]) < 0) // grad changed sign, decrease delta
				{
					delta = hoPrevWeightDeltas[i][j] * etaMinus; // the delta (not used, but saved later)
					if (delta < deltaMin) delta = deltaMin;
					layers[2].neurons[j].weight[i] -= hoPrevWeightDeltas[i][j]; // revert to previous weight
					hoWeightGradsAcc[i][j] = 0; // forces next branch, next iteration
				}
				else // this happens next iteration after 2nd branch above (just had a change in gradient)
				{
					delta = hoPrevWeightDeltas[i][j]; // no change to delta
													  // no way should delta be 0 . . . 
					double tmp = (-sgn(hoWeightGradsAcc[i][j])) * delta; // determine direction
					layers[2].neurons[j].weight[i] += tmp; // update
				}
				//gef::DebugOut("neuron %i weight %i: %f      ", j, i, layers[2].neurons[j].weight[i]);
				hoPrevWeightDeltas[i][j] = delta; // save delta
				hoPrevWeightGradsAcc[i][j] = hoWeightGradsAcc[i][j]; // save the (accumulated) gradients
			} // j
			//gef::DebugOut("\n");
		} // i

		  // update (hidden-to-) output biases
		for (int i = 0; i < layers[2].num_Neurons; i++)
		{
			if ((oPrevBiasGradsAcc[i] * oBiasGradsAcc[i]) > 0) // no sign change, increase delta
			{
				delta = oPrevBiasDeltas[i] * etaPlus; // compute delta
				if (delta > deltaMax) delta = deltaMax;
				double tmp = (-sgn(oBiasGradsAcc[i])) * delta; // determine direction
				layers[2].neurons[i].bias += tmp; // update
			}
			else if ((oPrevBiasGradsAcc[i] * oBiasGradsAcc[i]) < 0) // grad changed sign, decrease delta
			{
				delta = oPrevBiasDeltas[i] * etaMinus; // the delta (not used, but saved later)
				if (delta < deltaMin) delta = deltaMin;
				layers[2].neurons[i].bias -= oPrevBiasDeltas[i]; // revert to previous weight
				oBiasGradsAcc[i] = 0; // forces next branch, next iteration
			}
			else // this happens next iteration after 2nd branch above (just had a change in gradient)
			{
				delta = oPrevBiasDeltas[i]; // no change to delta
											// no way should delta be 0 . . . 
				double tmp = (-sgn(hBiasGradsAcc[i])) * delta; // determine direction
				layers[2].neurons[i].bias += tmp; // update
			}
			oPrevBiasDeltas[i] = delta;
			oPrevBiasGradsAcc[i] = oBiasGradsAcc[i];
		}

		meansquarederror = meansquaredsum / trainDataSize;

	} // while


	RestoreWeights();

	return 0;
}

int RProp::Test(const char* fname)
{
	return 0;
}

void RProp::Accuracy(const char* fnames, int trainDataSize)
{
	// no need to set the weights as they are aleady set in the layers variable
	//this.SetWeights(weights);
	// percentage correct using winner-takes all
	int numCorrect = 0;
	int numWrong = 0;
	double* xValues = new double[layers[0].num_Neurons]; // inputs
	double* tValues = new double[layers[2].num_Neurons]; // targets
	double* yValues = new double[layers[2].num_Neurons]; // computed Y
	//double* outputs ;

	double** trainData = fillTrainingData(fnames, trainDataSize, layers[0].num_Neurons + layers[2].num_Neurons);

	for (int i = 0; i < trainDataSize; i++)
	{
		copy_array_noindex(trainData[i], xValues, layers[0].num_Neurons); // parse data into x-values and t-values
		copy_array_index(trainData[i], layers[0].num_Neurons, tValues, 0, layers[2].num_Neurons);
		ComputeOutputs(xValues, layers[0].num_Neurons, yValues);
		int maxIndex = MaxIndex(yValues, layers[2].num_Neurons/*test data length?*/); // which cell in yValues has largest value?

		if (tValues[maxIndex] == 1.0) // ugly. consider AreEqual(double x, double y, double epsilon)
			++numCorrect;
		else
			++numWrong;
	}
	gef::DebugOut("Accuracy : %f\n", (numCorrect * 1.0) / (numCorrect + numWrong)); // ugly 2 - check for divide by zero
}

int RProp::Evaluate()
{
	return 0;

}

void RProp::Run(const char* fname, const int& maxiter)
{


}

void RProp::RandomWeights()
{
	int i, j, k;
	for (i = 1; i < num_layers; i++)
	{
		for (j = 0; j < layers[i].num_Neurons; j++)
		{
			for (k = 0; k < layers[i - 1].num_Neurons; k++)
			{
				layers[i].neurons[j].weight[k] = RandomEqualREAL(0.5, -0.5); 
				layers[i].neurons[j].pre_Weight[k] = 0.0;
				layers[i].neurons[j].saved_weight[k] = 0.0;
			}
			layers[i].neurons[j].bias = RandomEqualREAL(0.001, 0.01); 
			layers[i].neurons[j].savedBias = 0;

		}
	}
}

void RProp::PropagateSignal()
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
			layers[i].neurons[j].output = 1.0 / (1.0 + exp(-dGain * sum));
		}
	}
}

void RProp::AdjustWeights()
{

}

void RProp::Simulate(double* input, double* output, double* target, bool training)
{

}

// multiple helper functions for quickly resetting arrays, doing some mathematics. The maths functions help more if you are using the
// RPROP algorithm for a classfication problem, but we are using it for a regression problem in this application

void RProp::ZeroOutMat(double** matrix, int rows, int cols)
{
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			matrix[r][c] = 0;
		}
	}
}

void RProp::ZeroOutArray(double* ary, int size) // helper for Train
{
	for (int i = 0; i < size; i++)
		ary[i] = 0.0;
}

double HyperTan(double x)
{
	if (x < -20.0) return -1.0; // approximation is correct to 30 decimals
	else if (x > 20.0) return 1.0;
	else return tanh(x);
}

double* Softmax(double* oSums, int size)
{
	// does all output nodes at once so scale doesn't have to be re-computed each time
	// determine max output-sum
	double max = oSums[0];
	for (int i = 0; i < size; i++)
		if (oSums[i] > max) max = oSums[i];

	// determine scaling factor -- sum of exp(each val - max)
	double scale = 0.0;
	for (int i = 0; i < size; i++)
		scale += exp(oSums[i] - max);
		

	double* result = new double[size];
	for (int i = 0; i < size; i++)
		result[i] = exp(oSums[i] - max) / scale;

	return result; // now scaled so that xi sum to 1.0
}

double RProp::MeanSquaredError(double** trainData, int size) //double RProp::MeanSquaredError(double** trainData, double* weights, int size)
{
	//SetWeights(weights); // copy the weights to evaluate in

	double* xValues = new double[layers[0].num_Neurons]; // inputs
	double* tValues = new double[layers[2].num_Neurons]; // targets
	double* yValues = new double[layers[2].num_Neurons];
	double sumSquaredError = 0.0;
	for (int i = 0; i < size; i++) // walk through each training data item
	{
		// following assumes data has all x-values first, followed by y-values!
		copy_array_noindex(trainData[i], xValues, layers[0].num_Neurons); // extract inputs
		copy_array_index(trainData[i], layers[0].num_Neurons, tValues, 0, layers[2].num_Neurons); // extract targets
		int sizeofy = layers[0].num_Neurons;
		ComputeOutputs(xValues, sizeofy, yValues);
		for (int j = 0; j < sizeofy; j++)
		{
			sumSquaredError += ((tValues[j] - yValues[j]) * (tValues[j] - yValues[j]));//((yValues[j] - tValues[j]) * (yValues[j] - tValues[j]));
			//gef::DebugOut("the error %i: %f\n",j, (yValues[j] - tValues[j]));
		}
			
	}
	sumSquaredError /= size;

	//gef::DebugOut("MeanSquaredError: %f", sumSquaredError);

	return sumSquaredError;
}

void RProp::ComputeOutputs(double* xValues, int size, double* outs)
{
	

	for (int i = 0; i < size; i++) // copy x-values to inputs
		layers[0].neurons[i].output = xValues[i];

	for (int j = 0; j < layers[1].num_Neurons; j++)  // compute i-h sum of weights * inputs
		for (int i = 0; i < layers[0].num_Neurons; i++)
		{
			double output = layers[0].neurons[i].output;
			double weight = layers[1].neurons[j].weight[i];
			layers[1].neurons[j].output += (output * weight); // note +=
		}
			

	for (int i = 0; i < layers[1].num_Neurons; i++)  // add biases to input-to-hidden sums
		layers[1].neurons[i].output += layers[1].neurons[i].bias;

	for (int i = 0; i < layers[1].num_Neurons; i++)   // apply activation
	{
		double output = layers[1].neurons[i].output;
		double sig = 1 / (1 + exp(-(layers[1].neurons[i].output)));
		layers[1].neurons[i].output = sig; // hard-coded
	}
		
	for (int j = 0; j < layers[2].num_Neurons; j++)  // compute i-h sum of weights * inputs
		for (int i = 0; i < layers[1].num_Neurons; i++)
		{
			double output = layers[1].neurons[i].output;
			double weight = layers[2].neurons[j].weight[i];
			layers[2].neurons[j].output += (output * weight); // note +=
		}

	for (int i = 0; i < layers[2].num_Neurons; i++)  // add biases to input-to-hidden sums
		layers[2].neurons[i].output += layers[2].neurons[i].bias;

	for (int i = 0; i < layers[2].num_Neurons; i++)  // sigmoid activation
	{
		double sig = 1 / (1 + exp(-(layers[2].neurons[i].output))); //layers[2].neurons[i].output / (1 + exp(-(layers[2].neurons[i].output)));
		layers[2].neurons[i].output = sig;
	}
		


	for (int i = 0; i < layers[2].num_Neurons; i++)
	{
		outs[i] = layers[2].neurons[i].output;
	}

}

double** RProp::fillTrainingData(const char* fname, int rows, int cols)
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

double* RProp::GetWeights()
{
	int numWeights = (layers[0].num_Neurons * layers[1].num_Neurons) + (layers[1].num_Neurons * layers[2].num_Neurons) + layers[1].num_Neurons + layers[2].num_Neurons;
	double* result = new double[numWeights];
	int k = 0;
	for (int i = 0; i < layers[0].num_Neurons; ++i)
		for (int j = 0; j < layers[1].num_Neurons; ++j)
			result[k++] = layers[0].neurons[i].weight[j];
	for (int i = 0; i < layers[1].num_Neurons; ++i)
		result[k++] = layers[0].neurons[i].bias;
	for (int i = 0; i < layers[1].num_Neurons; ++i)
		for (int j = 0; j < layers[2].num_Neurons; ++j)
			result[k++] = layers[0].neurons[i].weight[j];
	for (int i = 0; i < layers[2].num_Neurons; ++i)
		result[k++] = layers[0].neurons[i].bias;
	return result;
}