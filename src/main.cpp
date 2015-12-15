#include "utils.h"
#include "peer.h"
#include "easylogging++.h"
#include "tclap/CmdLine.h"

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

	// Init MPI
	MpiBase mpi_o(&argc, &argv); mpiS = &mpi_o;
	int mpi_rank; MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);
	int mpi_size; MPI_Comm_size (MPI_COMM_WORLD, &mpi_size);

	// Search parameters
	Cnf cnf;
	int scans_limit;
	int sample_size;
	int num_iterations;
	char filename[4096]; // Maximum Linux path length. No need to conserve bytes nowadays...

	// Read command line parameters via TCLAP
	try {
		TCLAP::CmdLine cmd("This program is used to search for 'inverse backdoor sets'.", ' ', "0.1");
		TCLAP::UnlabeledValueArg<std::string> filename_arg("filename","Path to SAT problem file in DIMACS CNF format.", true, "","CNF_FILENAME"); cmd.add( filename_arg);
		TCLAP::ValueArg<int> scans_limit_arg("w", "scans", "Watched literal scans limit for individual solver process.", false, 200000,"SCANS_LIMIT"); cmd.add( scans_limit_arg);
		TCLAP::ValueArg<int> sample_size_arg("s", "samplesize","Total sample size.", false, 10,"SAMPLE_SIZE"); cmd.add(sample_size_arg);
		TCLAP::ValueArg<int> num_iterations_arg("i", "iter","Search iterations limit.", false, 1000,"ITERATIONS_LIMIT"); cmd.add(num_iterations_arg);
		cmd.parse( argc, argv );

		strcpy(filename, filename_arg.getValue().c_str()); // hackish!
		scans_limit = scans_limit_arg.getValue();
		sample_size = sample_size_arg.getValue();
		num_iterations = num_iterations_arg.getValue() ;

	}catch (TCLAP::ArgException &e){ 
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	int core_len = 177;
	int out_len= 200;
	ReadCNFile(filename, cnf);

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
		//for (int i=0; i<num_vars-out_len; ++i)
		for (int i=0; i<core_len; ++i)
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
