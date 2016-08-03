#ifndef COMMON_H_
#define COMMON_H_
#include <vector>
#include "assert.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <unordered_set>

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
template <typename T> T BM_or(const T& a, const T& b)
{
	bool ab = a.size() > b.size();
	auto out = ab ? a : b;
	for (int i = 0; i < (ab ? b.size() : a.size()); ++i)
		out[i] = a[i] | b[i];
	return out;
}

template <typename T> T BM_xor(const T& a, const T& b)
{
	bool ab = a.size() > b.size();
	auto out = ab ? a : b;
	for (int i = 0; i < (ab ? b.size() : a.size()); ++i)
		out[i] = a[i] ^ b[i];
	return out;
}

template <typename T> T BM_sub(const T& a, const T& b)
{
	assert (a.size() >= b.size());
	auto out = a;
	for (int i = 0; i < a.size(); ++i)
		out[i] = a[i] & ~b[i];
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
		if (!mask[v-1])
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
inline std::string Point2Bitstring(const PointId& p) { std::string out; for (int i=0; i<p.size(); ++i) out+= (p[i]==0 ? "0" : "1") ; return out;}
inline std::string Point2Varstring(const PointId& p) { std::string out; for (int i=0; i<p.size(); ++i) out+= (p[i]==0 ? "" : " "+std::to_string(i+1)); return out;}
//template <typename T> std::string Vec2String(const std::vector <T> & vec) { std::string out; for (T elem: vec) {out+= (std::to_string(elem) + " ");} return out;}
template <typename T> std::string Vec2String(const T & vec, const char* s = "") { std::string out; for (auto elem: vec) {out+= (std::to_string(elem) + s);} return out;}

inline std::string PrintPointStats(const PointStats& ps, std::vector <int> guessing_vars = std::vector <int> ())
{
	if (guessing_vars.size() == 0)
		for (int i = 0; i < ps.id.size(); ++i)
			guessing_vars.push_back(i+1);
	std::ostringstream ss;
	ss  << std::setw(5) << CountOnes(ps.id) << " "
	    << std::setw(8) << std::setprecision(2) << std::fixed << ps.best_incapacity << " "    
	    << std::setw(12) << std::scientific << pow(2.0, ps.best_incapacity) << " "    
	    << "W: " << std::setw(8) << std::scientific << ps.best_cutoff << " "    
	    << std::setw(5) << ps.sat_total << " /" 
	    << std::setw(5) << ps.sample_size << " " 
	    << Point2Bitstring (ps.id) << " ccc "
	    << Point2Varstring (ExpandBM (ps.id, guessing_vars)) ;
	return ss.str();

}

inline std::vector<PointId> HammingNbhd (const PointId& point, const char phase = 2)
{
	std::vector<PointId> result;
	for (int i = 0; i < point.size(); ++i)
	{
		PointId tmp = point;

		if (tmp[i] != phase)
		{
			FlipBit(tmp[i]);
			result.push_back(tmp);
		}
	}
	return result;
}

inline std::vector<PointId> PaletteNbhd (
		std::unordered_set <PointId>& palette,
	       	const PointId& point,
	       	const char phase = 2)
{
	std::vector<PointId> result;
	for (auto color: palette)
	{
		PointId tmp = point;

		if (phase==0)
			tmp = BM_sub (tmp, color);
		else if (phase==1)
			tmp = BM_or  (tmp, color);
		else 
			tmp = BM_xor (tmp, color);

		result.push_back(tmp);
	}
	return result;
}



inline PointId Ints2Point (std::vector <int>& ints, const std::vector <int>& guessing_vars)
{
	PointId point (guessing_vars.size(), 0);
	std::sort (ints.begin(), ints.end());
	int i = 0;
	for (auto v: ints)
	{
		while (v != guessing_vars[i])
			i++;
		point[i] = 1;
	}

	return point;
}
#endif
