#include "common.h"

PointStats IncapacityFitnessFunction(const PointResults& results);
PointStats TotalSolvedFitnessFunction(const PointResults& results);

class Evaluator
{
public:
	Evaluator (Master& master, Sample& sample, std::vector <int> guessingVars, FitnessFunction fitFunc); 
if (lastRecord.id != rec.id)
{
	lastRecord = rec;
	LOG(INFO) << " New record found: " << PrintPointStats (rec, eval.guessingVars);
}
