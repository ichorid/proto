#include <algorithm>
#include <assert.h>
#include <math.h>
#include "search/taboo.h"

int CountOnes(const std::string& str)
{
	int out=0;
	for (auto ch: str)
		out+=(ch==1);
	return out;
}


/*
std::vector<PointId> GetHammingNbhd (PointId point)
{
	std::vector<PointId>  result;
	for (int i =0; i<point.size(); ++i){
		result.push_back(point);
		FlipBit(result[i][i]);
	}
	return result;
}
*/

std::vector<PointId> TabooSearch::GetUncheckedHammingNbhd (const PointId& point)

{
	std::vector<PointId> result;
	for (int i =0; i<point.size(); ++i){
		PointId tmp = point;
		FlipBit(tmp[i]);
		if (PointChecked(tmp))
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
		// New record found !!!
		global_record_= checked_points_[point];
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
	}
	// Shuffle candidate points to even their probabilities
	std::shuffle(candidates.begin(), candidates.end(), mt);

	return candidates[0];
}

UnitClauseVector MaskUC(const BitMask& mask, const UnitClauseVector& ucv)
{
	assert(mask.size()==ucv.size());
	UnitClauseVector out;
	for (int i=0; i<mask.size(); ++i)
		if (mask[i]==1)
			out.push_back(ucv[i]);
	return out;
}


//Sample TabooSearch::GenerateNewSample (int sample_size)
Task TabooSearch::GenerateNewTask()
{
	Task out;
	const PointId point = GenerateNewPoint();
	for(auto solution_ucv: sample){
		BitMask full_mask = point + default_mask;
		out.push_back(MaskUC(full_mask, solution_ucv));
	}
	return out;
}

Task ProcessPointResults (const PointId& point, const Results& results)
{
	AddPointResults(point, results);
	Task new_task = GenerateNewTask();
	return new_task;
}



























