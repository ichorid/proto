#include <algorithm>
#include <assert.h>
#include <math.h>
#include <random>
#include "search/taboo.h"
#include <iostream>
#include <iomanip>
#include <unordered_set>
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
		//FlipBit(tmp[i]);
		tmp[i]=0;
		if (!PointChecked(tmp))
			result.push_back(tmp);
	}
	return result;
}

void TabooSearch::AddPointResults (const PointResults& results)
{
	PointId point = results.id;
	// Filter SATs and sort their scans values
	std::vector <long long int> sat_scans;
	for (auto report: results.reps)
		if (report.state==SAT)
			sat_scans.push_back(report.watch_scans);
	std::sort(sat_scans.begin(), sat_scans.end());

	// Create new point
	PointStats* ps = new PointStats {
		.point_id = point,
		.sample_size = results.reps.size(),
		.sat_total = sat_scans.size(),
		.best_cutoff = 0,
		.best_incapacity = MAX_DOUBLE
	};

	// derive it's best incapacity
	int S = CountOnes(point);
	for (int i=0; i<sat_scans.size(); ++i){
		double t = sat_scans[i];
		double p = double(1+i)/ps->sample_size;	// predicted SAT probability
		double incapacity = S + log2(t * 3/p) ;
		if (incapacity < ps->best_incapacity){
		       	ps->best_incapacity = incapacity;	// update local record
			ps->best_cutoff  = sat_scans[i];
		}
	}
	// and add it to DB and origin candidates queue.
	checked_points_[point] = ps;
	if (sat_scans.size()<sat_threshold_)
		return;
	origin_queue_.push(ps);

	// Check and update global incapacity record if necessary
	if (ps->best_incapacity < global_record_->best_incapacity){
		global_record_= checked_points_[point]; // New record found !!!
		LOG(INFO) << " New record found: " 
			<< std::setw(5) << CountOnes(point) << " "
			<< std::setw(8) << std::setprecision(2) << std::fixed << ps->best_incapacity << " "    
			<< std::setw(12) << std::scientific << pow(2.0, ps->best_incapacity) << " "    
			<< "W: " << std::setw(8) << std::scientific << ps->best_cutoff << " "    
			<< std::setw(5) << sat_scans.size() << " /" 
			<< std::setw(5) << results.reps.size() << " " 
			<< Point2Bitstring(point) << " ccc "
			<< Point2Varstring(point) ; // FIXME: expand vars according to the mask
	}
}

std::vector <PointId> TabooSearch::GenerateNewPoints(const int desired_candidates )
{
	// ACHTUNG!  Dequeues priority_queue in process!!
	std::vector <PointId> candidates;
	int levels = 0;
	std::queue <PointStats*> tmp_queue;
	for(;;)
	{
		// Select next origin candidate from top incapacity queue
		auto nbhd = GetUncheckedHammingNbhd(origin_queue_.top()->point_id);
		if (nbhd.size()==0)
		{
			// All origin's neighbours were already checked, so
			// we remove it from queue
			auto oldtop = origin_queue_.top();
			origin_queue_.pop();
			auto newtop = origin_queue_.top();
			LOG(DEBUG) << " ORIGIN POP! HD: " << std::setw(5) << CountOnes(BM_xor(oldtop->point_id, newtop->point_id));
			continue;
		}
		++levels;
		std::shuffle(nbhd.begin(), nbhd.end(), rng); 

		int slice_size = (((desired_candidates>>levels) > 1) ? (desired_candidates>>levels) : 1);
		//	LOG(DEBUG) << " SSIZE " << slice_size ;
		if (slice_size < nbhd.size())
			nbhd.resize(slice_size); // Logarithmic levels distibution

		// Append nbhd to candidates
		std::move(nbhd.begin(), nbhd.end(), std::inserter(candidates, candidates.end()));

 		// Enough candidates found
		if (candidates.size()>=desired_candidates)
			break;
		// Dig deeper into prio queue
		if (origin_queue_.size()>1)
		{
			tmp_queue.push(origin_queue_.top());
			origin_queue_.pop();
		}
	}
	// Restore prio queue
	while (tmp_queue.size()>0)
	{
		origin_queue_.push(tmp_queue.back()); tmp_queue.pop();
	}

	candidates.resize(desired_candidates);

	return candidates;
}

std::vector <PointId> TabooSearch::GenerateRandomPoints(const int num_ones,  const int desired_candidates, const int point_size )
{
	std::unordered_set <PointId> candidates;
	while (candidates.size()<desired_candidates)
	{
		PointId point;
		for (int i = 0; i < point_size; ++i)
			point.push_back( i < num_ones ? 1 : 0);
		std::shuffle(point.begin(), point.end(), rng); 
		if (!PointChecked(point) && candidates.count(point) == 0)
			candidates.insert(point);
		//FIXME: add safety checks!
	}
	return std::vector <PointId> (candidates.begin(), candidates.end());
}


/*
void TabooSearch::ProcessPointResults (const PointId& point, const Results& results)
{
	AddPointResults(point, results);
	
	// DEBUG
	auto second_best =  *(&origin_queue_.top() + (origin_queue_.size()>1 ? 1:0)); // dirty pointer hack!!
	LOG_EVERY_N(10, DEBUG) 
		<< Point2Bitstring(point) 
		<< " Point incapacity: " << checked_points_[point]->best_incapacity
		<< " Point SAT units: " << checked_points_[point]->sat_total
		<< " Best incapacity: " << GetStats().best_incapacity 
		<< " queue size: " <<origin_queue_.size() 
		<< " queue top: " << origin_queue_.top()->best_incapacity
		<< " second top: " << second_best->best_incapacity;

}
*/

PointStats TabooSearch::GetStats()
{
	return *global_record_;
}
