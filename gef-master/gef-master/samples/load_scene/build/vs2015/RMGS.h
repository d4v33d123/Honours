#pragma once
#ifndef RMGS_H
#define RMGS_H

#include "NeuralNetwork.h"

class RMGS : public Network
{
public:

	RMGS(int nl, int npl[]);
	~RMGS();

	int Train(const char* fnames);
	int Test(const char* fname);
	int Evaluate();

	void Run(const char* fname, const int& maxiter);

private:

	void RandomWeights();
	void MBD(double** trainingData, int size, double** FirstHiddenOutput, double** MBDOutput);
	void GramSchmidt(double** hidden, double** output, int size, int currentLayer);
	double** fillTrainingData(const char* fname, int rows, int cols);


	void PropagateSignal();
	void ComputeOutputError(double* target);
	void BackPropagateError();
	void AdjustWeights();

	void Simulate(double* input, double* output, double* target, bool training);




};


#endif // !"RMGS_H"
