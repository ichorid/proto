#ifndef WRAPPERS_LINGELING_H_
#define WRAPPERS_LINGELING_H__
#include "common.h"
#include "wrappers/interface.h"
#include "core/SolverTypes.h"
#include "core/Solver.h"
extern "C"
{
	#include "solvers/lingeling/lglib.h"
}
using Minisat::lbool;

class LingelingWrapper: public SWrapper
{
public:
	//Methods
	void InitSolver(const Cnf& cnf);
	void AddUCs(const UnitClauseVector& uc_vector);
	void Solve(const UnitClauseVector& uc_vector = UnitClauseVector ());
	void SetWatchScansLimit(long long unsigned int scans_limit);
	SolverReport GetReport();
	UnitClauseVector GetSolution();
	~LingelingWrapper();
protected:
	//Members
	LGL* lgl_;
	SWState state;
	long long unsigned int scans_limit_ = 0;
	void addProblem(const Cnf& cnf);
};

#endif


