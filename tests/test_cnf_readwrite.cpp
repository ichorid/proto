#include "utils.h"
#include <iostream>


int main(int argc, char* argv[])
{
	const char* filename="cnf/hanoi4.cnf";
	Cnf cnf;
	ReadCNFile(filename, cnf);
	std::cout << std::endl;
	for (auto cl: cnf){
		for (auto lit: cl){
			std::cout << int(lit)<< " ";
		}
		std::cout << std::endl;
	}
}

