#include "wrappers/lingeling.h"
extern "C"
{
	#include "lglib.h"
}
#include <iostream>

void LingelingWrapper::addProblem(const Cnf& cnf)
{
	for (auto cl: cnf)
	{
		for (auto lt: cl)
			lgladd (lgl_, lt);
		lgladd (lgl_, 0);
	}
}

void LingelingWrapper::InitSolver(const Cnf& cnf)
{
	lgl_ = lglinit ();
	addProblem(cnf);
}

void LingelingWrapper::Solve()
{
	int res = lglsat (lgl_);
	if (res == 10)
		state = SAT;
	else if (res == 20)
		state = UNSAT;
	else
		state = STOPPED;
}

void LingelingWrapper::AddUCs(const UnitClauseVector& uc_vector)
{
		for (auto lt: uc_vector)
		{
			lgladd (lgl_, lt);
			lgladd (lgl_, 0);
		}
}

SolverReport LingelingWrapper::GetReport()
{
	SolverReport out;
	out.state = state;
	out.watch_scans = lglvisits(lgl_);
	return out;
}

UnitClauseVector LingelingWrapper::GetSolution()
{
	UnitClauseVector out;
	//assert(S.nVars()==S.model.size());
	int maxvar = lglmaxvar (lgl_);
	for (int i = 1; i <= maxvar; i++)
	{
		Lit lit = (lglderef (lgl_, i) > 0) ? i : -i;
		out.push_back(lit);
	}

	lglstats(lgl_);
	return out;
}
void LingelingWrapper::SetWatchScansLimit(long long unsigned int scans_limit)
{
	lglsetopt(lgl_,"plim" ,scans_limit);
	//lglsetopt(lgl_,"verbose" ,5);
}
LingelingWrapper::~LingelingWrapper(void)
{
	lglrelease (lgl_);
}

