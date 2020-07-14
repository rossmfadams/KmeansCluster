#ifndef KMEANS_H
#define KMEANS_H

#include "Cluster.h"
#include <string>

class Kmeans : public Cluster
{
private:
	int K, I, D, numRun, totPoints;
	double threshold;
	string filename;
	vector<Cluster> clusters;

	int getNearestClusterId(Point *pt);

public:
	Kmeans();
	Kmeans(string fn, int K, int iterations, double threshold, int numRun,  int N, int dimensions);
	double run(vector <Point> & allPoints);

};

#endif
