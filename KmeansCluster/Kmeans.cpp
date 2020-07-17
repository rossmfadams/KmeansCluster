#include "Kmeans.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cfloat>
#include <iomanip>

Kmeans::Kmeans()
{
	// Default Constructor
	filename = "";
	K = 0;
	I = 0;
	threshold = 0.0;
	numRun = 0;
	totPoints = 0;
	D = 0;
	
}

Kmeans::Kmeans(string fn, int K, int iterations, double threshold, int numRun, int N, int dimensions)
{
	this->filename = fn;
	this->K = K;
	this->I = iterations;
	this->threshold = threshold;
	this->numRun = numRun;
	this->totPoints = N;
	this->D = dimensions;
	
}

double Kmeans::run(vector<Point>* allPoints)
{

	// Initializing Clusters
	vector<int> usedPointIds;
	srand(time(0));

	for (int i = 0; i < K; i++) {
		int index = rand() % totPoints;
		centroids.push_back((*allPoints)[index]);
	}
	
	cout << "clusters initialized = " << centroids.size() << endl << endl;

	cout << "Running K-Means Clustering... " << endl;

	int currentIter = 1;
	double prevSSE = 0.0;
	double currentSSE = 0.0;
	bool belowThreshold = false;

	// Open file for writing
	fstream out_file(filename, ios::out | ios::app);

	//Test for opening
	if (!out_file) {
		cout << "Cannot open " << filename << "Output.txt for writing" << endl;
	}
	cout << "\nRun " << numRun << endl;
	out_file << "\nRun " << numRun << endl;
	cout << "-----" << endl;
	out_file << "-----" << endl;

	do {
		cout << "Iteration " << currentIter << ": ";
		out_file << "Iteration " << currentIter << ": ";

		// Add all points to their nearest cluster
		for (vector<Point>::iterator it = allPoints->begin();
			it != allPoints->end(); ++it) {
			Point& p = *it;

			for (vector<Point>::iterator c = begin(centroids);
				c != end(centroids); ++c) {
				int clusterId = c - begin(centroids);
				double dist = c->distance(p);
				if (dist < p.getMinDist()) {
					p.setMinDist(dist);
					p.setCluster(clusterId);
				}
			}

			// Add min distance to SSE
			currentSSE += it->getMinDist();
		}

		// Print total SSE
		cout << "SSE = " << std::setprecision(7) << currentSSE << endl;
		out_file << "SSE = " << std::setprecision(7) << currentSSE << endl;
		
		// Check for threshold
		if (prevSSE > 0.0) {
			if ((abs(currentSSE - prevSSE)/prevSSE) < threshold) {
				belowThreshold = true;
			}
		}
		
		prevSSE = currentSSE; // Save SSE for next iteration
		currentSSE = 0.0;	  // Reset currentSSE

		// Recalculate the centroid of each cluster

		vector<int> nPoints(K,0); // Holds # of points per cluster. Initialize with zeroes
		
		// Reset centroid values to zero
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < D; j++) {
				centroids[i].setValueByPos(j, 0.0);
			}
		}

		// Sum up values for each coordinate and save in centroids vector
		for (int i = 0; i < totPoints; i++) {
			int clusterId = (*allPoints)[i].getCluster();
			nPoints[clusterId] += 1;
			for (int j = 0; j < D; j++) {
				double sum = centroids[clusterId].getValue(j) + (*allPoints)[i].getValue(j);
				centroids[clusterId].setValueByPos(j, sum);
			}

			(*allPoints)[i].setMinDist(DBL_MAX); // reset distance
		}

		// Divide values of centroid coordinates by # of points in cluster to obtain mean
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < D; j++) {
				double mean = centroids[i].getValue(j) / nPoints[i];
				centroids[i].setValueByPos(j, mean);
			}
		}
		
		currentIter++;
	} while (currentIter <= I && !belowThreshold);

	out_file.close();
	return prevSSE;
}
