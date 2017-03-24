#ifndef VEINBIV_h_
#define VEINVIB_h_
#include <vector>
#include <common.h>
#include "easylogging++.h"
#include <assert.h>
inline PointId construct_d_set_for_66_inputs(const PointId& inp) {
	assert (inp.size() == 66);
	std::vector<std::vector<int>> equations(66);
	for (int i = 0; i < 66; i++) {
		std::vector<int> tmp(4);
		tmp[0] = 66-i;
		tmp[1] = 93 - i;
		tmp[2] = 162 - i;
		tmp[3] = 177 - i;
		equations[i] = tmp;
	}
	std::vector<std::vector<int>> tres;
	for (int i = 0; i < inp.size(); i++) {
		if (inp[i] > 0) {
			tres.push_back(equations[i]);
		}
	}
	std::vector<int> core_vars(177);
	for (int i = 0; i < tres.size(); i++) {
		std::vector<int> tr_i_mod;
		for (int j = 0; j < tres[i].size(); j++) {
			if (core_vars[tres[i][j]-1] == 0) {
				core_vars[tres[i][j]-1] = 1;
				tr_i_mod.push_back(tres[i][j]);
			}				
		}
		tres[i] = tr_i_mod;
	}
	PointId res(177,0);
	for (int i = 0; i < tres.size(); i++)
	{
		for (int j = 0; j < tres[i].size() - 1;j++)
		{
			assert(tres[i][j]<177);
			res[tres[i][j]]=1;
		}
		/*
		for (int j = 0; j < tres[i].size() ; j++) {
			cout << tres[i][j] << " ";
		}
		cout << endl;
		*/
	}
		
	return std::move(res);
}


#endif
