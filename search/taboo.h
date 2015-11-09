#ifndef PROTO_SEARCH_TABOO_H_
#define PROTO_SEARCH_TABOO_H_
#include <random>
#include <unordered_map>
#include <vector>
#include <queue>
#include "common.h"

// Comparator for point fitness queue. Works via pointers.
class ComparePointFitness
{
	bool reverse;
public:
	ComparePointFitness(const bool& revparam=false) {reverse=revparam;}
	bool operator() (const PointStats* lhs, const PointStats* rhs) const
	{
		if (reverse) 
			return (lhs->best_fitness > rhs->best_fitness);
		else
			return (lhs->best_fitness < rhs->best_fitness);
	}
};
typedef std::priority_queue <PointStats*, std::vector <PointStats*>, ComparePointFitness > BestFitnessQueue;
typedef std::unordered_map <PointId, PointStats*> PointStatsDB;

class TabooSearch
{
public:
	TabooSearch();
	~TabooSearch();
	PointId ProcessPointResults (const PointId& point, const Results& results);
	void AddPointResults (const PointId& point, const Results& results);
	PointStats GetStats();
	BestFitnessQueue origin_queue_;
private:
	PointStatsDB checked_points_;
	PointStats* global_record_ = NULL;
	std::mt19937 rng;

	PointId GenerateNewPoint();
	//void LoadNewSample (int sample_size);

	// Service methods
	std::vector<PointId> GetUncheckedHammingNbhd (const PointId& point);
	inline bool PointChecked(const PointId& id) {return checked_points_.count(id)>0;};
};


#endif
