#include <master.h>
#include <worker.h>
#include <utils.h>


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
		Sample sample; MakeSample(cnf, core_len, out_mask, sample, units_per_rank);
		Master master();
		master.Search(num_iterations, starting_point, sample);
		master.PrintResults();
	}else{
		// Split sample between worker processes
		Worker worker(cnf, scans_limit);
		worker.MainJobCycle();
	}


}
