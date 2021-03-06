#ifndef PEER_H_
#define PEER_H_
#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include <mpi.h>
#include "common.h"
#include "search/taboo.h"

// It's constructor/destructor actually invoke MPI_Datatype (un)registration procedures.
class MpiBase
{
public:
	MPI_Datatype SolverReportT_;

	 MpiBase (int* argc_p, char ***argv_p ) { MPI_Init (argc_p, argv_p); MPI_InitDatatypes ();}
	~MpiBase () { MPI_UnregDatatypes ();   MPI_Finalize();}
private:
	void MPI_InitDatatypes () { MPI_MakeSolverReportType (); }
	void MPI_MakeSolverReportType ();
	MpiBase (MpiBase const&) = delete;
	void operator = (MpiBase const&) = delete;
	void MPI_UnregDatatypes () { MPI_Type_free (&SolverReportT_); }
};

class Peer
{
protected:
	int data_tag_=0;
};


class Worker : protected Peer
{
public:
	int master_id_ ;
	Cnf cnf_;
	int scans_limit_;

	Worker(Cnf cnf, int scans_limit =100000, int master_id = 0);
	void MainJobCycle();
protected:
	SolverReport ProcessAssignment (Assignment &asn);
	Assignment   WaitRecieveAssignment ();
	void         UploadAssignmentReport (const SolverReport rep);
};

class Master : protected Peer
{
public:
	TabooSearch search_engine_;

	Master (int mpi_size);
	void Search(
		const int     num_iterations,
	       	const PointId starting_point,
	       	const std::vector <int> guessing_vars,
	       	const BitMask out_mask,
	      	const Sample  sample);
	void SendExitSignal();
private:
	int total_workers_;
	std::vector <int> free_workers_;

	Results ProcessTask(Task& task);
	void GiveoutAssignment (int id, Assignment asn);
	SolverReport RecieveAndRegister();
	void RegisterWorker(int id) {free_workers_.push_back(id);}
	int  GetWorker() {int out=free_workers_.back(); free_workers_.pop_back(); return out;};
};

#endif
