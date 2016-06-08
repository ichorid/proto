#include "utils.h"
#include "wrappers/minisat22.h"
#include <random>
#include "easylogging++.h"

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
		val = val*10 + (*in - '0'),
		++in;
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


void ReadCNF(IStream& in, Cnf& cnf, std::vector < std::vector <int> > & var_layers )
{
	for(;;){
		SkipWhitespace(in);
		if(in.eof()) break;
		else if((*in == 'p') && Match(in, "p cnf")){
			SkipLine(in);
		}else if(*in == 'c'){
			if (Match(in, "c begin_layers_list")){
				SkipLine(in);
				//std::cout<< std::endl << "LAYERS BEGIN";
				while (false==Match(in, "c end_layers_list")){
					//std::cout<< std::endl << "LAYER";
					Clause layer;
					//++in; // skip 'c'
					ReadClause(in, layer);
					var_layers.push_back(layer);
					SkipLine(in);
				}
			}
			SkipLine(in);
		}else{
			Clause cla;
			ReadClause(in, cla);
			cnf.push_back(cla);
		}

	}
}

void ReadCnfFile(const char* file_name, Cnf& cnf, std::vector <std::vector<int> > & var_layers )
{
	std::ifstream file(file_name, std::ios::in);

	if(!file.is_open()){
		std::cout << "File " << file_name << " not found!" << std::endl;
		exit(1);
	}
	IStream in(file);
	ReadCNF(in, cnf, var_layers);
	file.close();
}

void ReadCnfFile(const char* file_name, Cnf& cnf ) 
{ 
	std::vector < std::vector <int> > dummy_layers; 
	ReadCnfFile(file_name, cnf, dummy_layers); 
}

std::vector <PointId> ReadPointsFile(const char* file_name,  const std::vector <int>& guessing_vars)
{
	std::ifstream file(file_name, std::ios::in);
	std::vector <Clause> tmp;
	std::vector <PointId> out;
	if(!file.is_open()){
		std::cout << "File " << file_name << " not found!" << std::endl;
		exit(1);
	}
	IStream in(file);
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
			//TODO: make work in broken cases. Use std algorithms, etc.
			// We assume both vectors are pre-sorted
			int i = 0;
			PointId point;
			std::sort(cla.begin(), cla.end());
			for (auto v: cla)
			{
				while (v != guessing_vars[i++])
				{
					point.push_back(0);
					assert(i < guessing_vars.size());
				}
				point.push_back(1);
			}

			while (i++ < guessing_vars.size())
				point.push_back(0);

			assert (point.size()==guessing_vars.size());
			out.push_back(point);
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
	Minisat22Wrapper solver;
	solver.InitSolver(cnf);
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
