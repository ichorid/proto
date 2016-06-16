#ifndef SEARCH_TABOO_H_
#define SEARCH_TABOO_H_
#include <random>
#include <unordered_map>
#include <vector>
#include <queue>
#include "common.h"

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
		if (reverse) 
			return (lhs->best_incapacity < rhs->best_incapacity);
		else
			return (lhs->best_incapacity > rhs->best_incapacity);
	}
};
typedef std::priority_queue <PointStats*, std::vector <PointStats*>, ComparePointIncapacity> BestIncapacityQueue;
typedef std::unordered_map <PointId, PointStats*> PointStatsDB;

class TabooSearch
{
public:
	TabooSearch(const int sat_threshold = 1);
	~TabooSearch();
	//PointId ProcessPointResults (const PointId& point, const Results& results);

	/**
	* \brief Adds point evaluation results to points DB
	* \param results [in] Point evaluation results, inluding point ID
	*/
	void AddPointResults (FitnessFunction fitnessFunction, const PointResults& results);
	
	/**
	* \brief Returns current record point from point DB
	*/
	PointStats GetCurrentRecord();
	void ResetCurrentRecord();

	// Priority queue, sorts evaluated points by incapacity record.
	BestIncapacityQueue origin_queue_;

	// Minimal required number of solved (SAT) problems in a sample
	int sat_threshold_ = 1;
	std::vector <PointId> GenerateNewPoints(const int desired_candidates = 1, const PointId& fixedVarsMask = PointId());
	std::vector <PointId> GenerateRandomPoints(const int num_ones,  const int desired_candidates, const PointId& basePoint);
	void Search(
		const int     num_iterations,
	       	const PointId starting_point,
	       	const std::vector <int> guessing_vars,
	       	const BitMask out_mask,
	      	const Sample  sample,
		const int num_points = 1);
	inline PointStats GetPointStats(const PointId& id) { return *checked_points_[id];}
	inline bool PointChecked(const PointId& id) const { return checked_points_.count(id) > 0; }
private:

	// Hash table. Contains all evaluated points and their stats
	PointStatsDB checked_points_;

	// A point holding the best incapacity record
	PointStats* global_record_ = NULL;

	std::mt19937 rng;

	//void LoadNewSample (int sample_size);

	// Service methods
	std::vector<PointId> GetUncheckedHammingNbhd (const PointId& point, const PointId& fixedVarsMask = PointId());
};


#endif
