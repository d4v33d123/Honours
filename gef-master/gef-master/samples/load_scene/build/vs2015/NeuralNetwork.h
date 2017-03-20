#pragma once
#ifndef NETWORK_H
#define NETWORK_H


#include <stdio.h>
#include <stdlib.h>
#include "system\debug_log.h"

struct Neuron
{
	double* weight; // weight
	double output; // position in layer
	double error; // current error
	double* pre_Weight; // previous weight for momentum
	double* saved_weight;
	double bias; // a bias for the current neuron
	double savedBias;
};

struct Layer
{
	int num_Neurons;
	Neuron* neurons;
};

class Network
{
public:




	//Network(int nl, int npl[]);
	//~Network();

	virtual int Train(const char* fnames) { return 0; }
	virtual int Test(const char* fname) { return 0; }
	virtual int Evaluate() { return 0; }
	virtual void Run(const char* fname, const int& maxiter) {}
	virtual void PropagateSignal() {}
	virtual void ComputeOutputError(double* target) {}
	virtual void AdjustWeights() {}
	virtual void Simulate(double* input, double* output, double* target, bool training) {}
	virtual void RandomWeights() {}


	int num_layers;
	Layer* layers;

	double dMSE;
	double dMAE;
	double dEta;
	double dAlpha;
	double dGain;
	double dAvgTestError;


	void SetInputSignal(double* input);
	void GetOutputSignal(double* output);

	void SaveWeights();
	void RestoreWeights();

	bool read_number(FILE* fp, double* number);

	void InititaliseRandoms(int rnd);
	int RandomEqualINT(int Low, int High);
	double RandomEqualREAL(double Low, double High);
	double** MakeMatrix(int rows, int cols, double v);
	double*	MakeVector(int len, double v);
	double* MinusVectors(double* Vec1, double* Vec2, int size);
	double DotProduct(double* Vec1, double* Vec2, int size);
	double* MultiplyVector(double* Vec, double value, int size);




};

#endif