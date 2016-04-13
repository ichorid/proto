#include "utils.h"
#include "wrappers/minisat22.h"
#include "wrappers/lingeling.h"
#include <iostream>

int main(int argc, char* argv[])
{
	// test basic solve 
	{
		const char* filename="../cnf/hanoi4.cnf";
		const char* filename_opt="../tests/ling_options.csv";
		Cnf cnf;
		ReadCnfFile(filename, cnf);
		LingelingWrapper WM;
		SWrapper &W = WM;
		W.InitSolver(cnf);
		W.PassOptions()

		W.Solve();

		const SolverReport rep = W.GetReport();
		if (rep.state==SAT){
			std::cout<< "SAT" << std::endl;
			for (auto lit: W.GetSolution())
				std::cout<< lit << std::endl;
		}
		if (rep.state==UNSAT){
			std::cout<< "UNSAT";
		}
		if (rep.state==STOPPED){
			std::cout<< "Undef";
		}
		std::cout << std::endl;
	}
}
