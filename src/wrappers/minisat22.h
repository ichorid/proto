#ifndef WRAPPERS_MINISAT22_H_
#define WRAPPERS_MINISAT22_H_
#include "common.h"
#include "wrappers/interface.h"
#include "core/SolverTypes.h"
#include "core/Solver.h"
using Minisat::lbool;

class Minisat22Wrapper : public SWrapper
{
public:
	//Methods
	void InitSolver(const Cnf& cnf);
	void AddUCs(const UnitClauseVector& uc_vector);
	void Solve(const UnitClauseVector& uc_vector = UnitClauseVector ());
	void SetWatchScansLimit(long long unsigned int scans_limit);
	SolverReport GetReport();
	UnitClauseVector GetSolution();
	void Randomize(const int rnd);
	Minisat22Wrapper();
protected:
	//Members
	Minisat::Solver S;
	SWState state;
	int solve_time_ = 0;
	void addProblem(const Cnf& cnf);
};

#endif

