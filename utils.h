#ifndef PROTO_UTILS_H_
#define PROTO_UTILS_H_
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
void ReadCNFile(const char* file_name, Cnf& cnf);

#endif
