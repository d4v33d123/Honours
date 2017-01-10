#include "RProp.h"
#include <math.h>

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
	double* hGradTerms = new double[layers[1].num_Neurons];
	double* oGradTerms = new double[layers[2].num_Neurons];

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
	int maxEpochs = 10000;

	int epoch = 0;
	while (epoch < maxEpochs)
	{
		++epoch;
		/* update this with getting the current weights of all the layers
		if (epoch % 100 == 0 && epoch != maxEpochs)
		{
			double[] currWts = this.GetWeights();
			double err = MeanSquaredError(trainData, currWts);
			Console.WriteLine("epoch = " + epoch + " err = " + err.ToString("F4"));
		}
		*/
		// 1. compute and accumulate all gradients
		ZeroOutMat(hoWeightGradsAcc); // zero-out values from prev iteration
		ZeroOutMat(ihWeightGradsAcc);
		ZeroOutArray(oBiasGradsAcc, layers[2].num_Neurons);
		ZeroOutArray(hBiasGradsAcc, layers[1].num_Neurons);

		double* xValues = new double[layers[0].num_Neurons]; // inputs
		double* tValues = new double[layers[2].num_Neurons]; // target values
		for (int row = 0; row < trainData.Length; ++row)  // walk thru all training data
		{
			// no need to visit in random order because all rows processed before any updates ('batch')
			copy_array_noindex(trainData[row], xValues, layers[0].num_Neurons); // get the inputs
			copy_array_index(trainData[row], layers[0].num_Neurons, tValues, 0, layers[2].num_Neurons); // get the target values
			double* outputs = ComputeOutputs(xValues); // copy xValues in, compute outputs using curr weights (and store outputs internally)

									 // compute the h-o gradient term/component as in regular back-prop
									 // this term usually is lower case Greek delta but there are too many other deltas below
			for (int i = 0; i < layers[2].num_Neurons; ++i)
			{
				double derivative = (1 - outputs[i]) * outputs[i]; // derivative of softmax = (1 - y) * y (same as log-sigmoid)
				oGradTerms[i] = derivative * (outputs[i] - tValues[i]); // careful with O-T vs. T-O, O-T is the most usual
			}

			// compute the i-h gradient term/component as in regular back-prop
			for (int i = 0; i < layers[1].num_Neurons; ++i)
			{
				double derivative = (1 - layers[1].neurons[i].output) * (1 + layers[1].neurons[i].output); // derivative of tanh = (1 - y) * (1 + y)
				double sum = 0.0;
				for (int j = 0; j < layers[2].num_Neurons; ++j) // each hidden delta is the sum of numOutput terms
				{
					double x = oGradTerms[j] * layers[1].neurons[i].weight[j];
					sum += x;
				}
				hGradTerms[i] = derivative * sum;
			}

			// add input to h-o component to make h-o weight gradients, and accumulate
			for (int i = 0; i < layers[1].num_Neurons; ++i)
			{
				for (int j = 0; j < layers[2].num_Neurons; ++j)
				{
					double grad = oGradTerms[j] * layers[1].neurons[i].output;
					hoWeightGradsAcc[i][j] += grad;
				}
			}

			// the (hidden-to-) output bias gradients
			for (int i = 0; i < layers[2].num_Neurons; ++i)
			{
				double grad = oGradTerms[i] * 1.0; // dummy input
				oBiasGradsAcc[i] += grad;
			}

			// add input term to i-h component to make i-h weight gradients and accumulate
			for (int i = 0; i < layers[0].num_Neurons; ++i)
			{
				for (int j = 0; j < layers[1].num_Neurons; ++j)
				{
					double grad = hGradTerms[j] * layers[0].neurons[i].output;
					ihWeightGradsAcc[i][j] += grad;
				}
			}

			// the (input-to-) hidden bias gradient
			for (int i = 0; i < layers[1].num_Neurons; ++i)
			{
				double grad = hGradTerms[i] * 1.0;
				hBiasGradsAcc[i] += grad;
			}
		} // each row
		  // end compute all gradients

		  // update all weights and biases (in any order)

		  // update input-hidden weights
		double delta = 0.0;

		for (int i = 0; i < layers[0].num_Neurons; ++i)
		{
			for (int j = 0; j < layers[1].num_Neurons; ++j)
			{
				if (ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j] > 0) // no sign change, increase delta
				{
					delta = ihPrevWeightDeltas[i][j] * etaPlus; // compute delta
					if (delta > deltaMax) delta = deltaMax; // keep it in range
					double tmp = -sgn(ihWeightGradsAcc[i][j]) * delta; // determine direction and magnitude
					layers[1].neurons[j].weight[i] += tmp; // update weights
				}
				else if (ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j] < 0) // grad changed sign, decrease delta
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
					double tmp = -sgn(ihWeightGradsAcc[i][j]) * delta; // determine direction
					layers[1].neurons[j].weight[i] += tmp; // update
				}
				//Console.WriteLine(ihPrevWeightGradsAcc[i][j] + " " + ihWeightGradsAcc[i][j]); Console.ReadLine();

				ihPrevWeightDeltas[i][j] = delta; // save delta
				ihPrevWeightGradsAcc[i][j] = ihWeightGradsAcc[i][j]; // save the (accumulated) gradient
			} // j
		} // i

		  // update (input-to-) hidden biases
		for (int i = 0; i < layers[1].num_Neurons; ++i)
		{
			if (hPrevBiasGradsAcc[i] * hBiasGradsAcc[i] > 0) // no sign change, increase delta
			{
				delta = hPrevBiasDeltas[i] * etaPlus; // compute delta
				if (delta > deltaMax) delta = deltaMax;
				double tmp = -sgn(hBiasGradsAcc[i]) * delta; // determine direction
				layers[1].neurons[i].bias += tmp; // update
			}
			else if (hPrevBiasGradsAcc[i] * hBiasGradsAcc[i] < 0) // grad changed sign, decrease delta
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
				double tmp = -sgn(hBiasGradsAcc[i]) * delta; // determine direction
				layers[1].neurons[i].bias += tmp; // update
			}
			hPrevBiasDeltas[i] = delta;
			hPrevBiasGradsAcc[i] = hBiasGradsAcc[i];
		}

		// update hidden-to-output weights
		for (int i = 0; i < layers[1].num_Neurons; ++i)
		{
			for (int j = 0; j < layers[2].num_Neurons; ++j)
			{
				if (hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j] > 0) // no sign change, increase delta
				{
					delta = hoPrevWeightDeltas[i][j] * etaPlus; // compute delta
					if (delta > deltaMax) delta = deltaMax;
					double tmp = -sgn(hoWeightGradsAcc[i][j]) * delta; // determine direction
					layers[2].neurons[j].weight[i] += tmp; // update
				}
				else if (hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j] < 0) // grad changed sign, decrease delta
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
					double tmp = -sgn(hoWeightGradsAcc[i][j]) * delta; // determine direction
					layers[2].neurons[j].weight[i] += tmp; // update
				}
				hoPrevWeightDeltas[i][j] = delta; // save delta
				hoPrevWeightGradsAcc[i][j] = hoWeightGradsAcc[i][j]; // save the (accumulated) gradients
			} // j
		} // i

		  // update (hidden-to-) output biases
		for (int i = 0; i < layers[2].num_Neurons; ++i)
		{
			if (oPrevBiasGradsAcc[i] * oBiasGradsAcc[i] > 0) // no sign change, increase delta
			{
				delta = oPrevBiasDeltas[i] * etaPlus; // compute delta
				if (delta > deltaMax) delta = deltaMax;
				double tmp = -sgn(oBiasGradsAcc[i]) * delta; // determine direction
				layers[2].neurons[i].bias += tmp; // update
			}
			else if (oPrevBiasGradsAcc[i] * oBiasGradsAcc[i] < 0) // grad changed sign, decrease delta
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
				double tmp = -sgn(hBiasGradsAcc[i]) * delta; // determine direction
				layers[2].neurons[i].bias += tmp; // update
			}
			oPrevBiasDeltas[i] = delta;
			oPrevBiasGradsAcc[i] = oBiasGradsAcc[i];
		}
	} // while

	//double* wts = GetWeights();
	//return wts;

	return 0;
}

