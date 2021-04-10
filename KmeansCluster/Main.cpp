#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <numeric>
#include <cmath>
#include <sstream>
#include "Point.h"
#include "Kmeans.h"

using namespace std;

/* Normalize data column-wise by coordinate setting the maximum value
   to 1 and minimum value to zero */
void minMaxNormalize(vector<Point>* allPoints, int totPoints, int dimensions) {
	double min, max;

	for (int i = 0; i < dimensions; i++) {
		// Holds the ith value from each point. Initialized to zero
		vector<double> column(totPoints, 0);

		// Fill column vector with ith value from each point
		for (int j = 0; j < totPoints; j++) {
			column[j] = (*allPoints)[j].getValue(i);
		}
		
		// Find min and max value
		auto result = minmax_element(column.begin(), column.end());
		min = *result.first;
		max = *result.second;

		if (max - min == 0) {
			max = 1;
			min = 0;
		}
		// Normalize values to be between 0 and 1 using the formula:
		// v' = (v - min) / (max - min)
		for (int f = 0; f < totPoints; f++) {
			double normalized = ((*allPoints)[f].getValue(i) - min) / (max - min);
			(*allPoints)[f].setValueByPos(i, normalized);
		}
	}
}

/* Normalize data column-wise by coordinate by finding the mean and standard
   deviation of a column and then applying the forumula v' = (v - mean) / stdev
   to each value in the column. */
void zScoreNormalize(vector<Point>* allPoints, int totPoints, int dimensions) {
	double mean, variance = 0.0, stdev, sum, normalized;

	for (int i = 0; i < dimensions; i++) {
		// Holds the ith value from each point. Initialized to zero
		vector<double> column(totPoints, 0);

		// Fill column vector with ith value from each point
		for (int j = 0; j < totPoints; j++) {
			column[j] = (*allPoints)[j].getValue(i);
		}

		// Find mean
		sum = accumulate(column.begin(), column.end(), 0.0);
		mean = sum / totPoints;

		// Find variance
		for (int i = 0; i < totPoints; i++) {
			sum = column[i] - mean;
			variance += sum * sum;
		}
		variance /= totPoints;

		// Find standard deviation
		stdev = sqrt(variance);

		if (stdev == 0) {
			stdev = 1;
		}
		// Normalize values using the formula: v' = (v - mean) / stdev
		for (int j = 0; j < totPoints; j++) {
			normalized = ((*allPoints)[j].getValue(i) - mean) / stdev;
			(*allPoints)[j].setValueByPos(i, normalized);
		}
	}
}

/* Driver. Takes in command-line arguments in the following order:
   [dataFilename] [K] [maxIterations] [threshold] [maxRuns]
   [initializationMethod] [normalizationMethod] 
   
   Calculates the best run based on lowest Sum of Squeared Error */
int main(int argc, char** argv) {
	int K, I, R, N, D, bestRun;
	double T;
	double SSE, minSSE;			// Hold return SSE value from kmeans run
	string F, initType,normType;
	string line;				// Stores data from input file
	vector<Point> all_points;	// Store all created points
	vector<double> allSSEs;		// Store all SSEs from all runs

	// Need 6 arguments (except executable filename) to run, else exit
	if (argc != 8) {
		cout << "Error: command-line argument count mismatch.";
		return 1;
	}

	// Initializing Variables
	K = atoi(argv[2]); // K: number of clusters
	I = atoi(argv[3]); // I: maximum number of iterations in a run
	T = atof(argv[4]); // T: convergence threshold
	R = atoi(argv[5]); // R: number of runs
	initType = argv[6];// Initialization method
	normType = argv[7];// Normalization method

	// Open File for reading
	F = argv[1];		// F: filename
	fstream in_file("C:/Users/rossm/source/repos/KmeansCluster/KmeansCluster/Data_Sets/" + F, ios::in);

	// Test for open
	if (!in_file) {
		cout << "Cannot open " << F << " for input" << endl;
		return 1;
	}

	// Read first line
	getline(in_file, line);
	istringstream(line) >> N >> D; // First line contains N: number of points and
								   // D: dimensionality of each point

	// Read from file and store points
	while (getline(in_file, line)) {
		Point point(line);
		all_points.push_back(point);
	}
	in_file.close();
	
	// Check all points imported
	if (all_points.size() != N) {
		cout << "\nError: incorrect number of data points imported." << endl << endl;
	}
	else
	{
		cout << "\nData Imported sucessfully" << endl << endl;
	}

	// Return if number of clusters > number of points
	if (all_points.size() < K) {
		cout << "Error: Number of clusters greater than number of points." << endl;
	}
	
	// Normalize data
	if (normType == "minmax") {
		minMaxNormalize(&all_points, N, D);
	}
	else if (normType == "zscore") {
		zScoreNormalize(&all_points, N, D);
	}

	// Update filename for writing
	F.erase(F.end() - 4,F.end());
	F += "Output.txt";

	// Running K-Means clustering
	for (int i = 0; i < R; i++) {
		Kmeans kmeans(F,K,I,T,(i + 1),N,D);
		SSE = kmeans.run(&all_points, initType);
		allSSEs.push_back(SSE);
	}

	// Find Best Run
	bestRun = min_element(allSSEs.begin(), allSSEs.end()) - allSSEs.begin() + 1;
	minSSE = *min_element(allSSEs.begin(), allSSEs.end());

	// Output best Run
	fstream out_file(F, ios::app);
	if (!out_file) {
		cout << "Error: Cannot open " << F << " for writing" << endl;
		return 1;
	}
	cout << setprecision(7) << "\nBest Run: " << bestRun << " SSE: " << minSSE << endl;
	out_file << setprecision(7) << "\nBest Run: " << bestRun << " SSE: " << minSSE << endl;
	out_file.close();

	return 0;
}