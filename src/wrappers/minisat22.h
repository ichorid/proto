#ifndef WRAPPERS_MINISAT22_H_
#define WRAPPERS_MINISAT22_H_
#include "common.h"
#include "wrappers/interface.h"
#include "core/Solver.h"
#include "core/SolverTypes.h"
#include "core/Solver.h"
using Minisat::lbool;

class Minisat22Wrapper : public SWrapper
{
public:
	//Methods
	void InitSolver(const Cnf& cnf);
	void AddUCs(const UnitClauseVector& uc_vector);
	void Solve();
	void SetWatchScansLimit(long long unsigned int scans_limit);
	SolverReport GetReport();
	UnitClauseVector GetSolution();
protected:
	//Members
	Minisat::Solver S;
	SWState state;
	void addProblem(const Cnf& cnf);
};

#endif

