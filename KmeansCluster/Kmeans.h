#ifndef KMEANS_H
#define KMEANS_H

#include "Point.h"
#include <string>

class Kmeans : public Point
{
private:
	int K, I, D, numRun, totPoints;
	double threshold;
	string filename;
	vector<Point> centroids;

public:
	Kmeans();
	Kmeans(string fn, int K, int iterations, double threshold, int numRun,  int N, int dimensions);
	double run(vector <Point> * allPoints);

};

#endif
