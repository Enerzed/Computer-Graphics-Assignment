#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Edge {
public:
	int id, dy;
	double x, dx;
};

class Polygon {
public:
	int id;
	Glsl::Vec4 abcd;
	double dy;
	bool is_in = false;
	Color color;
};

class Interval {
public:
	int id = -1;
	int xl, xr;
};

class ScanLine {
public:
	vector<Edge> ET;
	vector<Polygon> PT;
	vector<Interval> IT;
	vector<Vector3f> vertices;
	int n_vertices = 0;
	double ymin, ymax, xmin, xmax;

    void Scan();
	void ReadFile(string filename);

};