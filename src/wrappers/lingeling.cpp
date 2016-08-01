#include "wrappers/lingeling.h"
extern "C"
{
	#include "lglib.h"
	#include <signal.h>
}
#include <iostream>
#include <sys/time.h>
#include <ctime>
static inline double cpuTime(void) {
    return (double)clock() / CLOCKS_PER_SEC; }

double start_time;
double time_limit;
static LGL * lgl4sigh;
static void (*sig_alrm_handler)(int);
static int caughtalarm = 0;
static void catchalrm (int sig) {
  assert (sig == SIGVTALRM);
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
  if ((cpuTime() - start_time) > time_limit )
	  caughtalarm = 1;
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

	start_time = cpuTime();
	time_limit = double( scans_limit_) / 1000000 ;

	caughtalarm = 0;
	lglseterm (lgl_, checkalarm, &caughtalarm);

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
	//out.watch_scans = lglvisits(lgl_);
	out.watch_scans = 1+1000000*lglsearchtime(lgl_);
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

