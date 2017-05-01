#ifndef KNN_H
#define KNN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits> 
#include <cmath>
#include <algorithm>
#include <assert.h>
#include "libpca/pca.h"

#include <cmath> // pow()
#include "useful.h"

typedef pair<int, double> classEstimation; 

std::string computeKNN ( int k, double zero, std::vector<double>& features );
std::string knn ( int k, double zero , vector<double>& distances );
double euclideanDistance ( vector<double>& a, vector<double>& b, int dimension);
  
#endif
