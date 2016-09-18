#include "common.h"
#include "peer.h"

PointStats IncapacityFitnessFunction(const PointResults& results);
PointStats TotalSolvedFitnessFunction(const PointResults& results);

class Evaluator
{
private:
	Master& master;
	FitnessFunction fitFunc;
	BitMask out_mask;
public:
	std::vector <int> guessing_vars;
	Sample& sample;
	std::vector <PointStats> operator()(const std::vector <PointId> &task);
	Evaluator (
		Master& m,
		Sample& s,
		std::vector <int> g,
		BitMask o,
		FitnessFunction f):
		master(m), sample(s), guessing_vars(g), out_mask(o), fitFunc(f) {}

};

std::vector <PointStats> Evaluator::operator()(const std::vector <PointId> &task)
{
	std::vector <PointStats> out;
	for (auto ps: master.EvalPoints(task, guessing_vars, out_mask, sample))
		out.push_back (fitFunc (ps));
	return std::move(out);
}


