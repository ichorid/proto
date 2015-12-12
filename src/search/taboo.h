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
	ComparePointIncapacity(const bool& revparam=false) {reverse=revparam;}
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

inline std::string Point2Bitstring(const PointId& p) { std::string out; for (int i=0; i<p.size(); ++i) out+= (p[i]==0 ? "0" : "1") ; return out;}
inline std::string Point2Varstring(const PointId& p) { std::string out; for (int i=0; i<p.size(); ++i) out+= (p[i]==0 ? "" : " "+std::to_string(i+1)); return out;}

class TabooSearch
{
public:
	TabooSearch();
	~TabooSearch();
	//PointId ProcessPointResults (const PointId& point, const Results& results);
	void AddPointResults (const PointResults& results);
	PointStats GetStats();
	BestIncapacityQueue origin_queue_;
	int sat_threshold_ = 1;
	std::vector <PointId> GenerateNewPoints(const int desired_candidates = 1);
private:
	PointStatsDB checked_points_;
	PointStats* global_record_ = NULL;
	std::mt19937 rng;

	//void LoadNewSample (int sample_size);

	// Service methods
	std::vector<PointId> GetUncheckedHammingNbhd (const PointId& point);
	inline bool PointChecked(const PointId& id) {return checked_points_.count(id)>0;};
};


#endif
