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

/*
	double dEta;
	double dAlpha;
	double dGain;
	double dAvgTestError;


	//Network(int nl, int npl[]);
	//~Network();

	virtual int Train(const char* fnames) {}
	virtual int Test(const char* fname) {}
	virtual int Evaluate() {}

	virtual void Run(const char* fname, const int& maxiter) {}

	int num_layers;
	Layer* layers;

	double dMSE;
	double dMAE;

	virtual void RandomWeights() {}

	void SetInputSignal(double* input);
	void GetOutputSignal(double* output);

	void SaveWeights();
	void RestoreWeights();

	virtual void PropagateSignal() {}
	virtual void ComputeOutputError(double* target) {}
	virtual void AdjustWeights() {}

	virtual void Simulate(double* input, double* output, double* target, bool training) {}
*/