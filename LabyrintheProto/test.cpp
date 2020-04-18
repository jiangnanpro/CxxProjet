#include <time.h>
#include <stdlib.h>
#include <iostream>

int main(int argc, char const *argv[])
{
	srand((unsigned int)(time(NULL)));

	int f[7];
	for (int i = 0; i < 7; ++i)
	{
		f[i] = i;
	}


	for (int i = 0; i < 10; ++i)
	{
		std::cout << rand()%2 << std::endl;
 	}
	return 0;
}