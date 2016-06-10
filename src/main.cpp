#define ELPP_NO_DEFAULT_LOG_FILE
#include "utils.h"
#include <iterator>
#include <valarray>
#include "peer.h"
#include "fitness.h"
#include "search/taboo.h"
#include "easylogging++.h"
#include "tclap/CmdLine.h"

INITIALIZE_EASYLOGGINGPP

MpiBase* mpiS;

#define TINY_SAMPLE_SIZE 10

inline std::string IntVector2String(const std::vector <int> &p ) { std::stringstream out; out << std::setw(5); for (const auto& n: p)  out << n << " " ; return out.str();}


PointStats RiseFallSearch (
		Master& master,
		TabooSearch& searchEngine,
		const FitnessFunction& fitnessFunction,
	       	const std::vector <int> guessing_vars,
	       	const BitMask out_mask,
	      	const Sample  sample,
		const std::valarray <size_t> &fixedVars,
		const int num_points,
		const int groundLevel,
		const int stallLimit
		)
{
	PointId basePoint = PointId (guessing_vars.size(), 0);
	//TODO: switch to valarray as PointID base container
	for (int i=0; i<fixedVars.size(); ++i)
		basePoint[fixedVars[i]] = 1; 
	assert (CountOnes(basePoint) <= groundLevel);

	LOG(INFO) << " STAGE 1 - RISE";
	Sample sample_tiny (sample.begin (), sample.begin () + TINY_SAMPLE_SIZE);
	const int try_points = 10;
	for (int i = groundLevel; i <= guessing_vars.size () && searchEngine.origin_queue_.empty(); ++i)
	{
		auto probe_points = searchEngine.GenerateRandomPoints (i, try_points, basePoint);
		auto results = master.EvalPoints (probe_points, guessing_vars, out_mask, sample_tiny);
		for (const auto &r: results)
			searchEngine.AddPointResults (fitnessFunction, r);
	}

	LOG(INFO) << " STAGE 2 - FALL";
	PointStats lastRecord;
	for (int stallCount=0; !searchEngine.origin_queue_.empty() && (stallCount < stallLimit);)
	{
		auto probe_points = searchEngine.GenerateNewPoints (num_points, basePoint); 
		auto results = master.EvalPoints (probe_points, guessing_vars, out_mask, sample);
		for (const auto &r: results)
			searchEngine.AddPointResults (fitnessFunction, r);
		if (searchEngine.GetCurrentRecord ().id == lastRecord.id)
		{
			++stallCount;
		}
		else
		{
			lastRecord = searchEngine.GetCurrentRecord ();
			stallCount = 0;
		}
	}
	while(!searchEngine.origin_queue_.empty ()) searchEngine.origin_queue_.pop ();
	searchEngine.ResetCurrentRecord ();

	return lastRecord;
}




