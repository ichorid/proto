#ifndef PROTO_SEARCH_TABOO_H_
#define PROTO_SEARCH_TABOO_H_
#include <random>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include "common.h"
typedef std::string PointId;
typedef std::string BitMask;
typedef struct PointStats
{
	PointId  point_id;
	long unsigned int sample_size;
	long unsigned int sat_total;
	long int best_cutoff; // best scans limit
	double   best_fitness; 
} PointStats;


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
typedef std::priority_queue <PointStats*, std::vector <PointStats*>, 
	ComparePointFitness > BestFitnessQueue;
typedef std::unordered_map <PointId, PointStats*> PointStatsDB;

class TabooSearch
{
public:
	Task ProcessPointResults (const PointId& point, const Results& results);
	void AddPointResults (const PointId& point, const Results& results);
	void GetSearchStats();
private:
	PointStatsDB checked_points_;
	BestFitnessQueue origin_queue_;
	PointStats* global_record_;
	//std::random_device rng;
	//std::mt19937 mt(rng());
	std::mt19937 mt;

	PointId GenerateNewPoint();
	Task GenerateNewTask();
	//Sample GenerateNewSample (int sample_size);

	// Service methods
	std::vector<PointId> GetUncheckedHammingNbhd (const PointId& point);
	inline bool PointChecked(const PointId& id) {return checked_points_.count(id)>0;};
};

inline char FlipBit(char& bit){ return  bit^=1;}

#endif
