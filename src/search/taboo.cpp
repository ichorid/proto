#include <algorithm>
#include <assert.h>
#include <math.h>
#include <random>
#include "search/taboo.h"
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include "easylogging++.h"
#include <functional>
#include <stack>

TabooSearch::TabooSearch()
{
	// Init rng with system's enthropy source
	std::random_device rnd_dev;
	rng.seed(rnd_dev());
	// Default "record" point
	PointStats* ps = new PointStats;
	checked_points_[ps->id] = ps;
	global_record_ = ps;
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
	for (int i = 0; i < point.size(); ++i)
	{
		PointId tmp = point;
		FlipBit(tmp[i]);
		//tmp[i]=0;
		if (!PointChecked(tmp))
		{
			result.push_back(tmp);
		}
	}
	return result;
}

void TabooSearch::AddPointResults (const PointResults& results)
{
	PointId point = results.id;
	// Filter SATs and sort their scans values
	std::vector <uint64_t> sat_scans;
	for (auto report: results.reps)
		if (report.state==SAT)
			sat_scans.push_back(report.watch_scans);
	std::sort(sat_scans.begin(), sat_scans.end());

	// Create new point
	PointStats* ps = new PointStats;
	ps->id = point;
	ps->sample_size = results.reps.size();
	ps->sat_total = sat_scans.size();

	// derive it's best incapacity
	int S = CountOnes(point);
	for (int i = 0; i < sat_scans.size(); ++i)
	{
		double t = sat_scans[i];
		double p = double(1+i)/ps->sample_size;	// predicted SAT probability
		double incapacity = S + log2(t * 3/p) ;
		if (incapacity < ps->best_incapacity)
		{
		       	ps->best_incapacity = incapacity;	// update local record
			ps->best_cutoff  = sat_scans[i];
		}
	}
	// and add it to DB and origin candidates queue.

	assert (checked_points_.count(point)==0);
	checked_points_[point] = ps;

	if (sat_scans.size()<sat_threshold_)
		return;

	origin_queue_.push(ps);

	// Check and update global incapacity record if necessary
	if (ps->best_incapacity < global_record_->best_incapacity)
	{
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

std::vector <PointId> TabooSearch::GenerateNewPoints(const int desired_candidates)
{
	// ACHTUNG!  Dequeues priority_queue in process!!
	std::unordered_set <PointId> candidates;
	int level = 0;
	int level_desired_candidates = 0;
	std::stack <PointStats*> tmp_stack;
	while (origin_queue_.size() > 0 && candidates.size() < desired_candidates)
	{
		if (candidates.size() == level_desired_candidates)
		{
			++level;
			// Logarithmic levels size distribution
			int slice_size = desired_candidates * pow(0.5, level);
			slice_size = slice_size > 0 ? slice_size : 1; // Cap 1
			level_desired_candidates += slice_size;
		}
		// Select next origin candidate from top incapacity queue
		auto nbhd = GetUncheckedHammingNbhd(origin_queue_.top()->id);
		if (nbhd.size() == 0)
		{
			// All origin's neighbours were already checked, so
			// we remove it from queue
			origin_queue_.pop();
			LOG(DEBUG) << "Origin POP!";
			continue;
		}
		std::shuffle(nbhd.begin(), nbhd.end(), rng); 

		// Append nbhd to candidates
		for (auto cand: nbhd)
			if(candidates.count(cand) == 0 && (candidates.size() < level_desired_candidates))
				candidates.insert(cand);
		// Dig deeper into prio queue
		if (origin_queue_.size() > 1)
		{
			tmp_stack.push(origin_queue_.top());
			origin_queue_.pop();
		}
	}
	// Restore prio queue
	// Actually algorithm works better if we don't restore the queue!
	while (tmp_stack.size() > 0)
	{
		origin_queue_.push(tmp_stack.top());
		tmp_stack.pop();
	}
	return std::vector <PointId> (candidates.begin(), candidates.end());
}

std::vector <PointId> TabooSearch::GenerateRandomPoints(
		const int num_ones,
		const int desired_candidates,
		const int point_size)
{
	std::unordered_set <PointId> candidates;
	while (candidates.size() < desired_candidates)
	{
		PointId point;
		for (int i = 0; i < point_size; ++i)
			point.push_back(i < num_ones ? 1 : 0);
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

PointStats TabooSearch::GetCurrentRecord()
{
	return *global_record_;
}
