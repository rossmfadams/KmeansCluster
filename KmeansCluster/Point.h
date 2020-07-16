#ifndef POINT_H
#define POINT_H

#include <vector>
#include <string>

using namespace std;

class Point
{
private:
	int clusterId;
	int dimensions;
	double minDist;
	vector<double> values;

public:
	Point();
	Point(string line);
	int getCluster();
	double getValue(int pos);
	double getMinDist();
	void setMinDist(double distance);
	void setCluster(int value);
	void setValueByPos(int pos, double val);
	double distance(Point pt);

};

#endif


