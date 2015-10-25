#include "utils.h"

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

void ReadCNF(IStream& in, Cnf& cnf)
{
	for(;;){
		SkipWhitespace(in);
		if(in.eof()) break;
		//заголовок DIMACS-файла
		else if((*in == 'p') && Match(in, "p cnf")){
			SkipLine(in);
		}
		//пропускаем комментарии
		else if(*in == 'c')
			SkipLine(in);
		else{
			Clause cla;
			ReadClause(in, cla);
			cnf.push_back(cla);
		}

	}
}

void ReadCNFile(const char* file_name, Cnf& cnf)
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
