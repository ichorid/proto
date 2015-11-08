#include <master.h>
#include <worker.h>
#include <utils.h>
#define MAIN //Needed for a safe header trick with MPI pseudo-types
#include <mpi_types.h>


int main(int argc, char* argv[])
{
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
	Cnf cnf; ReadCNFile(filename, cnf);

	if (mpi_rank==0){
		Master master();
		master.Search(num_iterations);
		master.PrintResults();
	}else{
		// Split sample between worker processes
		int units_per_rank = sample_size/(mpi_size-1);
		Sample sample; MakeSample(cnf, core_len, sample, units_per_rank);

		Worker worker(cnf, scans_limit);
		worker.MainJobCycle();
	}


}
