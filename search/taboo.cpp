#include <algorithm>
#include <assert.h>
#include <math.h>
#include <random>
#include "search/taboo.h"
#include <iostream>
#include "easylogging++.h"

#define MAX_DOUBLE pow(2.0, 1024)



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
		.best_incapacity = MAX_DOUBLE
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
		.best_incapacity = MAX_DOUBLE
	};

	// derive it's best incapacity
	int S = CountOnes(point);
	const double sz = pow(2.0, S);	// backdoor size multiplier
	for (int i=0; i<sat_scans.size(); ++i){
		double t = sat_scans[i];
		double p = (1+i)/ps->sample_size;	// predicted SAT probability
		double incapacity = sz * t * 3/p ;
		if (incapacity < ps->best_incapacity){
		       	ps->best_incapacity = incapacity;	// update local record
			ps->best_cutoff  = sat_scans[i];
		}
	}
	// and add it to DB and origin candidates queue.
	checked_points_[point] = ps;
	origin_queue_.push(ps);

	// Check and update global incapacity record if necessary
	if (ps->best_incapacity < global_record_->best_incapacity){
		global_record_= checked_points_[point]; // New record found !!!
		LOG(INFO) <<" New record found  ("<< ps->best_incapacity << ") : " << Point2Varstring (point) ;
	}
}

PointId TabooSearch::GenerateNewPoint()
{
	std::vector <PointId> candidates;
	for(;;){
		// Select next origin candidate from top incapacity queue
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
	
	// DEBUG
	auto second_best =  *(&origin_queue_.top() + (origin_queue_.size()>1 ? 1:0)); // dirty pointer hack!!
	LOG_EVERY_N(10, DEBUG) 
		<< Point2Bitstring(point) 
		<< " Best incapacity: " << GetStats().best_incapacity 
		<< " queue size: " <<origin_queue_.size() 
		<< " queue top: " << origin_queue_.top()->best_incapacity
		<< " second top: " << second_best->best_incapacity;

	return GenerateNewPoint();
}

PointStats TabooSearch::GetStats()
{
	return *global_record_;
}
