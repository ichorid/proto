#include "wrappers/lingeling.h"
extern "C"
{
	#include "lglib.h"
	#include <signal.h>
}
#include <iostream>

static LGL * lgl4sigh;
static void (*sig_alrm_handler)(int);
static int caughtalarm = 0;
static void catchalrm (int sig) {
  assert (sig == SIGALRM);
  if (!caughtalarm) {
    caughtalarm = 1;
  }
}

void LingelingWrapper::addProblem(const Cnf& cnf)
{
	for (auto cl: cnf)
	{
		for (auto lt: cl)
			lgladd (lgl_, lt);
		lgladd (lgl_, 0);
	}
}

static int checkalarm (void * ptr) {
  assert (ptr == (void*) &caughtalarm);
  return caughtalarm;
}
void LingelingWrapper::InitSolver(const Cnf& cnf)
{
	lgl_ = lglinit ();
	addProblem(cnf);
}

void LingelingWrapper::Solve(const UnitClauseVector& uc_vector)
{
	// FIXME: lglfreeze all vars
	//      !!!WARNING!!!
	// Non-frozen/non-assumed vars become UNUSABLE after call to lglsolve/simp !
	for (auto lit: uc_vector)
		lglassume (lgl_, lit);
	lglseterm (lgl_, checkalarm, &caughtalarm);
	sig_alrm_handler = signal (SIGALRM, catchalrm);
	alarm (scans_limit_);
	int res = lglsat (lgl_);
	//FIXME: Dirty workaround for bad signal handling!
	if (!caughtalarm) pause();
	caughtalarm = 0;
	(void) signal (SIGALRM, sig_alrm_handler);
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
	//out.watch_scans = lglvisits(lgl_);
	out.watch_scans = 1+1000*lglsearchtime(lgl_);
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
	//lglsetopt(lgl_,"plim" ,scans_limit);
	//lglsetopt(lgl_,"verbose" ,5);
	scans_limit_ = scans_limit;
}
LingelingWrapper::~LingelingWrapper(void)
{
	lglrelease (lgl_);
}

