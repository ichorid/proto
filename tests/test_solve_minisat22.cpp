#include "utils.h"
#include "wrappers/minisat22.h"
#include <iostream>

int main(int argc, char* argv[])
{
	// test basic solve 
	{
		const char* filename="cnf/hanoi4.cnf";
		Cnf cnf;
		ReadCNFile(filename, cnf);
		Minisat22Wrapper WM;
		SWrapper &W = WM;
		W.InitSolver(cnf);

		W.Solve();
		const SolverReport rep = W.GetReport();
		if (rep.state==SAT){
			std::cout<< "SAT";
		}
		if (rep.state==UNSAT){
			std::cout<< "UNSAT";
		}
		if (rep.state==STOPPED){
			std::cout<< "Undef";
		}
		std::cout << std::endl;
	}

	// test solve with watch_scans limit
	{
		const char* filename="cnf/hanoi4.cnf";
		Cnf cnf;
		ReadCNFile(filename, cnf);
		Minisat22Wrapper WM;
		SWrapper &W = WM;
		W.InitSolver(cnf);
		W.SetWatchScansLimit(10000);

		W.Solve();
		const SolverReport rep = W.GetReport();
		if (rep.state==SAT){
			std::cout<< "SAT";
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
