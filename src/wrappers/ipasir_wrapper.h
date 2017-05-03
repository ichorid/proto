#ifndef WRAPPERS_IPASIRWRAPPER_H_
#define WRAPPERS_IPASIRWRAPPER_H__
#include "common.h"
#include "wrappers/interface.h"
#include "core/SolverTypes.h"
#include "core/Solver.h"
using Minisat::lbool;

class IpasirWrapper: public SWrapper
{
public:
	//Methods
	void InitSolver(const Cnf& cnf);
	void AddUCs(const UnitClauseVector& uc_vector);
	void Solve(const UnitClauseVector& uc_vector = UnitClauseVector ());
	void SetWatchScansLimit(long long unsigned int scans_limit);
	SolverReport GetReport();
	UnitClauseVector GetSolution();
	IpasirWrapper();
	~IpasirWrapper();
protected:
	//Members
	void* solver_;
	SWState state;
	void addProblem(const Cnf& cnf);
	double solve_time_;
};

#endif
