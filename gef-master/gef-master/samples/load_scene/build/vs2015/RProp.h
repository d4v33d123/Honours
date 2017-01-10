#pragma once
#ifndef RPROP_H
#define RPROP_H

#include "NeuralNetwork.h"

class RProp : public Network
{
public:

	RProp(int nl, int npl[]);
	~RProp();

	int Train(const char* fnames);
	int Test(const char* fname);
	int Evaluate();

	void Run(const char* fname, const int& maxiter);

private:

	void RandomWeights();

	void PropagateSignal();

	void AdjustWeights();

	void Simulate(double* input, double* output, double* target, bool training);

	double* ComputeOutputs(double* xValues, int size);

	void ZeroOutArray(double* ary, int size);

	void ZeroOutMat(double** matrix);

	double MeanSquaredError(double** trainData, double* weights, int size)

};


#endif // !RPROP_H


