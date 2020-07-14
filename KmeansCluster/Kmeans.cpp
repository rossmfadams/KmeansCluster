#include "Kmeans.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

int Kmeans::getNearestClusterId(Point *pt)
{
	double sum = 0.0;
	int nearestClusterId;

	for (int i = 0; i < D; i++) {
		sum += (clusters[0].getCentroidByPos(i) - pt->getValue(i)) * (clusters[0].getCentroidByPos(i) - pt->getValue(i));
	}

	pt->setDistance(sum);
	nearestClusterId = clusters[0].getId();

	for (int i = 1; i < K; i++) {

		sum = 0.0;

		for (int j = 0; j < D; j++) {
			sum += (clusters[i].getCentroidByPos(j) - pt->getValue(j)) * (clusters[i].getCentroidByPos(j) - pt->getValue(j));
		}

		if (sum < pt->getDistance()) {
			pt->setDistance(sum);
			nearestClusterId = clusters[i].getId();
		}
	}
	return nearestClusterId;
}

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

double Kmeans::run(vector<Point>& allPoints)
{

	// Initializing Clusters
	vector<int> usedPointIds;
	srand(time(0));

	for (int i = 1; i <= K; i++) {
		bool unused = false;

		do {
			int index = rand() % totPoints;

			if (find(usedPointIds.begin(), usedPointIds.end(), index) == usedPointIds.end()) {
				unused = true;
				usedPointIds.push_back(index);
				allPoints[index].setCluster(i);
				Cluster cluster(i, allPoints[index]);
				clusters.push_back(cluster);
			}
		} while (!unused);
	}
	
	cout << "clusters initialized = " << clusters.size() << endl << endl;

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
		for (int i = 0; i < totPoints; i++) {
			int currentClusterId = allPoints[i].getCluster();
			int nearestClusterId = getNearestClusterId(&allPoints[i]);

			// Add point contribution to SSE
			currentSSE += allPoints[i].getDistance();

			if (currentClusterId != nearestClusterId) {
				if (currentClusterId != 0) {
					for (int j = 0; j < K; j++) {
						if (clusters[j].getId() == currentClusterId) {
							clusters[j].removePoint(allPoints[i].getId());
						}
					}
				}

				for (int j = 0; j < K; j++) {
					if (clusters[j].getId() == nearestClusterId) {
						clusters[j].addPoint(allPoints[i]);
					}
				}

				allPoints[i].setCluster(nearestClusterId);
			}
		}

		// Print total SSE
		cout << "SSE = " << std::setprecision(5) << currentSSE << endl;
		out_file << "SSE = " << std::setprecision(5) << currentSSE << endl;
		
		// Check for threshold
		if (prevSSE > 0.0) {
			if ((abs(currentSSE - prevSSE)/prevSSE) < threshold) {
				belowThreshold = true;
			}
		}
		
		prevSSE = currentSSE; // Save SSE for next iteration
		currentSSE = 0.0;	  // Reset currentSSE

		// Recalculate the centroid of each cluster
		for (int i = 0; i < K; i++) {
			int clusterSize = clusters[i].getSize();
			for (int j = 0; j < D; j++) {
				double sum = 0.0;
				if (clusterSize > 0) {
					for (int p = 0; p < clusterSize; p++) {
						sum += clusters[i].getPoint(p).getValue(j);
					}
					clusters[i].setCentroidByPos(j, sum / clusterSize);
				}
			 }
		}

		currentIter++;
	} while (currentIter <= I && !belowThreshold);

	out_file.close();
	return prevSSE;
}
