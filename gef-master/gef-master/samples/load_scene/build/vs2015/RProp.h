#pragma once
#ifndef RPROP_H
#define RPROP_H

#include "NeuralNetwork.h"
#include <stdio.h>

class RProp : public Network
{
public:

	RProp(int nl, int npl[]);
	~RProp();

	int Train(const char* fnames, int trainDataSize, int numInAndOut);
	int Test(const char* fname);

	void Accuracy(const char* fnames, int trainDataSize);
	int Evaluate();
	void PropagateSignal();

	void Run(const char* fname, const int& maxiter);

private:

	void RandomWeights();

	

	void AdjustWeights();

	void Simulate(double* input, double* output, double* target, bool training);

	void ComputeOutputs(double* xValues, int size, double* outs);


	void ZeroOutArray(double* ary, int size);

	void ZeroOutMat(double** matrix, int rows, int cols);

	double** fillTrainingData(const char* fname, int rows, int cols);

	double * GetWeights();

	double MeanSquaredError(double** trainData, int size);

};


#endif // !RPROP_H


