#ifndef WRAPPERS_INTERFACE_H_
#define WRAPPERS_INTERFACE_H_
#include "common.h"

//Universal SAT solver wrapper interface definition
class SWrapper
{
public:
	//Methods
	virtual void InitSolver(const Cnf& cnf) = 0;
	virtual void AddUCs(const UnitClauseVector& uc_vector) = 0;
	virtual void Solve() = 0;
	virtual void SetWatchScansLimit(long long unsigned int scans_limit) = 0;
	virtual SolverReport GetReport() = 0;
	virtual UnitClauseVector GetSolution() = 0;
	virtual ~SWrapper(){};
};


#endif
