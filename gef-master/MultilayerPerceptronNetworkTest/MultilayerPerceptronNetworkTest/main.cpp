#include "Network.h"

int main()
{
	int layers2[] = { 1, 5, 1 };
	Network mlp2(3, layers2);
	mlp2.Run("sin.dat", 300);
	return 0;
}