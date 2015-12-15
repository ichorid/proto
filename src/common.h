#ifndef COMMON_H_
#define COMMON_H_
#include <vector>
#include "assert.h"
#include <string>

typedef int Lit;
typedef int Var;
typedef std::vector <Lit> Clause;
typedef Clause UnitClauseVector;
typedef std::vector <UnitClauseVector> Sample;
typedef std::vector <UnitClauseVector> Task;
typedef UnitClauseVector Assignment; // TODO: move me to separate file!
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
	int watch_scans;
} SolverReport;
typedef std::vector <SolverReport> Results;

inline int var(Lit l) { return (l>=0?l:(-l));} //inline int var(Lit l) { unsigned int t = l>> 31; l^= t; l+= t & 1; return int(t);}
inline char FlipBit(char& bit){ return  bit^=1;}

typedef std::string PointId;
typedef std::string BitMask;
typedef struct PointStats
{
	PointId  point_id;
	long unsigned int sample_size;
	long unsigned int sat_total;
	long int best_cutoff; // best scans limit
	double   best_incapacity; 
} PointStats;

// Some common UC manipulation functions

inline BitMask BM_or(const BitMask& a, const BitMask& b)
{
	bool ab = a.size() > b.size();
	auto out = ab ? a : b;
	for (int i =0; i< (ab ? b.size() : a.size()); ++i)
		out[i] = a[i] | b[i];
	return out;
}

inline UnitClauseVector MaskUCVector (const BitMask& mask, const UnitClauseVector& ucv)
{
	assert(ucv.size()>0);
	// It is OK if mask is shorter than uc vector.
	UnitClauseVector out;
	for (int i=0; i<ucv.size(); ++i)
		if (i<=mask.size() && mask[i]==1)
			out.push_back(ucv[i]);
	return out;
}

inline Task GenTask(const BitMask& mask, const Sample& sample)
{
	assert(sample.size()>0);
	assert(sample[0].size()>=mask.size());
	Task out;
	for(auto solution_ucv: sample)
		out.push_back( MaskUCVector(mask, solution_ucv) );
	return out;
}

inline int CountOnes(const std::string& str)
{
	int out=0;
	for (auto ch: str)
		out+=(ch==1);
	return out;
}
/*
std::vector<PointId> GetHammingNbhd (PointId point)
{
	std::vector<PointId>  result;
	for (int i =0; i<point.size(); ++i){
		result.push_back(point);
		FlipBit(result[i][i]);
	}
	return result;
}
*/

#endif
