#include "utils.h"
#include "peer.h"

MpiBase* mpiS;

int main(int argc, char* argv[])
{
	MpiBase mpi_o(&argc, &argv); mpiS = &mpi_o;
	//MPI_Init (&argc, &argv);
	int mpi_rank; MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);
	int mpi_size; MPI_Comm_size (MPI_COMM_WORLD, &mpi_size);

	if (argc<2){
	       	std::cout <<"Usage: <CNF> <scans_limit> " <<std::endl;
		exit(1);
	}
	Cnf cnf; ReadCNFile(argv[1], cnf);
	int scans_limit = atoi(argv[2]);

	int core_len = 177;
	int out_len= 200;
	int sample_size = 100;
	int num_iterations= 1000;


	if (mpi_rank==0){
		Sample sample; MakeSample(cnf, core_len, sample, sample_size);
		int num_vars = sample[0].size();
		BitMask out_mask;
		for (int i=0; i<num_vars-out_len; ++i)
			out_mask.push_back(0);
		for (int i=0; i<out_len; ++i)
			out_mask.push_back(1);
		assert(out_mask.size()==sample[0].size());
		PointId starting_point;
		for (int i=0; i<num_vars-out_len; ++i)
			starting_point.push_back(1);

		Master master(mpi_size);
		master.Search(num_iterations, starting_point, out_mask, sample);
		//master.SendExitSignal();
	}else{
		// Split sample between worker processes
		Worker worker(cnf, scans_limit);
		worker.MainJobCycle();
	}

	//MPI_Finalize();
	return 0;


}
