#include "wrappers/minisat22.h"
#include <iostream>

void Minisat22Wrapper::addProblem(const Cnf& cnf)
{
	for (auto cl: cnf){
		Minisat::vec <Minisat::Lit> new_clause;
		for (auto lt: cl){
			while (var(lt)-1>=S.nVars()){
			       S.newVar();
			}
			new_clause.push(Minisat::mkLit(var(lt)-1, lt>0));
		}
		S.addClause(new_clause);
	}
}

void Minisat22Wrapper::InitSolver(const Cnf& cnf)
{
	addProblem(cnf);
}

void Minisat22Wrapper::Solve()
{
	Minisat::vec <Minisat::Lit> empty_assums;
	const lbool result = S.solveLimited(empty_assums);

	if (result == l_Undef)
		state = STOPPED;
	else if (result == l_True)
		state = SAT;
	else if (result == l_False)
		state = UNSAT;
}

void Minisat22Wrapper::AddUCs(const UnitClauseVector& uc_vector)
{
	for (auto uc: uc_vector){
		S.addClause(Minisat::mkLit(var(uc)-1, uc>0));
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
	out.watch_scans = S.watch_scans;
	return out;
}