int RProp::Test(const char* fname)
{
	
}

double RProp::Accuracy(double** testData, double* weights)
{
	// no need to set the weights as they are aleady set in the layers variable
	//this.SetWeights(weights);
	// percentage correct using winner-takes all
	int numCorrect = 0;
	int numWrong = 0;
	double* xValues = new double[layers[0].num_Neurons]; // inputs
	double* tValues = new double[layers[2].num_Neurons]; // targets
	double* yValues; // computed Y

	for (int i = 0; i < testData.Length; ++i)
	{
		copy_array_noindex(testData[i], xValues, layers[0].num_Neurons); // parse data into x-values and t-values
		copy_array_index(testData[i], layers[0].num_Neurons, tValues, 0, layers[2].num_Neurons);
		yValues = ComputeOutputs(xValues);
		int maxIndex = MaxIndex(yValues, /*test data length?*/); // which cell in yValues has largest value?

		if (tValues[maxIndex] == 1.0) // ugly. consider AreEqual(double x, double y, double epsilon)
			++numCorrect;
		else
			++numWrong;
	}
	return (numCorrect * 1.0) / (numCorrect + numWrong); // ugly 2 - check for divide by zero
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

void RProp::ZeroOutMat(double** matrix)
{
	for (int r = 0; r < num_layers; r++)
	{
		for (int c = 0; c < layers[r].num_Neurons; c++)
		{
			matrix[r][c] = 0;
		}
	}
}

void RProp::ZeroOutArray(double* ary, int size) // helper for Train
{
	for (int i = 0; i < size; ++i)
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
	for (int i = 0; i < size; ++i)
		if (oSums[i] > max) max = oSums[i];

	// determine scaling factor -- sum of exp(each val - max)
	double scale = 0.0;
	for (int i = 0; i < size; ++i)
		scale += exp(oSums[i] - max);
		

	double* result = new double[size];
	for (int i = 0; i < size; ++i)
		result[i] = exp(oSums[i] - max) / scale;

	return result; // now scaled so that xi sum to 1.0
}

double RProp::MeanSquaredError(double** trainData, double* weights, int size)
{
	//SetWeights(weights); // copy the weights to evaluate in

	double* xValues = new double[layers[0].num_Neurons]; // inputs
	double* tValues = new double[layers[2].num_Neurons]; // targets
	double sumSquaredError = 0.0;
	for (int i = 0; i < size; ++i) // walk through each training data item
	{
		// following assumes data has all x-values first, followed by y-values!
		copy_array_noindex(trainData[i], xValues, layers[0].num_Neurons); // extract inputs
		copy_array_index(trainData[i], layers[0].num_Neurons, tValues, 0, layers[2].num_Neurons); // extract targets
		int sizeofy = 0;
		double* yValues = ComputeOutputs(xValues, sizeofy);
		for (int j = 0; j < sizeofy; ++j)
			sumSquaredError += ((yValues[j] - tValues[j]) * (yValues[j] - tValues[j]));
	}
	return sumSquaredError / size;
}

double* RProp::ComputeOutputs(double* xValues, int size)
{
	//double* hSums = new double[numHidden]; // hidden nodes sums scratch array
	//double* oSums = new double[numOutput]; // output nodes sums

	for (int i = 0; i < size; ++i) // copy x-values to inputs
		layers[0].neurons[i].output = xValues[i];
	// note: no need to copy x-values unless you implement a ToString and want to see them.
	// more efficient is to simply use the xValues[] directly.

	for (int j = 0; j < layers[1].num_Neurons; ++j)  // compute i-h sum of weights * inputs
		for (int i = 0; i < layers[0].num_Neurons; ++i)
		{
			double output = layers[0].neurons[i].output;
			double weight = layers[1].neurons[j].weight[i];
			layers[1].neurons[j].output += (output * weight); // note +=
		}
			

	for (int i = 0; i < layers[1].num_Neurons; ++i)  // add biases to input-to-hidden sums
		layers[1].neurons[i].output += layers[1].neurons[i].bias;

	for (int i = 0; i < layers[1].num_Neurons; ++i)   // apply activation
	{
		double output = layers[1].neurons[i].output;
		layers[1].neurons[i].output = HyperTan(output); // hard-coded
	}
		
	for (int j = 0; j < layers[2].num_Neurons; ++j)  // compute i-h sum of weights * inputs
		for (int i = 0; i < layers[1].num_Neurons; ++i)
		{
			double output = layers[1].neurons[i].output;
			double weight = layers[2].neurons[j].weight[i];
			layers[2].neurons[j].output += (output * weight); // note +=
		}

	for (int i = 0; i < layers[2].num_Neurons; ++i)  // add biases to input-to-hidden sums
		layers[2].neurons[i].output += layers[2].neurons[i].bias;

	double* valuesforsoftout = new double[layers[2].num_Neurons];
	for (int i = 0; i < layers[2].num_Neurons; i++)
	{
		valuesforsoftout[i] = layers[2].neurons[i].output;
	}


	double* softOut = Softmax(valuesforsoftout, layers[2].num_Neurons); // softmax activation does all outputs at once for efficiency
	//Array.Copy(softOut, outputs, softOut.Length);

	//double* retResult = new double[numOutput]; // could define a GetOutputs method instead
	//Array.Copy(this.outputs, retResult, retResult.Length);
	return softOut;
}


