#include <mpi.h>
#include "worker.h"
#include "mpi_derived.h"
#include "wrappers/minisat22.h" // TODO: make this object a template


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
	MPI_Send(&rep, 1, mpiT_SolverReport, master_id_, data_tag_, MPI_COMM_WORLD);
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


