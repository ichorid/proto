#include <algorithm>
#include <assert.h>
#include <math.h>
#include <random>
#include "search/taboo.h"
#include <unordered_set>
#include "easylogging++.h"
#include <functional>
#include <stack>

TabooSearch::TabooSearch(int sat_threshold)
{
	sat_threshold_ = sat_threshold;
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



std::vector <PointId> TabooSearch::Checked (const std::vector<PointId> & vec)
{
	std::vector<PointId> result;
	for (PointId p: vec)
		if (PointChecked (p))
			result.push_back (p);
	return result;
}

std::vector <PointId> TabooSearch::Unchecked (const std::vector<PointId> & vec)
{
	std::vector<PointId> result;
	for (PointId p: vec)
		if (!PointChecked (p))
			result.push_back (p);
	return result;
}


std::vector<PointId> TabooSearch::GetUncheckedHammingNbhd (const PointId& point, const PointId& fixedVarsMask)
{
	std::vector<PointId> result;
	for (PointId p: HammingNbhd(point, 0))
	{
		PointId tmp = BM_or(p, fixedVarsMask);
		if (!PointChecked(tmp))
			result.push_back(tmp);
	}
	return result;
}

void TabooSearch::AddPointResults (const PointStats& ps)
{
	PointStats* psp = new PointStats; 
	*psp = ps;
	// and add it to DB and origin candidates queue.

	assert (checked_points_.count(ps.id)==0);
	checked_points_[ps.id] = psp;

	if (ps.sat_total < sat_threshold_)
		return;
	origin_queue_.push(psp);
	// Check and update global incapacity record if necessary
	if (ps.best_incapacity < global_record_->best_incapacity)
	{
		global_record_= checked_points_[ps.id]; // New record found !!!
		LOG(INFO) << " New record found: " << PrintPointStats(ps);
	}
}

std::vector <PointId> TabooSearch::GenerateNewPoints(const int desired_candidates, const PointId& fixedVarsMask)
{
	// ACHTUNG!  Dequeues priority_queue in process!!
	std::unordered_set <PointId> candidates;
	std::vector <PointId> stash;
	std::stack <PointStats*> tmp_stack;
	int level = 0;
	int level_desired_candidates = 0;
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
		auto nbhd = GetUncheckedHammingNbhd(origin_queue_.top()->id, fixedVarsMask);
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
			if(candidates.count(cand) == 0)
				if (candidates.size() < level_desired_candidates)
					candidates.insert(cand);
				else
					stash.push_back(cand);

		// Dig deeper into prio queue
		if (origin_queue_.size() > 1)
		{
			tmp_stack.push(origin_queue_.top());
			origin_queue_.pop();
		}
		else
		{
			for (auto cand: stash)
				if(candidates.count(cand) == 0 && candidates.size() < level_desired_candidates)
					candidates.insert(cand);
			break;
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
		const PointId& basePoint)
{

	std::unordered_set <PointId> candidates;
	size_t baseSize = basePoint.size();
	size_t baseCount0 = CountZeroes(basePoint);
	size_t baseCount1 = baseSize-baseCount0;
	assert (num_ones >= baseCount1);
	if (num_ones == baseSize)
	{
		PointId allOnes (baseSize, 1);
		if (!PointChecked (allOnes))
			candidates.insert (allOnes);
	}
	else 	
	{
		//FIXME: add BETTER safety checks!
		int failCount = 10000;
		while (candidates.size() < desired_candidates)
		{
			PointId randomBools;
			for (size_t i=0; i < baseCount0; ++i)
				randomBools.push_back (i < (num_ones - baseCount1) ? 1 : 0);
			std::shuffle (randomBools.begin(), randomBools.end(), rng); 

			PointId point;
			size_t i=0;
			for (auto v: basePoint)
				point.push_back (v ? v : randomBools[i++]);

			if (!PointChecked (point) && candidates.count (point) == 0)
				candidates.insert (point);
			else if (failCount-- == 0)
				break;
		}
	}
	std::vector <PointId> res (candidates.begin(), candidates.end());
	std::shuffle (res.begin(), res.end(), rng); 
	if (res.size()>desired_candidates)
		res.resize(desired_candidates);
	return res;
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

void TabooSearch::ResetCurrentRecord()
{
	global_record_ = checked_points_[PointStats().id];
}

