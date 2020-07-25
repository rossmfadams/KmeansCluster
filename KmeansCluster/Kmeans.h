#ifndef KMEANS_H
#define KMEANS_H

#include "Point.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cfloat>
#include <iomanip>

using namespace std;

class Kmeans : public Point
{
private:
	int K, I, D, numRun, totPoints;
	double threshold;
	string filename;
	vector<Point> centroids;

public:
	/* Default Constructor */
	Kmeans()
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

	/* Constructor */
	Kmeans(string fn, int K, int iterations, double threshold, int numRun, int N, int dimensions)
	{
		this->filename = fn;
		this->K = K;
		this->I = iterations;
		this->threshold = threshold;
		this->numRun = numRun;
		this->totPoints = N;
		this->D = dimensions;

	}

	/* Calculates the centeroid of a cluster based on the mean of all coordinate values 
	   of points within the cluster */
	void calculateCentroids(vector<Point>* allPoints) {
		vector<int> nPoints(K, 0);
		vector<Point> resetCentroids;
		
		// Create point vector of empty points
		for (int i = 0; i < K; i++) {
			Point pt(D);
			resetCentroids.push_back(pt);
		}
		
		// Swap empty point vector with centroid vector to reset values
		centroids.swap(resetCentroids);

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
	}

	/* Randomly selects K points as the initial clusters */
	void randSelection(vector<Point>* allPoints) {
		srand(time(0));

		for (int i = 0; i < K; i++) {
			int index = rand() % totPoints;
			centroids.push_back((*allPoints)[index]);
		}

	}

	/* Assigns each point to a cluster selected uniformly at random */
	void randPartition(vector<Point>* allPoints) {
		srand(time(0));

		for (int i = 0; i < totPoints; i++) {
			int assignment = rand() % K;
			(*allPoints)[i].setCluster(assignment);
		}

		/// Calculate centroids of randomly partitioned clusters
		calculateCentroids(allPoints);
	}

	/* Arbitrarily sets the first centroid as the mean of all coordinates of all
	   points in the data set. Then incrementally selects the point furthest from
	   all previously selected centroids to be the next centroid until K centroids
	   are selected */
	void maximin(vector<Point>* allPoints) {
		vector<double> meanValues(D, 0.0);
		Point pt(D);
		int max_dist_index = 0;
		double dist, max_dist;

		// Initialize first centroid as average of all point values
		for (int i = 0; i < totPoints; i++) {
			for (int j = 0; j < D; j++) {
				meanValues[j] += (*allPoints)[i].getValue(j) / totPoints;
			}
		}
		
		for (int i = 0; i < D; i++) {
			pt.setValueByPos(i, meanValues[i]);
		}

		centroids.push_back(pt);

		// Choose the remaining centers using maximin
		for (int i = 1; i < K; i++) {
			max_dist = DBL_MIN;
			for (int j = 0; j < totPoints; j++) {
				// Compute points distance to previously chosen centroid
				dist = centroids[i - 1].distance((*allPoints)[j]);

				if (dist < (*allPoints)[j].getMinDist()) {
					// Update nearest-centroid-distance for this point
					(*allPoints)[j].setMinDist(dist);
				}
				if (max_dist < (*allPoints)[j].getMinDist()) {
					// Update the maximum nearest-centroid-distance so far
					max_dist = (*allPoints)[j].getMinDist();
					max_dist_index = j;
				}
			}

			// Point with maximum distance to its nearest center is chosen as a centroid
			centroids.push_back((*allPoints)[max_dist_index]);
		}

	}

	/* The batch (Lloyd's) K-mean algorithm 
	   returns the Sum of Squared Error */
	double run(vector<Point>* allPoints, string initType = "randSelection")
	{

		// Initializing Clusters
		if (initType == "randSelection") {
			randSelection(allPoints);
		}
		else if (initType == "randPartition") {
			randPartition(allPoints);
		}
		else if (initType == "maximin") {
			maximin(allPoints);
		}


		cout << "clusters initialized = " << centroids.size() << endl << endl;

		cout << "Running K-Means Clustering... " << endl;

		int currentIter = 1;
		double prevSSE = INT_MAX;
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
					if (dist <= p.getMinDist()) {
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
			if ((abs(currentSSE - prevSSE) / prevSSE) < threshold) {
				belowThreshold = true;
			}
			
			prevSSE = currentSSE; // Save SSE for next iteration
			currentSSE = 0.0;	  // Reset currentSSE

			// Recalculate the centroid of each cluster
			calculateCentroids(allPoints);

			currentIter++;
		} while (currentIter <= I && !belowThreshold);

		out_file.close();
		return prevSSE;
	}

};

#endif
