#ifndef PROTO_PEER_H_
#define PROTO_PEER_H_
#include <mpi.h>
#include "common.h"
#include "search/taboo.h"

// This class defines a kind of MPI "language and protocol" for classes that
// derive from it to use. It should contain MPI "derived types", tag numbers, etc.
// It's constructor/destructor actually invoke MPI_Datatype (un)registration procedures.
class Peer
{
public:
	Peer () { MPI_InitDatatypes (); }
	~Peer () { MPI_UnregDatatypes (); }
protected:
	int data_tag_=0;
	// TODO: Find a better way to produce this things
	static MPI_Datatype mpiT_SolverReport_;
private:
	void MPI_InitDatatypes () { MPI_MakeSolverReportType (); }
	void MPI_UnregDatatypes () { MPI_Type_free (&mpiT_SolverReport_); }
	void MPI_MakeSolverReportType ();
};


class Worker : protected Peer
{
public:
	Worker(Cnf cnf, int scans_limit =100000, int master_id = 0);
	void MainJobCycle();
	int master_id_ ;
	int scans_limit_;
	Cnf cnf_;
protected:
	void UploadAssignmentReport(const SolverReport rep);
	Assignment WaitRecieveAssignment ();
	SolverReport ProcessAssignment(const Assignment &asn);
};

class Master : protected Peer
{
public:
	Master (int num_workers);
	void Search(
		const int     num_iterations,
	       	const PointId starting_point,
	       	const BitMask out_mask,
	      	const Sample  sample);
	TabooSearch search_engine_;
private:
	std::vector <int> free_workers_;
	Results ProcessTask(Task& task);
	void GiveoutAssignment (int id, Assignment asn);
	int GetWorker() {int out=free_workers_.back(); free_workers_.pop_back(); return out;};
	SolverReport RecieveAndRegister();
	void RegisterWorker(int id) {free_workers_.push_back(id);}
};

#endif