void Search 	(
		Master master,
		TabooSearch searchEngine,
		const FitnessFunction fitnessFunction,
		const int num_iterations,
	       	const std::vector <int> guessing_vars,
	       	const BitMask out_mask,
	      	const Sample  sample,
		const int num_points,
		const int groundLevel,
		const int stallLimit,
		const size_t varFixStep,
		const std::vector <PointId> starting_points = std::vector <PointId> ()
		)
{

	if (starting_points.size() > 0)
	{
		LOG(INFO) << " STARTING POINT CHECK";
		auto results = master.EvalPoints(starting_points, guessing_vars, out_mask, sample);
		for (auto r: results)
			searchEngine.AddPointResults(fitnessFunction, r);
			searchEngine.ResetCurrentRecord ();
	}

	std::valarray <size_t> varsOrder (guessing_vars.size());
	std::iota (std::begin(varsOrder), std::end(varsOrder), 0);
	std::valarray <double> varsCount (guessing_vars.size());
	std::vector <PointStats> localRecords;
	for (size_t k = 0; k < groundLevel; k+=varFixStep)
	{
		std::valarray <size_t> fixedVars (varsOrder[std::slice(0,k,1)]); // Select k first vars in array
		std::valarray <char>   fixedVarsMask (guessing_vars.size());
		fixedVarsMask[fixedVars] = char(1);
		LOG(INFO) << "Fixed vars: " << Vec2String (std::valarray <size_t> (fixedVars+size_t(1)), " ")
			  << "Fixed vars mask: " << Vec2String (fixedVarsMask);
		for (int j = 0; j < num_iterations; ++j)
		{
			PointStats lastRecord = RiseFallSearch (
				master,
				searchEngine,
				fitnessFunction,
				guessing_vars,
				out_mask,
				sample,
				fixedVars,
				num_points,
				groundLevel,
				stallLimit);

			if (lastRecord.sat_total == 0)
				continue;
			float_t fit = (float_t(1) / pow(2.0, lastRecord.best_incapacity)) / float_t (CountOnes(lastRecord.id));
			for (size_t i = 0; i < lastRecord.id.size(); ++i)
				if (lastRecord.id[i] == 1)
					varsCount[i] += fit;
			localRecords.push_back (lastRecord);
			std::stable_sort (std::begin(varsOrder), std::end(varsOrder), 
					[&varsCount](size_t a, size_t b) { return varsCount[a] > varsCount[b];});
			std::valarray <double> tmp = varsCount;
			std::valarray <double> tmp2 = std::log10(tmp);
			LOG(INFO) << "Vars stats: " << Vec2String (tmp2, " ") 
				  << "Vars prio: "  << Vec2String (std::valarray <size_t> (varsOrder+size_t(1)), " ");

			//FIXME: make db private again, implement this
			//as a procedure
		PointStats *ps = &lastRecord;
		LOG(INFO) << " Final record: " 
			<< std::setw(5) << CountOnes(ps->id) << " "
			<< std::setw(8) << std::setprecision(2) << std::fixed << ps->best_incapacity << " "    
			<< std::setw(12) << std::scientific << pow(2.0, ps->best_incapacity) << " "    
			<< "W: " << std::setw(8) << std::scientific << ps->best_cutoff << " "    
			<< std::setw(5) << ps->sat_total << " /" 
			<< std::setw(5) << sample.size() << " " 
			<< Point2Bitstring(ps->id) << " ccc "
			<< Point2Varstring(ps->id) ; // FIXME: expand vars according to the mask
		}
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
	SolverType solverType = MINISAT_SOLVER;

	bool modeUnsat = false;

	int core_len;
	int out_len;
	int guessing_layer;
	int groundLevel=0;
	std::vector <int> guessing_vars;
	std::vector <std::vector <int> > var_layers;
	std::vector <PointId> starting_points;
	std::vector <std::vector <char> > extInitStreams;
	BitMask out_mask;
	int num_vars;
	int stallLimit;
	int varFixStep;
	PointId knownVars;
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
		TCLAP::ValueArg<int> groundLevel_arg	("g", "ground","Starting backdoor size for bottom-up search.", false, 0, "GROUND", cmd);
		TCLAP::ValueArg<int> stallLimit_arg	("", "stall_limit","Maximum number of failed search attempts before giving up.", false, 123456789, "STALL_LIMIT", cmd);
		TCLAP::ValueArg<int> varFixStep_arg  ("", "var_step","Fix this number of vars each Rise-Fall iteration.", false, 1, "var_step", cmd);
		TCLAP::ValueArg <std::string> startingPointsFilename_arg ("", "starting_points","Starting points list filename", false, "","STPFILENAME", cmd);
		TCLAP::ValueArg <std::string> knownVarsFilename_arg ("", "known_vars","Known vars filename.", false, "","KNVFILENAME", cmd);
		TCLAP::ValueArg <std::string> extInitStreamsFilename_arg ("", "streams_file","External init streams (core vars values) filename", false, "","ISFILENAME", cmd);
		TCLAP::SwitchArg modeUnsat_arg ("a", "mode-unsat","Experimental 'UNSAT' mode",cmd, false);
		TCLAP::SwitchArg useLingeling_arg("", "ling","use Lingeling solver engine",cmd, false);
		TCLAP::UnlabeledValueArg<std::string> filename_arg("filename","Path to SAT problem file in DIMACS CNF format.", true, "","CNF_FILENAME", cmd);
		cmd.parse(argc, argv);

		scans_limit = scans_limit_arg.getValue();
		sample_size = sample_size_arg.getValue();
		num_iterations = num_iterations_arg.getValue() ;
		sat_threshold = sat_threshold_arg.getValue() ;
		core_len = corelen_arg.getValue();
		out_len = outlen_arg.getValue();
		guessing_layer = guessing_layer_arg.getValue();
		num_points = num_points_arg.getValue();
		modeUnsat = modeUnsat_arg.getValue();
		groundLevel = groundLevel_arg.getValue();
		stallLimit = stallLimit_arg.getValue();
		varFixStep = varFixStep_arg.getValue();
		solverType = useLingeling_arg.getValue() ? LINGELING_SOLVER: MINISAT_SOLVER;
		ReadCnfFile(filename_arg.getValue().c_str(), cnf, var_layers);

		// Initialize worker processes
		if (mpi_rank > 0)
			goto worker_thread;
		if (var_layers.size() == 0)
		{
			LOG(INFO) << " No variable layers data found in CNF file. Will solve on core variables.";
			for (int i = 0; i < core_len; ++i)
				guessing_vars.push_back (i + 1);
		}
		else
		{
			assert(guessing_layer < var_layers.size());
			guessing_vars = var_layers[guessing_layer];
		}

		if (startingPointsFilename_arg.isSet())
		{
			starting_points = ReadPointsFile(startingPointsFilename_arg.getValue().c_str(), guessing_vars);
			for (auto a: starting_points)
				LOG (INFO) << "Starting point:" << Point2Varstring (a);
		}

		if (extInitStreamsFilename_arg.isSet())
			extInitStreams = ReadInitStreamsFile(extInitStreamsFilename_arg.getValue().c_str());

		if (knownVarsFilename_arg.isSet())
		{
			knownVars = ReadPointsFile(knownVarsFilename_arg.getValue().c_str(), guessing_vars)[0];
			LOG (INFO) << "Known vars:" << Point2Varstring (knownVars);
		}
	}
	catch (TCLAP::ArgException &e)
		{std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;}

worker_thread:
	if (mpi_rank > 0)
	{
		Worker worker(cnf, scans_limit, 0, solverType);
		worker.MainJobCycle();
	}
	else
	{
		auto sample = MakeSample(cnf, core_len, sample_size, extInitStreams);
		num_vars = sample[0].size();

		for (int i = 0; i < num_vars; ++i)
			out_mask.push_back(i < (num_vars - out_len) ? 0 : 1);
		// Remove known vars from guessing vars
		if (!knownVars.empty())
		{
			guessing_vars = RemoveIntsFromVectorByBitMask (guessing_vars, knownVars); 
			out_mask = BM_or(out_mask, knownVars);
		}
		assert(out_mask.size() == sample[0].size());
		// UNSAT mode option - make wrong UC assumptions for sample units
		if (modeUnsat)
		{
			std::random_device rng;
			std::mt19937 mt(rng());
			std::uniform_int_distribution <int> rnd_bit (0,1);
			for (auto &unit: sample)
				for (int i = 0; i < (num_vars - out_len); ++i)
					if (rnd_bit(mt))
						unit[i] *= -1;
		}
		Search (Master(mpi_size),
			TabooSearch(sat_threshold),
			(modeUnsat ? TotalSolvedFitnessFunction : IncapacityFitnessFunction),
			num_iterations,
			guessing_vars,
			out_mask,
			sample,
			num_points,
			groundLevel,
			stallLimit,
			varFixStep,
			starting_points);
	}

	return 0;
}
