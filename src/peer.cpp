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
	       	const BitMask out_mask,
	      	const Sample  sample)
{
	PointId current_point = starting_point;
	for (int i=0; i<num_iterations; ++i){
		Task t = GenTask (BM_or (current_point, out_mask), sample);
		Results r = ProcessTask (t);
		current_point = search_engine_.ProcessPointResults(current_point, r);
	}
}

Results Master::ProcessTask(Task& task)
{
	Results res;
	int initial_task_size = task.size();
	while(res.size()<initial_task_size){
		// Send work until free_workers stack depletes or there
		// are no task units left
		while (free_workers_.size()>0 && task.size()>0){
			GiveoutAssignment(GetWorker(), task.back()); 
			task.pop_back();
		}
		if (task[0].size()==0) break; // Special case - stop signal
		// Wait for a report from worker and add his id to
		// free_workers stack immediately.
		SolverReport rep = RecieveAndRegister();
		res.push_back(rep);
	};
	return res;
}

SolverReport Master::RecieveAndRegister()
{
	SolverReport out;
	MPI_Status status;
	MPI_Recv(&out, 1, mpiS->SolverReportT_, MPI_ANY_SOURCE, data_tag_, MPI_COMM_WORLD, &status);
	RegisterWorker(status.MPI_SOURCE);
	return out;
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
	MPI_Send(&asn[0], msg_len, MPI_INT, target, data_tag_,
		       	MPI_COMM_WORLD);
}

void Master::SendExitSignal()
{
	Task t;
	for (int i=0; i<total_workers_; ++i)
		t.push_back(UnitClauseVector());
	ProcessTask(t);
}

Master::Master (int mpi_size)
{
	total_workers_= mpi_size-1; 
	for (int i=1; i<mpi_size; ++i)
		RegisterWorker(i);
}










