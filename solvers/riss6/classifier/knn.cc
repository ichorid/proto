/*
 * Classifier.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: Aaron Stephan
 */
#include "classifier/knn.h"
#include "classifier/methods.h"
#include "classifier/knnvar.cc"

#include <iostream>


string knn ( int k, double zero , vector<pair<int, double> >& distances){
    
  sort(distances.begin(), distances.end(), sort_pred());
//  for ( int i = 0; i < 30; ++i ) cerr << distances[i].second << " class: " << distances[i].first << endl;
  
  classEstimation nearestClassNeigbours[amountClassesCC]{{0,0}}; // start counting by zero
  
  int result = 0; // TODO some configuration 
  
  if ( distances[0].second < zero ) result = getNearestPoint(distances, amountClassesCC);
  
  else {
    for (int i = 0; i < k; ++i) {
      nearestClassNeigbours[distances[i].first].first++;
      nearestClassNeigbours[distances[i].first].second += distances[i].second;
    }
    
    //if something happens //TODO
    result = 0;
    classEstimation max = nearestClassNeigbours[0];
    //cerr << nearestClassNeigbours[0].first << " " << nearestClassNeigbours[0].second << endl;

    for (int i = 1; i < amountClassesCC; ++i) { // start by 1 because max is nearestClassNeigbours[0]
      //cerr << nearestClassNeigbours[i].first << " " << nearestClassNeigbours[i].second << endl;
      if (max.first <= nearestClassNeigbours[i].first){
	if (max.first == nearestClassNeigbours[i].first){  // nearestNeighbours << sum of the distance
	  if (max.second > nearestClassNeigbours[i].second){
	    max = nearestClassNeigbours[i];
	    result = i;
	  }
	}
	else{
	  max = nearestClassNeigbours[i];
	  result = i;
	}
      }
    }
  }
  //cout << result << " " << classNamesCC[result] << endl;
  
  return classNamesCC[result];
  
  
}

string computeKNN ( int k, double zero, vector<double>& features){

  vector< vector<double> > points;
  vector<pair<int, double> > distances;
  vector<double> values;   

  
   for ( int i = 0; i < dimensionCC; ++i ){
    values.push_back(features[featureIdentsCC[i].first-1]); // -1 is crucial, because the instance row is missig (is simply no double)
  }
 // cout << values.size() << "size" << endl;
  normalizeDiv(values, divisorsCC); //normalize the input vector as well


  int sizeDataset = allClassCC.size();
  for ( int i = 0; i < sizeDataset; ++i ){
    distances.push_back(pair<int,double>());
    distances[i].first = allClassCC[i];
  }
  
  
  // begin projection
  // create a pca object
  stats::pca pca("dummy");

  //pca.load("pca");
  

 // printVector(values);
  values = pca.to_principal_space(values); //projection
  for (int i = 0; i < sizeDataset; ++i){
    points.push_back(pca.get_principal(i));
  }
  //calculate distance 
  
  
  for ( int i = 0; i < sizeDataset; ++i ){
     distances[i].second = euclideanDistance(values, points[i], pca.get_num_retained());
  }
  
  return (knn(k, zero, distances));
  
}

