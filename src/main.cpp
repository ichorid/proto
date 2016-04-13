#define ELPP_NO_DEFAULT_LOG_FILE
#include "utils.h"
#include "peer.h"
#include "fitness.h"
#include "search/taboo.h"
#include "easylogging++.h"
#include "tclap/CmdLine.h"

INITIALIZE_EASYLOGGINGPP

MpiBase* mpiS;

#define TINY_SAMPLE_SIZE 10

void Search(
		Master master,
		TabooSearch searchEngine,
		const FitnessFunction fitnessFunction,
		const int     num_iterations,
	       	const std::vector <int> guessing_vars,
	       	const BitMask out_mask,
	      	const Sample  sample,
		const int num_points,
		const std::vector <PointId> starting_points = std::vector <PointId> ()
		)
{
	// Stage 0: check starting point if given one
	if (starting_points.size() > 0)
	{
		auto results = master.EvalPoints(starting_points, guessing_vars, out_mask, sample);
		for (auto r: results)
			searchEngine.AddPointResults(fitnessFunction, r);
	}

	// Stage 1: bottom-up climb
	Sample sample_tiny(sample.begin(), sample.begin() + TINY_SAMPLE_SIZE);
	const int try_points = 10;
	for (int i=4; i<num_iterations && searchEngine.origin_queue_.size()==0; ++i)
	{
		auto probe_points = searchEngine.GenerateRandomPoints(i, try_points, guessing_vars.size());
		auto results = master.EvalPoints(probe_points, guessing_vars, out_mask, sample_tiny);
		for (auto r: results)
			searchEngine.AddPointResults(fitnessFunction, r);
	}
	// Stage 2: Search
	for (int i=0; i<num_iterations; ++i)
	{
		auto probe_points = searchEngine.GenerateNewPoints(num_points); 
		auto results = master.EvalPoints(probe_points, guessing_vars, out_mask, sample);
		for (auto r: results)
			searchEngine.AddPointResults(fitnessFunction, r);
	}
}

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
	int num_points;

	bool modeUnsat = false;

	int core_len;
	int out_len;
	int guessing_layer;
	std::vector <int> guessing_vars;
	// Get work mode and CNF file name from commandline
	// Parse mode switch argument
	/*
	std::vector<std::string> workModesNames;
	workModesNames.push_back("search");
	workModesNames.push_back("check_record");
	TCLAP::ValuesConstraint<std::string> allowedWorkModeVals (workModesNames);
	*/
	{
		try
		{
			// Read command line parameters via TCLAP
			TCLAP::CmdLine cmd("This program is used to search for 'inverse backdoor sets'.", ' ', "0.1");
			TCLAP::ValueArg<int> scans_limit_arg	("w", "scans", "Watched literal scans limit for individual solver process.", false, 200000,"SCANS_LIMIT", cmd);
			TCLAP::ValueArg<int> sample_size_arg	("s", "samplesize","Total sample size.", false, 10,"SAMPLE_SIZE", cmd);
			TCLAP::ValueArg<int> num_iterations_arg	("i", "iter","Search iterations limit.", false, 1000,"ITERATIONS_LIMIT", cmd);
			TCLAP::ValueArg<int> sat_threshold_arg	("t", "thresh","Ignore point results if less than this number of units were solved.", false, 1,"SAT_THRESH", cmd);
			TCLAP::ValueArg<int> num_points_arg	("p", "points","Check this number of points before making next step.", false, 1,"SCAN_POINTS", cmd);
			TCLAP::ValueArg<int> corelen_arg	("c", "corelen","Num of core vars.", true, 0,"CORE_LEN", cmd);
			TCLAP::ValueArg<int> outlen_arg		("o", "outlen","Num of out vars.", true, 0,"OUT_LEN", cmd);
			TCLAP::ValueArg<int> guessing_layer_arg	("l", "layer","Index of var layer to search on.", false, 0,"LAYER", cmd);
			TCLAP::ValueArg <std::string> startingPointsFilename_arg ("", "starting_points","Starting points list filename", false, "","STPFILENAME", cmd);
			TCLAP::ValueArg <std::string> extInitStreamsFilename_arg ("", "streams_file","External init streams (core vars values) filename", false, "","ISFILENAME", cmd);
			TCLAP::SwitchArg modeUnsat_arg ("a", "mode-unsat","Experimental 'UNSAT' mode",cmd, false);
			TCLAP::UnlabeledValueArg<std::string> filename_arg("filename","Path to SAT problem file in DIMACS CNF format.", true, "","CNF_FILENAME", cmd);
			cmd.parse(argc, argv);

			// TODO: Rewrite this cpp-style
			std::vector < std::vector <int> > var_layers;
			{
				char filename[4096]; // Maximum Linux path length. No need to conserve bytes nowadays...
				strcpy(filename, filename_arg.getValue().c_str()); // hackish!
				ReadCnfFile(filename, cnf, var_layers);
			}


			scans_limit = scans_limit_arg.getValue();
			sample_size = sample_size_arg.getValue();
			num_iterations = num_iterations_arg.getValue() ;
			sat_threshold = sat_threshold_arg.getValue() ;
			core_len = corelen_arg.getValue();
			out_len = outlen_arg.getValue();
			guessing_layer = guessing_layer_arg.getValue();
			num_points = num_points_arg.getValue();
			modeUnsat = modeUnsat_arg.getValue();
			// Initialize worker processes
			if (mpi_rank > 0)
			{
				Worker worker(cnf, scans_limit);
				worker.MainJobCycle();
				return 0;
			}
			if (var_layers.size()==0)
			{
				LOG(INFO) << " No variable layers data found in CNF file. Will solve on core variables.";
				for (int i=0; i<core_len; ++i)
					guessing_vars.push_back(i+1);
			}
			else
			{
				assert(guessing_layer < var_layers.size());
				guessing_vars = var_layers[guessing_layer];
			}

			std::vector <PointId> starting_points;
			if (startingPointsFilename_arg.isSet())
			{
				char filename[4096];
				strcpy(filename, startingPointsFilename_arg.getValue().c_str());
				starting_points = ReadPointsFile(filename, guessing_vars);
				for (auto a: starting_points)
					LOG (INFO) << "Starting point:" << Point2Varstring (a);
			}

			std::vector < std::vector <char> > extInitStreams;
			if (extInitStreamsFilename_arg.isSet())
			{
				char filename[4096];
				strcpy(filename, extInitStreamsFilename_arg.getValue().c_str());
				extInitStreams = ReadInitStreamsFile(filename);
			}

			//Master master(mpi_size);
			auto sample =  MakeSample(cnf, core_len, sample_size, extInitStreams);
			int num_vars = sample[0].size();
			BitMask out_mask;
			for (int i = 0; i < num_vars; ++i)
				out_mask.push_back(i < (num_vars - out_len) ? 0 : 1);

			// UNSAT mode option - make wrong UC assumptions for sample units
			if (modeUnsat)
			{
				std::random_device rng;
				std::mt19937 mt(rng());
				std::uniform_int_distribution<int> rnd_bit(0,1);
				for (auto &unit: sample)
					for (int i = 0; i < (num_vars-out_len); ++i)
						if (rnd_bit(mt))
							unit[i] *= -1;
			}
			assert(out_mask.size() == sample[0].size());
			// Define starting point
			//auto searchEngine = TabooSearch(sat_threshold);
			//Search(master, searchEngine, IncapacityFitnessFunction, num_iterations, starting_point, guessing_vars, out_mask, sample, num_points);
			Search(
				Master(mpi_size),
			       	TabooSearch(sat_threshold),
			       	(modeUnsat ? TotalSolvedFitnessFunction : IncapacityFitnessFunction),
			       	num_iterations,
			       	guessing_vars,
			       	out_mask,
			       	sample,
			       	num_points,
				starting_points);
		}
		catch (TCLAP::ArgException &e)
		{
			std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		}
	}



	return 0;
}
