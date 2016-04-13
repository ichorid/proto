#include "../src/utils.h"
#include <iostream>


int main(int argc, char* argv[])
{
	{
		const char* filename="cnf/hanoi4.cnf";
		Cnf cnf;
		ReadCnfFile(filename, cnf);
		std::cout << std::endl;
		for (auto cl: cnf){
			for (auto lit: cl){
				std::cout << int(lit)<< " ";
			}
			std::cout << std::endl;
		}
	}
	std::cout << " Layers read test" << std::endl;
	{
		const char* filename="cnf/Bivium.cnf";
		Cnf cnf;
		std::vector < std::vector <int> > var_layers;
		ReadCnfFile(filename, cnf, var_layers);
		std::cout << std::endl;
		for (auto cl: cnf){
			for (auto lit: cl){
				std::cout << int(lit)<< " ";
			}
			std::cout << std::endl;
		}
		std::cout << "Layers:" << std::endl;
		for (auto cl: var_layers){
			for (auto lit: cl){
				std::cout << int(lit)<< " ";
			}
			std::cout << std::endl;
		}
	}
}

