#define ELPP_NO_DEFAULT_LOG_FILE
#include "utils.h"
#include <iterator>
#include <valarray>
#include "peer.h"
#include "fitness.h"
#include "search/taboo.h"
#include "search/risefall.h"
#include "easylogging++.h"
#include "tclap/CmdLine.h"
#include <set>
#include <utility>

INITIALIZE_EASYLOGGINGPP

MpiBase* mpiS;

#define TINY_SAMPLE_SIZE 10

inline std::string IntVector2String(const std::vector <int> &p ) { std::stringstream out; out << std::setw(5); for (const auto& n: p)  out << n << " " ; return out.str();}



void Search ( RiseFallSearch& rise_fall_search,
		const int num_iterations,
		const int groundLevel,
		const size_t varFixStep)
{

	auto& guessing_vars = rise_fall_search.eval_.guessing_vars;
	std::valarray <size_t> varsOrder (guessing_vars.size());
	std::iota (std::begin(varsOrder), std::end(varsOrder), 0);
	std::valarray <double> varsCount (guessing_vars.size());
	for (size_t k = 0; k < groundLevel; k+=varFixStep)
	{
		std::valarray <size_t> fixedVars (varsOrder[std::slice(0,k,1)]); // Select k first vars in array
		std::valarray <char>   fixedVarsMask (guessing_vars.size());
		fixedVarsMask[fixedVars] = char(1);
		LOG(INFO) << "Fixed vars: " << Vec2String (std::valarray <size_t> (fixedVars+size_t(1)), " ")
			  << "Fixed vars mask: " << Vec2String (fixedVarsMask);

		for (int j = 0; j < num_iterations; ++j)
		{
			//TODO: switch to valarray as PointId base container
			PointId basePoint = PointId (guessing_vars.size(), 0);
			for (int i=0; i<fixedVars.size(); ++i)
				basePoint[fixedVars[i]] = 1; 
			PointStats lastRecord = rise_fall_search (groundLevel, basePoint);
			if (lastRecord.sat_total == 0)
				continue;

			float_t fit = (float_t(1) / pow(2.0, lastRecord.best_incapacity)) / float_t (CountOnes(lastRecord.id));
			for (size_t i = 0; i < lastRecord.id.size(); ++i)
				if (lastRecord.id[i] == 1)
					varsCount[i] += fit;
			std::stable_sort (std::begin(varsOrder), std::end(varsOrder), [&varsCount](size_t a, size_t b) { return varsCount[a] > varsCount[b];});

			std::valarray <double> tmp  = varsCount;
			std::valarray <double> tmp2 = std::log10(tmp);
			LOG(INFO) << "Vars stats: " << Vec2String (tmp2, " ") 
				  << "Vars prio: "  << Vec2String (std::valarray <size_t> (varsOrder+size_t(1)), " ");

			LOG(INFO) << " Final record: " << PrintPointStats(lastRecord, guessing_vars);
		}
	}
}

