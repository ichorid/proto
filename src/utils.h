#ifndef UTILS_H_
#define UTILS_H_
#include "common.h"
#include <iostream>
#include <fstream>
#include "assert.h"

class IStream
{
	std::istream& in;
	char c;
public:
	IStream(std::istream& s = std::cin): in(s), c(0){in.get(c);}
	char operator * () {return c;}
	void operator ++ () {in.get(c);}
	bool eof() const {return in.eof();}
};

void SkipLine(IStream& in);
void SkipWhitespace(IStream& in);
bool Match(IStream& in, const char *str);
int  ParseInt(IStream& in);
void ReadClause(IStream& in, Clause& cla);
void ReadCNF(IStream& in, Cnf& cnf);
void ReadCnfFile(const char* file_name, Cnf& cnf, std::vector < std::vector <int> > & var_layers );
void ReadCnfFile(const char* file_name, Cnf& cnf );
std::vector <PointId> ReadPointsList(const char* file_name,  const std::vector <int>& guessing_vars);
Sample MakeSample(const Cnf& cnf, int core_len, int sample_size);

#endif
