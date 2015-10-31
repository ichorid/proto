#include <iostream>
#include <search/taboo.h>
#include <wrappers/minisat22.h>
#include <utils.h>

int main(int argc, char* argv[])
{
	const char* filename="cnf/bivium_template_new.cnf";
	int core_len = 177;
	int out_len= 200;
	int sample_size = 100;
	int watch_scans_limit = 100000;
	int num_iterations= 1000;
	Cnf cnf; ReadCNFile(filename, cnf);
	Sample sample;
	MakeSample(cnf, core_len, sample, sample_size);
	int num_vars = sample[0].size();
	TabooSearch ts = TabooSearch(sample);
	PointId point;
	for (int i=0; i<core_len; ++i)
		point.push_back(1);

	BitMask out_mask;
	for (int i=0; i<num_vars-out_len; ++i)
		out_mask.push_back(0);
	for (int i=0; i<out_len; ++i)
		out_mask.push_back(1);
	assert(out_mask.size()==sample[0].size());

	for (int i=0; i<num_iterations; ++i){
		Results res;
		Task task = GenTask(BM_or(point,out_mask), sample);
		std::cout << std::endl;
		//for (auto ch: BM_or(point, out_mask)) std::cout << (ch==0 ? 0 : 1) ; std::cout << std::endl;
		for (auto ch: point) std::cout << (ch==0 ? 0 : 1) ; std::cout << std::endl;
		for (auto unit: task){
			Minisat22Wrapper solver;
			solver.InitSolver(cnf);
			solver.AddUCs(unit);
			solver.SetWatchScansLimit(watch_scans_limit);
			solver.Solve();
			res.push_back(solver.GetReport());
		}
		point = ts.ProcessPointResults(point, res);
		PointStats best_point = ts.GetStats();


		auto tmp = ts.origin_queue_.top(); ts.origin_queue_.pop(); auto second_best =  ts.origin_queue_.top(); ts.origin_queue_.push(tmp);

		std::cout << " Best fitness: " << best_point.best_fitness << " Queue size: " <<ts.origin_queue_.size() << " queue top: " << ts.origin_queue_.top()->best_fitness << " second top: " << second_best->best_fitness; 
	}

	std::cout << std::endl;

}

