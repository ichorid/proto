#define ELPP_NO_DEFAULT_LOG_FILE
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
	int sat_threshold;
	char filename[4096]; // Maximum Linux path length. No need to conserve bytes nowadays...

	int core_len;
	int out_len;
	int guessing_layer ;
	std::vector <int> guessing_vars;
	// Read command line parameters via TCLAP
	try {
		TCLAP::CmdLine cmd("This program is used to search for 'inverse backdoor sets'.", ' ', "0.1");
		TCLAP::UnlabeledValueArg<std::string> filename_arg("filename","Path to SAT problem file in DIMACS CNF format.", true, "","CNF_FILENAME"); cmd.add( filename_arg);

		TCLAP::ValueArg<int> scans_limit_arg	("w", "scans", "Watched literal scans limit for individual solver process.", false, 200000,"SCANS_LIMIT"); cmd.add( scans_limit_arg);
		TCLAP::ValueArg<int> sample_size_arg	("s", "samplesize","Total sample size.", false, 10,"SAMPLE_SIZE"); cmd.add(sample_size_arg);
		TCLAP::ValueArg<int> num_iterations_arg	("i", "iter","Search iterations limit.", false, 1000,"ITERATIONS_LIMIT"); cmd.add(num_iterations_arg);
		TCLAP::ValueArg<int> sat_threshold_arg	("t", "thresh","Ignore point results if less than this number of units were solved.", false, 1,"SAT_THRESH"); cmd.add(sat_threshold_arg);
		TCLAP::ValueArg<int> corelen_arg	("c", "corelen","Num of core vars.", true, 0,"CORE_LEN"); cmd.add(corelen_arg);
		TCLAP::ValueArg<int> outlen_arg		("o", "outlen","Num of out vars.", true, 0,"OUT_LEN"); cmd.add(outlen_arg);
		TCLAP::ValueArg<int> guessing_layer_arg	("l", "layer","Index of var layer to search on.", false, 0,"LAYER"); cmd.add(guessing_layer_arg);
		cmd.parse( argc, argv );

		strcpy(filename, filename_arg.getValue().c_str()); // hackish!
		scans_limit = scans_limit_arg.getValue();
		sample_size = sample_size_arg.getValue();
		num_iterations = num_iterations_arg.getValue() ;
		sat_threshold = sat_threshold_arg.getValue() ;
		core_len = corelen_arg.getValue();
		out_len = outlen_arg.getValue();
		guessing_layer = guessing_layer_arg.getValue();

	}catch (TCLAP::ArgException &e){ 
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	std::vector < std::vector <int> > var_layers;
	ReadCNFile(filename, cnf, var_layers);

	if (var_layers.size()==0){
		LOG(INFO) << " No variable layers data found in CNF file. Will solve on core variables.";
		for (int i=0; i<core_len; ++i)
			guessing_vars.push_back(i+1);
	}else{
		assert(guessing_layer<var_layers.size());
		guessing_vars = var_layers[guessing_layer];
	}

	if (mpi_rank==0){
		// Generate sample
		Sample sample;
		MakeSample(cnf, core_len, sample, sample_size);
		int num_vars = sample[0].size();
		BitMask out_mask;
		for (int i = 0; i < num_vars; ++i)
		{
			out_mask.push_back(i < (num_vars - out_len) ? 0 : 1);
		}

		assert(out_mask.size() == sample[0].size());

		// Define starting point
		PointId starting_point;
		for (int i=0; i<guessing_vars.size(); ++i)
			starting_point.push_back(1);

		Master master(mpi_size);
		master.search_engine_.sat_threshold_= sat_threshold;
		master.Search(num_iterations, starting_point, guessing_vars, out_mask, sample);
		master.SendExitSignal();

	}else{
		Worker worker(cnf, scans_limit);
		worker.MainJobCycle();
	}

	return 0;
}
