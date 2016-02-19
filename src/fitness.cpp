#include "common.h"

PointStats IncapacityFitnessFunction(const PointResults& results)
{
	PointStats ps;
	// Filter SATs and sort their scans values
	std::vector <uint64_t> sat_scans;
	for (auto report: results.reps)
		if (report.state==SAT)
			sat_scans.push_back(report.watch_scans);
	std::sort(sat_scans.begin(), sat_scans.end());

	// Create new point
	ps.id = results.id;
	ps.sample_size = results.reps.size();
	ps.sat_total = sat_scans.size();

	// derive it's best incapacity
	int S = CountOnes(ps.id);
	for (int i = 0; i < sat_scans.size(); ++i)
	{
		double t = sat_scans[i];
		double p = double(1+i) / ps.sample_size;// predicted SAT probability
		double incapacity = S + log2(t * 3/p) ;
		if (incapacity < ps.best_incapacity)
		{
		       	ps.best_incapacity = incapacity;// update local record
			ps.best_cutoff  = sat_scans[i];
		}
	}
	return ps;
}

PointStats TotalSolvedFitnessFunction(const PointResults& results)
{
	// Create new point
	PointStats ps;
	ps.id = results.id;
	ps.sample_size = results.reps.size();
	ps.sat_total = 0;

	int totalScans = 0;
	for (auto report: results.reps)
	{
		if (report.state==UNSAT)
		{
			++ps.sat_total;
			totalScans += report.watch_scans;
		}
	}
	//totalScans += (ps.sample_size - ps.sat_total) * totalScans;
	int S = CountOnes(ps.id);
	ps.best_incapacity = S + log2((float)(ps.sample_size - ps.sat_total)/ps.sample_size);
	
	return ps;
}
