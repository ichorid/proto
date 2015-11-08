#include <iostream>
#include <mpi.h>
#include <cstddef>
#include "common.h"
#include "peer.h"
#include "wrappers/minisat22.h"
#include <search/taboo.h>

/* Peer base class methods */
// Here we describe our datastructures to MPI
void Peer::MPI_MakeSolverReportType()
{
	int count = 2; // number of blocks
	int blocklens[] = {1,1}; // number of elements in each block
	MPI_Aint indices[2]; // byte displacements of each block
	indices[0] = (MPI_Aint)offsetof (struct SolverReport, state);
	indices[1] = (MPI_Aint)offsetof (struct SolverReport, watch_scans);

	MPI_Datatype old_types[] = {MPI_INT, MPI_INT}; // types of elements in each block 
	MPI_Type_struct (count, blocklens, indices, old_types, &mpiT_SolverReport_);
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
	MPI_Recv(&msg_len, 1, MPI_INT, master_id_, data_tag_,
		       	MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	int* msg_body = (int*) malloc(msg_len * sizeof(Lit));
	MPI_Recv(&msg_body, msg_len, MPI_INT, master_id_, data_tag_,
		       	MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	Assignment out(msg_body, msg_body+msg_len);
	free (msg_body);
	return out;
}


SolverReport Worker::ProcessAssignment(const Assignment &asn)
{
	Minisat22Wrapper s;
	s.InitSolver (cnf_);
	s.AddUCs (asn);
	s.SetWatchScansLimit (scans_limit_);
	s.Solve ();

	return s.GetReport ();
}

void Worker::UploadAssignmentReport(const SolverReport rep)
{
	MPI_Send(&rep, 1, mpiT_SolverReport_, master_id_, data_tag_, MPI_COMM_WORLD);
}


void Worker::MainJobCycle()
{
	for (;;){
		Assignment asn = WaitRecieveAssignment ();
		if (asn.size()==0)
			break;
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
	PointId point = starting_point;
	for (int i=0; i<num_iterations; ++i){
		Task t = GenTask (BM_or (point, out_mask), sample);
		Results res = ProcessTask (t);
		point = search_engine_.ProcessPointResults(point, res);
		// DEBUG
		PointStats best_point = search_engine_.GetStats();
		auto tmp = search_engine_.origin_queue_.top(); 
		search_engine_.origin_queue_.pop();
	       	auto second_best =  search_engine_.origin_queue_.top();
	       	search_engine_.origin_queue_.push(tmp);

		std::cout << " Best fitness: " << best_point.best_fitness 
			<< " Queue size: " <<search_engine_.origin_queue_.size() 
			<< " queue top: " << search_engine_.origin_queue_.top()->best_fitness
		       	<< " second top: " << second_best->best_fitness; 
	}
}

Results Master::ProcessTask(Task& task)
{
	Results res;
	int initial_task_size = task.size();
	while(res.size()<initial_task_size){
		// Send work until free_workers stack depletes or there
		// it no task units left
		while (free_workers_.size()>0 && task.size()>0){
			GiveoutAssignment(GetWorker(), task.back()); 
			task.pop_back();
		}
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
	MPI_Recv(&out, 1, mpiT_SolverReport_, MPI_ANY_SOURCE, data_tag_,
		       	MPI_COMM_WORLD, &status);
	int sender_id = status.MPI_SOURCE;
	RegisterWorker(sender_id);
}

void Master::GiveoutAssignment (int target, Assignment asn)
{
	int msg_len = asn.size();
	MPI_Send(&msg_len, 1, MPI_INT, target, data_tag_,
		       	MPI_COMM_WORLD);

	// Achtung! Pointer trick should work only for std::vector containers!!!
	// TODO: make some kind of comiler assert to check if we're
	// actually using std::vector or some other container !!!
	MPI_Send(&asn[0], msg_len, MPI_INT, target, data_tag_,
		       	MPI_COMM_WORLD);
}




Master::Master (int num_workers)
{
	for (int i=1; i<num_workers; ++i)
		RegisterWorker(i);
}









