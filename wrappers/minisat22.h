#ifndef PROTO_WRAPPERS_MINISAT22_H_
#define PROTO_WRAPPERS_MINISAT22_H_
#include "common.h"
#include "wrappers/interface.h"
#include "solvers/minisat22/core/Solver.h"
#include "solvers/minisat22/core/SolverTypes.h"
#include "solvers/minisat22/core/Solver.h"
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
protected:
	//Members
	Minisat::Solver S;
	SWState state;
	void addProblem(const Cnf& cnf);
};

#endif

