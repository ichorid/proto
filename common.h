#ifndef PROTO_COMMON_H_
#define PROTO_COMMON_H_
#include <vector>

typedef int Lit;
typedef int Var;
typedef std::vector <Lit> Clause;
typedef Clause UnitClauseVector;
typedef std::vector <UnitClauseVector> Sample;
typedef std::vector <UnitClauseVector> Task;
typedef std::vector <Clause> Cnf;
typedef enum {
	UNINITIALIZED,
	INITIALIZED,
	STOPPED,
	SAT,
	UNSAT,
	ERROR,
	WORKING
} SWState;
typedef struct SolverReport
{
	SWState state;
	long long int watch_scans;
} SolverReport;
typedef std::vector <SolverReport> Results;

//inline int var(Lit l) { unsigned int t = l>> 31; l^= t; l+= t & 1; return int(t);}
inline int var(Lit l) { return (l>=0?l:(-l));}


#endif