int main (int argc, char* argv[])
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
	int seconds_limit = 0;
	int sample_size;
	int num_iterations;
	int sat_threshold;
	int num_points;
	SolverType solverType = MINISAT_SOLVER;

	bool modeUnsat = false;

	int core_len;
	int out_len;
	int groundLevel=0;
	std::vector <Clause> varGroups;
	std::vector <PointId> varGroupsMasks;
	std::vector <int> guessing_vars;
	std::vector <PointId> starting_points;
	std::vector <std::vector <char> > extInitStreams;
	BitMask out_mask;
	int num_vars;
	int stallLimit;
	int varFixStep;
	Clause knownVars;
	try
	{
		// Read command line parameters via TCLAP
		TCLAP::CmdLine cmd("This program is used to search for 'inverse backdoor sets'.", ' ', "0.1");
		TCLAP::ValueArg<int> scans_limit_arg	("w", "scans", "Watched literal scans limit for individual solver process.", false, -1,"SCANS_LIMIT", cmd);
		TCLAP::ValueArg<int> seconds_limit_arg	("e", "seconds", "Time limit in seconds for individual solver process.", false, 0,"SECONDS_LIMIT", cmd);
		TCLAP::ValueArg<int> sample_size_arg	("s", "samplesize","Total sample size.", false, 10,"SAMPLE_SIZE", cmd);
		TCLAP::ValueArg<int> num_iterations_arg	("i", "iter","Search iterations limit.", false, 1000,"ITERATIONS_LIMIT", cmd);
		TCLAP::ValueArg<int> sat_threshold_arg	("t", "thresh","Ignore point results if less than this number of units were solved.", false, 1,"SAT_THRESH", cmd);
		TCLAP::ValueArg<int> num_points_arg	("p", "points","Check this number of points before making next step.", false, 1,"SCAN_POINTS", cmd);
		TCLAP::ValueArg<int> corelen_arg	("c", "corelen","Num of core vars.", true, 0,"CORE_LEN", cmd);
		TCLAP::ValueArg<int> outlen_arg		("o", "outlen","Num of out vars.", true, 0,"OUT_LEN", cmd);
		TCLAP::ValueArg<int> groundLevel_arg	("g", "ground","Starting backdoor size for bottom-up search.", false, 0, "GROUND", cmd);
		TCLAP::ValueArg<int> stallLimit_arg	("", "stall_limit","Maximum number of failed search attempts before giving up.", false, 123456789, "STALL_LIMIT", cmd);
		TCLAP::ValueArg<int> varFixStep_arg  ("", "var_step","Fix this number of vars each Rise-Fall iteration.", false, 1, "var_step", cmd);
		TCLAP::ValueArg <std::string> varGroupsFilename_arg ("", "vargroups","Var groups filename (guessing vars derived from var groups).", false, "","VGRPFILENAME", cmd);
		TCLAP::ValueArg <std::string> startingPointsFilename_arg ("", "starting_points","Starting points list filename", false, "","STPFILENAME", cmd);
		TCLAP::ValueArg <std::string> knownVarsFilename_arg ("", "known_vars","Known vars filename.", false, "","KNVFILENAME", cmd);
		TCLAP::ValueArg <std::string> extInitStreamsFilename_arg ("", "streams_file","External init streams (core vars values) filename", false, "","ISFILENAME", cmd);
		TCLAP::SwitchArg modeUnsat_arg ("a", "mode-unsat","Experimental 'UNSAT' mode",cmd, false);
		TCLAP::SwitchArg useLingeling_arg("", "ling","use Lingeling solver engine",cmd, false);
		TCLAP::UnlabeledValueArg<std::string> filename_arg("filename","Path to SAT problem file in DIMACS CNF format.", true, "","CNF_FILENAME", cmd);
		cmd.parse(argc, argv);

		scans_limit = scans_limit_arg.getValue();
		seconds_limit = seconds_limit_arg.getValue();
		solverType = useLingeling_arg.getValue() ? LINGELING_SOLVER: MINISAT_SOLVER;
		ReadCnfFile(filename_arg.getValue().c_str(), cnf);
		//TODO: init workers through MPI 
		// Initialize worker processes
		if (mpi_rank > 0)
		{
			Worker worker(cnf, scans_limit, seconds_limit, 0, solverType);
			worker.MainJobCycle();
			return 0;
		}

		sample_size = sample_size_arg.getValue();
		num_iterations = num_iterations_arg.getValue() ;
		sat_threshold = sat_threshold_arg.getValue() ;
		core_len = corelen_arg.getValue();
		out_len = outlen_arg.getValue();
		num_points = num_points_arg.getValue();
		modeUnsat = modeUnsat_arg.getValue();
		groundLevel = groundLevel_arg.getValue();
		stallLimit = stallLimit_arg.getValue();
		varFixStep = varFixStep_arg.getValue();

		if (varGroupsFilename_arg.isSet())
		{
			varGroups = ReadVarGroupsFile (varGroupsFilename_arg.getValue().c_str());
			// Derive guessing vars set by concatenating var groups file
			for (auto g: varGroups)
				guessing_vars.insert(guessing_vars.end(), g.begin(), g.end());
			std::sort (guessing_vars.begin(), guessing_vars.end());
			// Raise error if duplicates found
			// TODO: better error checking
			for (int i = 1; i < guessing_vars.size(); ++i)
				assert(guessing_vars[i] != guessing_vars[i-1]);
			for (auto g: varGroups)
				varGroupsMasks.push_back (Ints2Point (g, guessing_vars));
		}
		else
		{
			LOG(INFO) << "No var groups / guessing vars file provided. Will solve on core variables.";
			for (int i = 0; i < core_len; ++i)
				guessing_vars.push_back (i + 1);

			varGroupsMasks = HammingNbhd (PointId (core_len, 0), 1);
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
			knownVars = ReadVarGroupsFile (knownVarsFilename_arg.getValue().c_str())[0];
			LOG (INFO) << "Known vars:" << Vec2String (knownVars, " ");
		}
	}
	catch (TCLAP::ArgException &e)
		{std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;}

	auto sample = MakeSample(cnf, core_len, sample_size, extInitStreams);
	num_vars = sample[0].size();

	for (int i = 0; i < num_vars; ++i)
		out_mask.push_back(i < (num_vars - out_len) ? 0 : 1);

	if (!knownVars.empty())
	{
		BitMask knownVarsBM = ExpandBM( BitMask( knownVars.size(), char(1)), knownVars);
		out_mask = BM_or(out_mask, knownVarsBM);
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

	Master master (mpi_size);
	Evaluator eval (master, sample, guessing_vars, out_mask,
			(modeUnsat ? TotalSolvedFitnessFunction : IncapacityFitnessFunction));

	TabooSearch taboo;
	for (auto vec: varGroupsMasks)
		taboo.varPalette_.insert(vec);

	for (auto r: eval(starting_points))
	{
		PointStats* ps = taboo.AddPointResults (r);
		LOG(INFO) << "Starting point: " << PrintPointStats (*ps, eval.guessing_vars);
	}
	RiseFallSearch risefall (eval, taboo, num_points, stallLimit, sat_threshold);
	Search (risefall, num_iterations, groundLevel, varFixStep);

	return 0;
}
