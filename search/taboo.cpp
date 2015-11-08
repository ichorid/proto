#include <algorithm>
#include <assert.h>
#include <math.h>
#include <random>
#include "search/taboo.h"
#include <iostream>




TabooSearch::TabooSearch()
{
	std::random_device rnd_dev;
	rng.seed(rnd_dev());
	PointId point_zero = PointId();
	PointStats* ps = new PointStats {
		.point_id = point_zero,
		.sample_size = 0,
		.sat_total = 0,
		.best_cutoff = 0,
		.best_fitness = 0
	};
	checked_points_[point_zero] = ps;
	global_record_=ps;

}

TabooSearch::~TabooSearch()
{
	// Delete all points from DB
	for (auto point_pair: checked_points_)
		delete point_pair.second;
}

std::vector<PointId> TabooSearch::GetUncheckedHammingNbhd (const PointId& point)
{
	std::vector<PointId> result;
	for (int i =0; i<point.size(); ++i){
		PointId tmp = point;
		FlipBit(tmp[i]);
		//tmp[i]=0;
		if (!PointChecked(tmp))
			result.push_back(tmp);
	}
	return result;
}

void TabooSearch::AddPointResults (const PointId& point, const Results& results)
{
	// Filter SATs and sort their scans values
	std::vector <long long int> sat_scans;
	for (auto report: results)
		if (report.state==SAT)
			sat_scans.push_back(report.watch_scans);
	std::sort(sat_scans.begin(), sat_scans.end());

	// Create new point
	PointStats* ps = new PointStats {
		.point_id = point,
		.sample_size = results.size(),
		.sat_total = sat_scans.size(),
		.best_cutoff = 0,
		.best_fitness = 0
	};

	// derive it's best fitness
	int backdoor_size = CountOnes(point);
	const double multi = pow(2.0, backdoor_size);	// backdoor size multiplier
	for (int i=0; i<sat_scans.size(); ++i){
		double prob = (1+i)/ps->sample_size;	// predicted SAT probability
		double cost = multi * sat_scans[i];	// backdoor cost
		double fitness = prob / cost;		// fitness
		if (fitness > ps->best_fitness){
		       	ps->best_fitness = fitness;	// update local record
			ps->best_cutoff  = sat_scans[i];
		}
	}
	// and add it to DB and origin candidates queue.
	checked_points_[point] = ps;
	origin_queue_.push(ps);

	// Check and update global fitness record if necessary
	if (ps->best_fitness > global_record_->best_fitness){
		global_record_= checked_points_[point]; // New record found !!!
	}
}

PointId TabooSearch::GenerateNewPoint()
{
	std::vector <PointId> candidates;
	for(;;){
		// Select next origin candidate from top fitness queue
		candidates = GetUncheckedHammingNbhd(
				origin_queue_.top()->point_id);
		if (candidates.size()>0) break;
		// All origin's neighbours were already checked, so
		// we remove it from queue
		origin_queue_.pop();
		std::cout << std::endl << " ORIGIN POP!";
	}
	// Shuffle candidate points to even their probabilities
	std::shuffle(candidates.begin(), candidates.end(), rng);

	return candidates[0];
}

PointId TabooSearch::ProcessPointResults (const PointId& point, const Results& results)
{
	AddPointResults(point, results);
	return GenerateNewPoint();
}

PointStats TabooSearch::GetStats()
{
	return *global_record_;
}
