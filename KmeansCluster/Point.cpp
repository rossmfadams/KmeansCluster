#include "Point.h"
#include <sstream>
#include <cfloat>

Point::Point()
{
	//Default Constructor
	clusterId = -1;
	minDist = DBL_MAX;
}

Point::Point(string line)
{
	clusterId = -1;
	minDist = DBL_MAX;
	stringstream is(line);
	double value;
	while (is >> value) {
		values.push_back(value);
	}
}

int Point::getCluster()
{
	return clusterId;
}

double Point::getValue(int pos)
{
	return values[pos];
}

double Point::getMinDist()
{
	return minDist;
}

void Point::setMinDist(double distance)
{
	this->minDist = distance;
}
   
void Point::setCluster(int value)
{
	clusterId = value;
}

void Point::setValueByPos(int pos, double val)
{
	values[pos] = val;
}

double Point::distance(Point & pt)
{
	double sum = 0.0, delta = 0.0;
	for (int i = 0; i < values.size(); i++) {
		delta = pt.getValue(i) - this->getValue(i);
		sum += delta * delta;
	}

	return sum;
}
