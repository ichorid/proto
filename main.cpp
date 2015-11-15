#include "utils.h"
#include "peer.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

MpiBase* mpiS;

int main(int argc, char* argv[])
{
	// Configure EasyLogger++
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	defaultConf.setGlobally( el::ConfigurationType::Format, "%datetime  %msg");
	defaultConf.setGlobally( el::ConfigurationType::ToFile, "true");
	defaultConf.setGlobally( el::ConfigurationType::Filename, "./fh.log");
	el::Loggers::reconfigureLogger("default", defaultConf);


	MpiBase mpi_o(&argc, &argv); mpiS = &mpi_o;
	int mpi_rank; MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);
	int mpi_size; MPI_Comm_size (MPI_COMM_WORLD, &mpi_size);

	if (argc<4){
	       	std::cout <<"Usage: <CNF_file> <scans_limit> <sample_size> <num_iterations> " <<std::endl;
		exit(1);
	}

	Cnf cnf; ReadCNFile(argv[1], cnf);
	int scans_limit = atoi(argv[2]);
	int sample_size = atoi(argv[3]);
	int num_iterations =  atoi(argv[4]);

	int core_len = 177;
	int out_len= 200;


	if (mpi_rank==0){
		// Generate sample
		Sample sample; MakeSample(cnf, core_len, sample, sample_size);
		int num_vars = sample[0].size();
		BitMask out_mask;
		for (int i=0; i<num_vars-out_len; ++i)
			out_mask.push_back(0);
		for (int i=0; i<out_len; ++i)
			out_mask.push_back(1);
		assert(out_mask.size()==sample[0].size());

		// Define starting point
		PointId starting_point;
		for (int i=0; i<num_vars-out_len; ++i)
			starting_point.push_back(1);

		Master master(mpi_size);
		master.Search(num_iterations, starting_point, out_mask, sample);
		master.SendExitSignal();
	}else{
		Worker worker(cnf, scans_limit);
		worker.MainJobCycle();
	}

	return 0;


}
