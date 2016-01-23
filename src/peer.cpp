#include <iostream>
/* Workaround for a buggy MPI compiler */
#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include <mpi.h>
#include <cstddef>
#include "common.h"
#include "peer.h"
#include "wrappers/minisat22.h"
#include "search/taboo.h"
#include "easylogging++.h"

extern MpiBase* mpiS;

// Here we describe our datastructures to MPI
void MpiBase::MPI_MakeSolverReportType()
{
	int count = 2; // number of blocks
	int blocklens[] = {1,1}; // number of elements in each block
	MPI_Aint indices[2]; // byte displacements of each block
	indices[0] = (MPI_Aint)offsetof (struct SolverReport, state);
	indices[1] = (MPI_Aint)offsetof (struct SolverReport, watch_scans);

	MPI_Datatype old_types[] = {MPI_INT, MPI_INT}; // types of elements in each block 
	MPI_Type_struct (count, blocklens, indices, old_types, &SolverReportT_);
	MPI_Type_commit(&SolverReportT_);
}

/* Worker class methods */
Worker::Worker(Cnf cnf, int scans_limit , int master_id )
{
	cnf_         = cnf;
	scans_limit_ = scans_limit;
	master_id_   = master_id;
}

Assignment Worker::WaitRecieveAssignment ()
{
	// Assumes assignment is just an array of int's
	int msg_len;
	MPI_Recv(&msg_len, 1, MPI_INT, master_id_, data_tag_, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (msg_len==0)
		return Assignment(); //Stop signal recieved

	int* tmp_arr = (int*) malloc(msg_len * sizeof(Lit));
	MPI_Recv(tmp_arr, msg_len, MPI_INT, master_id_, data_tag_, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	Assignment out(tmp_arr, tmp_arr+msg_len);

	free (tmp_arr);
	return out;
}

SolverReport Worker::ProcessAssignment(Assignment &asn)
{
	Minisat22Wrapper s;
	s.InitSolver (cnf_);
	s.AddUCs (asn);
	s.SetWatchScansLimit (scans_limit_);
	s.Solve ();

	return s.GetReport ();
}

void Worker::UploadAssignmentReport(SolverReport rep)
{
	MPI_Send(&rep, 1, mpiS->SolverReportT_, master_id_, data_tag_, MPI_COMM_WORLD);
}


void Worker::MainJobCycle()
{
	for (;;){
		Assignment asn = WaitRecieveAssignment ();
		if (asn.size()==0) break;
		SolverReport rep = ProcessAssignment (asn);
		UploadAssignmentReport (rep);
	}
}


/* Master methods */

void Master::Search(
		const int     num_iterations,
	       	const PointId starting_point,
	       	const std::vector <int> guessing_vars,
	       	const BitMask out_mask,
	      	const Sample  sample,
		const int num_points)
{
	std::vector <PointId> probe_points;
	//probe_points.push_back(starting_point); // FIXME


	// Stage 1: bottom-up climb
	const int try_points = 10;
	for (int iter=4; iter<num_iterations; ++iter)
	{
		probe_points = (search_engine_.GenerateRandomPoints(iter, num_points/try_points, guessing_vars.size()));
		std::vector <Task> all_tasks;
		for (auto point: probe_points)
		{
			Task t =
			{
				.id = point,
				.units = GenTaskUnits (BM_or ( ExpandBM(point, guessing_vars), out_mask), sample)
			};
			t.units.resize(try_points);
			all_tasks.push_back(t); // FIXME: resize sample BEFORE generation
		}

		std::vector <PointResults> all_results = ProcessTasks (all_tasks);
		for (auto result: all_results)
		{
			search_engine_.AddPointResults(result);
		}
		if (search_engine_.origin_queue_.size()>0)
			break;
	}
	probe_points = search_engine_.GenerateNewPoints(num_points); 

	// Stage2 2: Search
	for (int iter=0; iter<num_iterations; ++iter)
	{
		std::vector <Task> all_tasks;
		for (auto point: probe_points)
		{
			Task t =
			{
				.id = point,
				.units = GenTaskUnits (BM_or ( ExpandBM(point, guessing_vars), out_mask), sample)
			};
			all_tasks.push_back(t);
		}

		std::vector <PointResults> all_results = ProcessTasks (all_tasks);
		for (auto result: all_results)
		{
			search_engine_.AddPointResults(result);
		}
		probe_points = search_engine_.GenerateNewPoints(num_points); 
	}
}



std::vector <PointResults> Master::ProcessTasks(const std::vector <Task> &tasks)
{
	std::vector <PointResults> out;

	std::vector <UnitClauseVector> flat_task;
	for (auto task: tasks)
	{
		std::copy(task.units.begin(), task.units.end(), std::back_inserter(flat_task));
	}
	std::vector <SolverReport> flat_results = ProcessTaskUnits(flat_task);

	int i=0;
	for (auto task: tasks)
	{
		PointResults pr =
		{
			.id = task.id,
			.reps = std::vector <SolverReport> (&flat_results[i], &flat_results[i + task.units.size()])
		};
		out.push_back(pr);
		i+=task.units.size();
	}
	return out;
}

std::vector <SolverReport> Master::ProcessTaskUnits(const std::vector <UnitClauseVector> &units)
{
	std::vector <SolverReport> out(units.size());
	std::vector <int> worker2unitnum(free_workers_.size()+1);
	for (int i=0, j=0; i < out.size(); ++i)
	{
		// Send work until free_workers stack depletes or there
		// are no units left
		while (free_workers_.size()>0 && j < out.size()){
			int workernum = GetWorker();
			worker2unitnum[workernum] = j;
			GiveoutAssignment(workernum, units[j]); 
			++j;
		}
		if (units[0].size()==0) return out; // Special case - stop signal task
		// Wait for a report from worker and add it's id to
		// free_workers stack immediately.
		auto report = RecieveAndRegister();
		out[worker2unitnum[free_workers_.back()]]=report;
	};
	assert (free_workers_.size()==(worker2unitnum.size()-1));
	return out;
}

SolverReport Master::RecieveAndRegister()
{
	SolverReport rep;
	MPI_Status status;
	MPI_Recv(&rep, 1, mpiS->SolverReportT_, MPI_ANY_SOURCE, data_tag_, MPI_COMM_WORLD, &status);
	RegisterWorker(status.MPI_SOURCE);
	return rep;
}

void Master::GiveoutAssignment (int target, Assignment asn)
{
	int msg_len = asn.size();
	MPI_Send(&msg_len, 1, MPI_INT, target, data_tag_, MPI_COMM_WORLD);
	if (msg_len==0)
		return; // Sending stop signal

	// Achtung! Pointer trick should work only for std::vector containers!!!
	// TODO: make some kind of comiler assert to check if we're
	// actually using std::vector and not some other container !!!
	MPI_Send(&asn[0], msg_len, MPI_INT, target, data_tag_, MPI_COMM_WORLD);
}

void Master::SendExitSignal()
{
	std::vector <UnitClauseVector> t;
	for (int i=0; i<total_workers_; ++i)
		t.push_back(UnitClauseVector());
	ProcessTaskUnits(t);
}

Master::Master (int mpi_size)
{
	total_workers_= mpi_size-1; 
	for (int i=1; i<mpi_size; ++i)
		RegisterWorker(i);
}
