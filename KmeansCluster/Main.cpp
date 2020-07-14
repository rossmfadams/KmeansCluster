#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <sstream>
#include "Point.h"
#include "Cluster.h"
#include "Kmeans.h"

using namespace std;

int main(int argc, char** argv) {
	int K, I, R, N, D, bestRun;
	double T;
	int pointId = 1; // Initialize beginning of point IDs
	double SSE, minSSE;// Hold return SSE value from kmeans run
	string F;		 // Input filename
	string line;	 // Stores data from input file
	vector<Point> all_points;  // Store all created points
	vector<double> allSSEs;	   // Store all SSEs from all runs

	// Need 5 arguments (except executable filename) to run, else exit
	if (argc != 6) {
		cout << "Error: command-line argument count mismatch.";
		return 1;
	}

	// Initializing Variables
	K = atoi(argv[2]); // K: number of clusters
	I = atoi(argv[3]); // I: maximum number of iterations in a run
	T = atof(argv[4]); // T: convergence threshold
	R = atoi(argv[5]); // R: number of runs

	// Open File for reading
	F = argv[1];
	fstream in_file(F, ios::in);

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
		Point point(pointId, line);
		all_points.push_back(point);
		pointId++;
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

	// Update filename for writing
	F.erase(F.end() - 4,F.end());
	F += "Output.txt";
	// Running K-Means clustering
	for (int i = 0; i < R; i++) {
		Kmeans kmeans(F,K,I,T,(i + 1),N,D);
		SSE = kmeans.run(all_points);
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
	cout << setprecision(5) << "\nBest Run: " << bestRun << " SSE: " << minSSE << endl;
	out_file << setprecision(5) << "\nBest Run: " << bestRun << " SSE: " << minSSE << endl;
	out_file.close();

	return 0;
}