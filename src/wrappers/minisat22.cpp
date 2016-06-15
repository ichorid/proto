#include "wrappers/minisat22.h"
#include <iostream>
#include <random>
#include <mpi.h>

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
	int mpi_rank; MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);
	S.random_seed = (mpi_rank+1);
	addProblem(cnf);
}

void Minisat22Wrapper::Solve(const UnitClauseVector& uc_vector)
{
	Minisat::vec <Minisat::Lit> assums;
	for (auto uc: uc_vector)
		assums.push(Minisat::mkLit(var(uc)-1, uc<0));
	
	const lbool result = S.solveLimited(assums);

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
	out.watch_scans = S.watch_scans;
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

