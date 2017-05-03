#include "wrappers/ipasir_wrapper.h"
#include "wrappers/ipasir.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
//#include "easylogging++.h"

static double cpuTime(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
}

static int maxvar = 0;
static bool stop_signal = 0;
static void catch_SIGVTALARM(int signum) { stop_signal = 1;}
static int checkStopSignal (void * state) {return stop_signal;}

void IpasirWrapper::SetWatchScansLimit(long long unsigned int scans_limit)
{
	;
}

static void SetTimeLim(int cpu_lim)
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

static void RemoveTimeLim()
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
	stop_signal = 0;
}
IpasirWrapper::IpasirWrapper()
{
	solver_ = ipasir_init();
	ipasir_set_terminate(solver_, NULL, checkStopSignal);
}

void IpasirWrapper::InitSolver(const Cnf& cnf)
{
	addProblem(cnf);
}

void IpasirWrapper::addProblem(const Cnf& cnf)
{
	for (auto cl: cnf)
	{
		for (auto lt: cl)
			ipasir_add(solver_, lt);
		ipasir_add(solver_, 0);
	}
}

void IpasirWrapper::AddUCs(const UnitClauseVector& uc_vector)
{
		for (auto lt: uc_vector)
		{
			// Quickhack!
			if (abs(lt)>maxvar)
				maxvar = abs(lt);
			ipasir_add (solver_, lt);
			ipasir_add (solver_, 0);
		}
}

void IpasirWrapper::Solve(const UnitClauseVector& uc_vector)
{
	for (auto lit: uc_vector)
		ipasir_assume(solver_, lit);


	SetTimeLim(seconds_limit_);
	double start_time = cpuTime();
	int res = ipasir_solve (solver_);
	solve_time_ = cpuTime() - start_time;
	RemoveTimeLim();

	if (res == 10)
		state = SAT;
	else if (res == 20)
		state = UNSAT;
	else
		state = STOPPED;
}

UnitClauseVector IpasirWrapper::GetSolution()
{
	UnitClauseVector out;
	for (int i = 1; i <= maxvar; i++)
	{
		Lit lit = (ipasir_val (solver_, i) > 0) ? i : -i;
		out.push_back(lit);
	}
	return out;
}

SolverReport IpasirWrapper::GetReport()
{
	SolverReport out;
	out.state = state;
	out.watch_scans = 1+1000*solve_time_;
	return out;
}
IpasirWrapper::~IpasirWrapper()
{
	ipasir_release(solver_);
}
