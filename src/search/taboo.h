#ifndef SEARCH_TABOO_H_
#define SEARCH_TABOO_H_
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include "common.h"

#define SAT_THRESH 2

// Comparator for point incapacity queue. Works via pointers.
class ComparePointIncapacity
{
	bool reverse;
public:
	
	ComparePointIncapacity(const bool revparam = false)
		: reverse(revparam)
	{}

	bool operator() (const PointStats* lhs, const PointStats* rhs) const
	{
		bool lhsGood = lhs->sat_total >= SAT_THRESH;
		bool rhsGood = rhs->sat_total >= SAT_THRESH;
		if (( lhsGood && rhsGood) || (!lhsGood && !rhsGood))
			if (reverse) 
				return (lhs->best_incapacity < rhs->best_incapacity);
			else
				return (lhs->best_incapacity > rhs->best_incapacity);

		if (reverse) 
			return (lhsGood);
		else
			return (rhsGood);
	}
};
typedef std::priority_queue <PointStats*, std::vector <PointStats*>, ComparePointIncapacity> BestIncapacityQueue;
typedef std::unordered_map <PointId, PointStats*> PointStatsDB;

class TabooSearch
{
public:
	TabooSearch (const int sat_threshold = 1);
	~TabooSearch ();
	//PointId ProcessPointResults (const PointId& point, const Results& results);

	/**
	* \brief Adds point evaluation results to points DB
	* \param results [in] Point evaluation results, inluding point ID
	*/
	void AddPointResults (const PointStats& ps);
	
	/**
	* \brief Returns current record point from point DB
	*/
	PointStats GetCurrentRecord ();
	void ResetCurrentRecord ();

	// Priority queue, sorts evaluated points by incapacity record.
	BestIncapacityQueue origin_queue_;

	// A "palette" of vars / var groups. Used to construct new points. 
	std::unordered_set <PointId> varPalette_;

	// Minimal required number of solved (SAT) problems in a sample
	int sat_threshold_ = 1;
	std::vector <PointId> GenerateNewPoints ( 
			const int desired_candidates = 1,
			const PointId& fixedVarsMask = PointId());
	std::vector <PointId> GenerateRandomPoints (
			const int num_ones,
			const int desired_candidates,
			const PointId& basePoint);
	inline PointStats GetPointStats (const PointId& id) { return *checked_points_[id];}
	std::vector <PointId> Checked (const std::vector<PointId> & vec);
	std::vector <PointId> Unchecked (const std::vector<PointId> & vec);
private:

	// Hash table. Contains all evaluated points and their stats
	PointStatsDB checked_points_;

	// A point holding the best incapacity record
	PointStats* global_record_ = NULL;

	std::mt19937 rng;

	//void LoadNewSample (int sample_size);

	// Service methods
	std::vector<PointId> GetUncheckedHammingNbhd (
			const PointId& point,
			const PointId& fixedVarsMask = PointId());
	std::vector<PointId> GetUncheckedPaletteNbhd(
			const PointId& point,
			const PointId& fixedVarsMask = PointId());
	inline bool PointChecked (const PointId& id) const {return checked_points_.count(id) > 0;}
};


#endif
