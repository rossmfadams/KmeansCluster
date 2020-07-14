#ifndef POINT_H
#define POINT_H

#include <vector>
#include <string>

using namespace std;

class Point
{
private:
	int pointId, clusterId;
	int dimensions;
	double distance;
	vector<double> values;

public:
	Point();
	Point(int id, string line);
	int getDimensions();
	int getCluster();
	int getId();
	double getValue(int pos);
	double getDistance();
	void setDistance(double distance);
	void setCluster(int value);

};

#endif


