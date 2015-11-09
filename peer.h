#ifndef PROTO_PEER_H_
#define PROTO_PEER_H_
#include <mpi.h>
#include "common.h"
#include "search/taboo.h"

class MpiBase
{
public:
	 MpiBase (int* argc_p, char ***argv_p ) { MPI_Init (argc_p, argv_p); MPI_InitDatatypes ();}
	~MpiBase () { MPI_UnregDatatypes ();   MPI_Finalize();}
	MPI_Datatype SolverReportT_;
private:
	void MPI_InitDatatypes () { MPI_MakeSolverReportType (); }
	void MPI_UnregDatatypes () { MPI_Type_free (&SolverReportT_); }
	void MPI_MakeSolverReportType ();
	MpiBase (MpiBase const&) = delete;
	void operator = (MpiBase const&) = delete;
};

// This class defines a kind of MPI "language and protocol" for classes that
// derive from it to use. It should contain MPI "derived types", tag numbers, etc.
// It's constructor/destructor actually invoke MPI_Datatype (un)registration procedures.
class Peer
{
protected:
	int data_tag_=0;
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
	Master (int mpi_size);
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
