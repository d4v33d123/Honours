#pragma once
#ifndef NETWORK_H
#define NETWORK_H


#include <stdio.h>
#include <stdlib.h>

struct Neuron
{
	double* weight; // weight
	double output; // position in layer
	double error; // current error
	double* pre_Weight; // previous weight for momentum
	double* saved_weight;
	double bias; // a bias for the current neuron
};

struct Layer
{
	int num_Neurons;
	Neuron* neurons;
};

class Network
{
public:

	double dEta;
	double dAlpha;
	double dGain;
	double dAvgTestError;


	//Network(int nl, int npl[]);
	//~Network();

	virtual int Train(const char* fnames) { return 0; }
	virtual int Test(const char* fname) { return 0; }
	virtual int Evaluate() { return 0; }

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

	bool read_number(FILE* fp, double* number);

	void InititaliseRandoms();
	int RandomEqualINT(int Low, int High);
	double RandomEqualREAL(double Low, double High);
	double** MakeMatrix(int rows, int cols, double v);
	double*	MakeVector(int len, double v);
	double* MinusVectors(double* Vec1, double* Vec2, int size);
	double DotProduct(double* Vec1, double* Vec2, int size);
	double* MultiplyVector(double* Vec, double value, int size);

	virtual void PropagateSignal() {}
	virtual void ComputeOutputError(double* target) {}
	virtual void AdjustWeights() {}

	virtual void Simulate(double* input, double* output, double* target, bool training) {}




};

#endif