#include "easylogging++.h"

class RiseFallSearch
{
private:
	TabooSearch& searchEngine_;
	int numPoints_;
	int stallLimit_;

public:
	int rise_sat_threshold_ = 1;
	int fall_sat_threshold_ = 2;
	Evaluator& eval_;
	RiseFallSearch (Evaluator& e, TabooSearch& s, int n, int l, int t):
		eval_(e), searchEngine_(s), numPoints_(n), stallLimit_(l), fall_sat_threshold_(t) {}
	
	PointStats operator() (int groundLevel, PointId basePoint = PointId());
};

PointStats RiseFallSearch::operator() (int groundLevel, PointId basePoint )
{
	if (basePoint == PointId ())
		basePoint = PointId(eval_.guessing_vars.size(), 0);

	assert (CountOnes(basePoint) <= groundLevel);
	LOG(INFO) << " STAGE 1 - RISE";
	for (int i = groundLevel; (searchEngine_.origin_queue_.empty() && (i <= eval_.guessing_vars.size())); ++i)
		for (auto r: eval_ (searchEngine_.GenerateRandomPoints (i, 10 /* num points */, basePoint)))
			searchEngine_.AddPointResults (r, rise_sat_threshold_);

	LOG(INFO) << " STAGE 2 - FALL";
	PointStats lastRecord;
	for (int stallCount = 0; (!searchEngine_.origin_queue_.empty() && (stallCount < stallLimit_)); ++stallCount)
		for (auto r: eval_ (searchEngine_.GenerateNewPoints (numPoints_, basePoint)))
		{
			PointStats* cur = searchEngine_.AddPointResults (r, fall_sat_threshold_);
			if ((r.sat_total >= fall_sat_threshold_) && (r.best_incapacity < lastRecord.best_incapacity))
			{
				stallCount = 0;
				lastRecord = *cur;
				LOG(INFO) << " New record found: " << PrintPointStats (lastRecord, eval_.guessing_vars);
			}
		}

	while(!searchEngine_.origin_queue_.empty ()) 
		searchEngine_.origin_queue_.pop ();

	return lastRecord;
}

