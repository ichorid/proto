#include "utils.h"
#include "wrappers/minisat22.h"
#include <random>
#include <limits>

void SkipLine(IStream& in)
{
	while(in.eof() || *in != '\n') ++in;
	if(*in == '\n') ++in;
}

bool Match(IStream& in, const char *str)
{
	for (; *str != 0; ++str, ++in)
	if (*str != *in)
		return false;
	return true;
}

void SkipWhitespace(IStream& in)
{
	while ((*in >= 9 && *in <= 13) || *in == 32){
        ++in; 
		if(in.eof()) return;
	}
}

int ParseInt(IStream& in)
{
	int     val = 0;
	bool    neg = false;
	SkipWhitespace(in);
	if      (*in == '-') neg = true, ++in;
	else if (*in == '+') ++in;
	if (*in < '0' || *in > '9')
	       	std::cout << "PARSE ERROR! Unexpected char:" << *in << std::endl, exit(1);
	while (*in >= '0' && *in <= '9')
	{
		if (in.eof()) return 0;
		val = val*10 + (*in - '0'),
		++in;
	}
	return neg ? -val : val;
}

void ReadClause(IStream& in, Clause& cla) 
{
	int parsed_lit;
	assert(cla.size()==0);
	for (;;){
		parsed_lit = ParseInt(in);
		if (parsed_lit == 0) break;
		cla.push_back(parsed_lit);
	}
}

std::vector <char> ReadBitString(IStream& in)
{
	std::vector <char> result;
	SkipWhitespace(in);
	while (*in == '0' ||  *in == '1')
		result.push_back((*in == '0') ? 0 : 1),
			++in;
	return result;
}


void ReadCNF(IStream& in, Cnf& cnf)
{
	for(;;){
		SkipWhitespace(in);
		if(in.eof()) break;
		else if((*in == 'p') && Match(in, "p cnf")){
			SkipLine(in);
		}else if(*in == 'c'){
			SkipLine(in);
		}else{
			Clause cla;
			ReadClause(in, cla);
			cnf.push_back(cla);
		}
	}
}

void ReadCnfFile(const char* file_name, Cnf& cnf)
{
	std::ifstream file(file_name, std::ios::in);

	if(!file.is_open()){
		std::cout << "File " << file_name << " not found!" << std::endl;
		exit(1);
	}
	IStream in(file);
	ReadCNF(in, cnf);
	file.close();
}

std::vector <PointId> ReadPointsFile(const char* file_name,  const std::vector <int>& guessing_vars)
{
	std::ifstream file(file_name, std::ios::in);
	std::vector <Clause> tmp;
	std::vector <PointId> out;
	if(!file.is_open()){
		std::cout << "file " << file_name << " not found!" << std::endl;
		exit(1);
	}
	IStream in(file);
	for(;;){
		SkipWhitespace(in);
		if(in.eof()) 
			break;
		else
		{
			Clause cla;
			ReadClause(in, cla);
			out.push_back (Ints2Point (cla, guessing_vars));
		}

	}
	file.close();
	return out;
}

std::vector <Clause> ReadVarGroupsFile (const char* file_name)
{
	std::vector <Clause> out;
	std::ifstream file(file_name, std::ios::in);
	if(!file.is_open()){
		std::cout << "file " << file_name << " not found!" << std::endl;
		exit(1);
	}
	IStream in(file);
	for(;;){
		SkipWhitespace(in);
		if(in.eof()) 
			break;
		else
		{
			Clause cla;
			ReadClause(in, cla);
			//todo: make work in broken cases. use std algorithms, etc.
			// we assume both vectors are pre-sorted
			std::sort(cla.begin(), cla.end());
			for (auto v: cla)
				assert (v > 0 );
			out.push_back(cla);
		}
	}
	file.close();
	return out;
}

std::vector <std::vector <char> > ReadInitStreamsFile(const char* file_name)
{
	std::ifstream file(file_name, std::ios::in);
	std::vector <std::vector <char> > out;
	if(!file.is_open()){
		std::cout << "File " << file_name << " not found!" << std::endl;
		exit(1);
	}
	IStream in(file);
	for(;;){
		SkipWhitespace(in);
		if(in.eof())
			break;
		else
			out.push_back(ReadBitString(in));
	}
	file.close();
	return out;
}

Sample MakeSample(const Cnf& cnf, int core_len, int sample_size, std::vector <std::vector <char> > initStream)
{
	const bool randomMode = initStream.empty();
	Sample sample;
	std::random_device rng;
	std::mt19937 mt(rng());
	std::uniform_int_distribution<int> rnd_bit(0,1);
	std::uniform_int_distribution<int> rnd_int1max(1,std::numeric_limits<int>::max());
	Minisat22Wrapper solver;
	solver.InitSolver(cnf);
	// Little hack to randomize polarity of non-core vars in some corner
	// cases, e.g. in block ciphers. It is not super correct, since it
	// relies on solver's rng. Minisat's rng is weak, by the way. 
	if (randomMode)
		solver.Randomize(rnd_int1max(mt));
	for (int j=0; j<sample_size; ++j){
		if (!randomMode)
			assert (initStream[j].size() == core_len);
		// Create core vector
		UnitClauseVector coreLits;
		for (int i = 0; i < core_len; ++i)
			coreLits.push_back ((i + 1) * (-1 + 2 * (randomMode ?  rnd_bit(mt) : initStream[j][i])));
		solver.Solve(coreLits);
		assert (solver.GetReport().state == SAT);
		sample.push_back(solver.GetSolution());
	}
	return sample;
}
