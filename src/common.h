#ifndef COMMON_H_
#define COMMON_H_
#include <vector>
#include "assert.h"
#include <string>
#include <algorithm>

typedef int Lit;
typedef int Var;
typedef std::vector <Lit> Clause;
typedef Clause UnitClauseVector;
typedef std::vector <UnitClauseVector> Sample;

typedef UnitClauseVector Assignment; // TODO: move me to separate file!
typedef std::vector <Clause> Cnf;
typedef std::string PointId;
typedef std::string BitMask;
typedef enum
{
	UNINITIALIZED,
	INITIALIZED,
	STOPPED,
	SAT,
	UNSAT,
	ERROR,
	WORKING
} SWState;

struct Task
{
	PointId id = "";
	std::vector <UnitClauseVector> units;
};

struct SolverReport
{
	SWState state = UNINITIALIZED;
	uint64_t watch_scans = 0;
};

struct PointResults
{
	PointId id = "";
	std::vector <SolverReport> reps;
};

struct PointStats
{
	PointId  id = "";
	uint64_t sample_size = 0;
	uint64_t sat_total = 0;
	uint32_t best_cutoff = 0; // best scans limit
	double   best_incapacity = std::numeric_limits<double>::infinity(); 
};

typedef PointStats (*FitnessFunction)(const PointResults& results);

inline Var var(Lit l) { return (l >= 0 ? l : (-l));} //inline int var(Lit l) { unsigned int t = l>> 31; l^= t; l+= t & 1; return int(t);}
inline char FlipBit(char& bit){ return  bit ^= 1;}

// Some common UC manipulation functions
inline BitMask BM_or(const BitMask& a, const BitMask& b)
{
	bool ab = a.size() > b.size();
	auto out = ab ? a : b;
	for (int i = 0; i < (ab ? b.size() : a.size()); ++i)
		out[i] = a[i] | b[i];
	return out;
}

inline BitMask BM_xor(const BitMask& a, const BitMask& b)
{
	bool ab = a.size() > b.size();
	auto out = ab ? a : b;
	for (int i = 0; i < (ab ? b.size() : a.size()); ++i)
		out[i] = a[i] ^ b[i];
	return out;
}
/*
inline std::vector<BitMask> BM_or(const std::vector<BitMask>& a, const BitMask& b)
{
	std::vector<BitMask> out;
	for (auto m: a)
		out.push_back(BM_or(m, b));
	return out;
}
*/

inline BitMask ExpandBM(const BitMask& m, const std::vector <int>& v)
{
	assert(m.size() == v.size());
	const int out_size = *std::max_element(v.begin(), v.end());
	BitMask out(out_size, 0);
	for (int i = 0; i < m.size(); ++i)
		out[v[i]-1] = m[i];
	return out;
}

inline std::vector <int> RemoveIntsFromVectorByBitMask (const std::vector <int>& vec, const BitMask& mask)
{
	std::vector <int> out;
	for (auto v: vec)
		if (!mask[v])
			out.push_back(v);
	return out;
}



//TODO: reimplement this stuff as templates
/*
inline std::vector<BitMask> ExpandBM(const std::vector<BitMask>& a, const std::vector <int>& v)
{
	std::vector<BitMask> out;
	for (auto m: a)
	{
		out.push_back(ExpandBM(m, v));
	}
	return out;
}
*/

inline UnitClauseVector MaskUCVector (const BitMask& mask, const UnitClauseVector& ucv)
{
	assert(ucv.size() > 0);
	// It is OK if mask is shorter than uc vector.
	UnitClauseVector out;
	for (int i = 0; i < ucv.size(); ++i)
		if (i <= mask.size() && mask[i] == 1)
			out.push_back(ucv[i]);
	return out;
}

inline std::vector <UnitClauseVector> GenTaskUnits(const BitMask& mask, const Sample& sample)
{
	assert(sample.size() > 0);
	assert(sample[0].size() >= mask.size());
	std::vector <UnitClauseVector> out;
	for(auto solution_ucv: sample)
		out.push_back(MaskUCVector(mask, solution_ucv));
	return out;
}

/*
inline Task GenTask(const std::vector <BitMask>& masks_vec, const Sample& sample)
{
	assert(sample.size()>0);
	//assert(sample[0].size()>=mask[0].size());
	Task out
	for(auto mask: masks_vec)
		for(auto solution_ucv: sample)
			out.push_back( MaskUCVector(mask, solution_ucv) );
	return out;
}
*/

inline int CountSymbol(const std::string& str, const char s)
{
	int out = 0;
	for (auto ch: str)
		out += (ch == s);
	return out;
}
inline int CountOnes(const std::string& str)   { return CountSymbol(str, 1); }
inline int CountZeroes(const std::string& str) { return CountSymbol(str, 0); }
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
