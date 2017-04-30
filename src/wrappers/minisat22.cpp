#include "wrappers/minisat22.h"
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <execinfo.h>
#include <signal.h>


Minisat::Solver *pS;

void catch_SIGVTALARM(int signum) { pS->interrupt(); }

// TODO: move me to separate lib!
// Shameless copypaste from Minisat source!
double cpuTime(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
}

void SetTimeLim(int cpu_lim)
{
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &catch_SIGVTALARM;
	sigaction (SIGVTALRM, &sa, NULL);

	struct itimerval timer;
	timer.it_value.tv_sec = cpu_lim;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	setitimer (ITIMER_VIRTUAL, &timer, NULL);
}

void RemoveTimeLim()
{
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	setitimer (ITIMER_VIRTUAL, &timer, NULL);


	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = SIG_DFL;
	sigaction (SIGVTALRM, &sa, NULL);
}

Minisat22Wrapper::Minisat22Wrapper()
{
	// get a pointer to the actual Solver object, so the signal
	// handler could call the interrupt() method
	pS = &S;
}
//TODO: make Lit to MinisatLit function
void Minisat22Wrapper::addProblem(const Cnf& cnf)
{
	// ACHTUNG! Minisat uses inverted signs in  its lits representation
	// e.g. lit -15 should be created as mkLit(15, true)
	assert(S.nVars()==0);
	for (auto cl: cnf){
		Minisat::vec <Minisat::Lit> new_clause;
		for (auto lt: cl){
			while (var(lt)-1>=S.nVars()){
			       S.newVar();
			}
			new_clause.push(Minisat::mkLit(var(lt)-1, lt<0));
		}
		S.addClause(new_clause);
	}
}

void Minisat22Wrapper::InitSolver(const Cnf& cnf)
{
	addProblem(cnf);
}

void Minisat22Wrapper::Solve(const UnitClauseVector& uc_vector)
{
	Minisat::vec <Minisat::Lit> assums;
	for (auto uc: uc_vector)
		assums.push(Minisat::mkLit(var(uc)-1, uc<0));

	if (seconds_limit_ > 0)
		SetTimeLim(seconds_limit_);
	double start_time = cpuTime();
	const lbool result = S.solveLimited(assums);
	solve_time_ = cpuTime() - start_time;
	
	if (seconds_limit_ > 0)
		RemoveTimeLim();

	if (result == l_Undef)
		state = STOPPED;
	else if (result == l_True)
		state = SAT;
	else if (result == l_False)
		state = UNSAT;
}

void Minisat22Wrapper::AddUCs(const UnitClauseVector& uc_vector)
{
	// ACHTUNG! Minisat uses inverted signs in  its lits representation
	// e.g. lit -15 should be created as mkLit(15, true)
	for (auto uc: uc_vector){
		S.addClause(Minisat::mkLit(var(uc)-1, uc<0));
	}
}

void Minisat22Wrapper::SetWatchScansLimit(long long unsigned int scans_limit)
{
	S.setScansBudget(scans_limit);
}

SolverReport Minisat22Wrapper::GetReport()
{
	SolverReport out;
	out.state = state;
	// milliseconds
	out.watch_scans = seconds_limit_>0 ? 1000*solve_time_ : S.watch_scans;
	if (out.watch_scans == 0)
		out.watch_scans = 1;
	return out;
}

UnitClauseVector Minisat22Wrapper::GetSolution()
{
	UnitClauseVector out;
	assert(S.nVars()==S.model.size());
	for (int i=0; i<S.nVars(); ++i){
		Lit lit = (i+1) * (1-2*(S.model[i]==l_False));
		out.push_back(lit);
	}
	return out;
}

void Minisat22Wrapper::Randomize(const int rnd)
{
	S.rnd_pol = true;
	S.random_seed = double(rnd);
}

