#include <iostream>

#include "Framework.h";


int main()
{
	Framework* framework = new Framework();

	delete framework;

	std::printf("framework deleted, press key followed by enter to exit\n");

	int kk = 0;
	std::cin >> kk;

	return 0;
}