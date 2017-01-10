#ifndef NETWORK_H
#define NETWORK_H


struct Neuron
{
	double* weight; // weight
	double output; // position in layer
	double error; // current error
	double* pre_Weight; // previous weight for momentum
	double* saved_weight; 
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


	Network(int nl, int npl[]);
	~Network();

	int Train(const char* fnames);
	int Test(const char* fname);
	int Evaluate();

	void TrainRMGS(const char* fname);

	void Run(const char* fname, const int& maxiter);

private:

	int num_layers;
	Layer* layers;

	double dMSE;
	double dMAE;

	void RandomWeights(); 

	void SetInputSignal(double* input);
	void GetOutputSignal(double* output);

	void SaveWeights();
	void RestoreWeights();

	void PropagateSignal();
	void ComputeOutputError(double* target);
	void BackPropagateError();
	void AdjustWeights();

	void Simulate(double* input, double* output, double* target, bool training);




};

#endif