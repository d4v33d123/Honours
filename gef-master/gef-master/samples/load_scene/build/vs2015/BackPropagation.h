#pragma once
#ifndef BPROP_H
#define BPROP_H

#include "NeuralNetwork.h"

class BProp : public Network
{
public:

	BProp(int nl, int npl[]);
	~BProp();

	int Train(const char* fnames, int datasize, double** TrainDat);
	double ** fillTrainingData(const char * fname, int rows, int cols);
	int Test(const char* fname, int datasize, double** TrainDat);
	int Evaluate();
	void PropagateSignal();

	void Run(const char* fname, int datasize,const int& maxiter);

private:

	void RandomWeights();

	
	void ComputeOutputError(double* target);
	void BackPropagateError();
	void AdjustWeights();

	void Simulate(double* input, double* output, double* target, bool training);




};

#endif
