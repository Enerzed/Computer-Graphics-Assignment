#pragma once
// KG121.h
/*
    Эта программа заняла у меня более 100 часов...
*/


#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000


using namespace std;
using namespace sf;
using namespace Glsl;


double DotProduct(Vector3f v1, Vector3f v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3f CrossProduct(Vector3f v1, Vector3f v2) {
	return Vector3f(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

Vector3f Normalize(Vector3f v) {
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return Vector3f(v.x / length, v.y / length, v.z / length);
}

double s2(double x) { return x * x; }

struct Light {
    Vector3f position;
    Color intensity;
    Light() {
        position = Vector3f(1000, 2000, -5000);
        intensity = Color::White;
    }
};

class Indicies {
public:
    vector<int> indicies;
};

class Face {
public:
    int id;
    vector<Indicies> verticies;
};

class Edge {
public:
    int id, dy, ymax, ymin;
    double x, dx, current_x;
};

class Poly {
public:
    int id;
    double a, b, c, d;
    double dy;
    Vector3f normal;
    bool is_in;
    vector<Vector3f> verticies;
    vector<Vector3f> normals_;
    vector<int> indicies_;
    Color color = Color::White;
    Color light;

    // Вычисление z
    double GetZ(double x, double y) {
        return -(a * x + b * y + d) / c;
    }

    void SetColor(Color color) {
        this->color = color;
    }
    
    // Вычисление нормали к полигону
    void CalculateNormal() {
        double vx1 = verticies[0].x - verticies[1].x;
		double vy1 = verticies[0].y - verticies[1].y;
        double vz1 = verticies[0].z - verticies[1].z;
        double vx2 = verticies[1].x - verticies[2].x;
		double vy2 = verticies[1].y - verticies[2].y;
		double vz2 = verticies[1].z - verticies[2].z;

        double nx = vy1 * vz2 - vz1 * vy2;
		double ny = vz1 * vx2 - vx1 * vz2;
		double nz = vx1 * vy2 - vy1 * vx2;

        double wrki = sqrt(s2(vy1*vz2 - vz1*vy2) + s2(vz1*vx2 - vx1*vz2) + s2(vx1*vy2 - vy1*vx2));
        nx /= wrki;
		ny /= wrki;
        nz /= wrki;
        normal = Vector3f(nx, ny, nz);
    }
};

class IntervalScan {
public:
    bool is_init = false;
    vector<Edge> ET;
    vector<Edge> AET;
    vector<Poly> PT;
    vector<Poly> APT;
    vector<Face> FT;
    vector<Vector3f> VT;
    int n_edges = 0, n_polygons = 0, n_faces = 0, n_verticies = 0;
    int xmin, xmax, ymin, ymax, zmin, zmax;
    double d = 0;
    double camera_x = 0;

    void Init(string filename, double d, double camera_x);

    Image Scan(bool is_black_white, Light light, bool is_flat);

    Vec4 SolveABCD(vector<Vector3f> verticies, Indicies indicies) {
        Vec4 p;
        p.x = (verticies[indicies.indicies[1]].y - verticies[indicies.indicies[0]].y) * (verticies[indicies.indicies[2]].z - verticies[indicies.indicies[0]].z) -
            (verticies[indicies.indicies[1]].z - verticies[indicies.indicies[0]].z) * (verticies[indicies.indicies[2]].y - verticies[indicies.indicies[0]].y);
        p.y = (verticies[indicies.indicies[1]].z - verticies[indicies.indicies[0]].z) * (verticies[indicies.indicies[2]].x - verticies[indicies.indicies[0]].x) -
            (verticies[indicies.indicies[1]].x - verticies[indicies.indicies[0]].x) * (verticies[indicies.indicies[2]].z - verticies[indicies.indicies[0]].z);
        p.z = (verticies[indicies.indicies[1]].x - verticies[indicies.indicies[0]].x) * (verticies[indicies.indicies[2]].y - verticies[indicies.indicies[0]].y) -
            (verticies[indicies.indicies[1]].y - verticies[indicies.indicies[0]].y) * (verticies[indicies.indicies[2]].x - verticies[indicies.indicies[0]].x);
        p.w = -(p.x * verticies[indicies.indicies[0]].x + p.y * verticies[indicies.indicies[0]].y + p.z * verticies[indicies.indicies[0]].z);
        return p;
    }

    Poly GetPolygonByID(int id, vector<Poly> polygons) {
        for (int i = 0; i < polygons.size(); i++)
            if (polygons[i].id == id)
                return polygons[i];
    }

    // Старая функция для получения цвета на основе z координаты
    Color GetColor(Color color, double z, double zmin, double zmax, bool is_black_white) {
        double factor = (z - zmin) / (zmax - zmin);
        if (is_black_white)
            return Color(factor * 255, factor * 255, factor * 255);
        return Color(color.r * factor, color.g * factor, color.b * factor);

    }

    Color CalculateLightning(Vector3f vertex, Vector3f normal, Light light) 
    {
        Vector3f lightDir = (light.position - vertex);
        float dotProduct = abs(DotProduct(normal, Normalize(lightDir)));
        return Color
        (
            static_cast<int>(light.intensity.r * dotProduct),
            static_cast<int>(light.intensity.g * dotProduct),
            static_cast<int>(light.intensity.b * dotProduct)
        );
    }

    Color BarycentricInterpolation(const Poly& poly, std::vector<Color> colors, double x, double y) {
        if (poly.indicies_[0] == -1 || poly.indicies_[1] == -1 || poly.indicies_[2] == -1) return Color::Black;
        double area = (poly.verticies[1].y - poly.verticies[2].y) * (poly.verticies[0].x - poly.verticies[2].x) + (poly.verticies[2].x - poly.verticies[1].x) * (poly.verticies[0].y - poly.verticies[2].y);
        double w1 = ((poly.verticies[1].y - poly.verticies[2].y) * (x - poly.verticies[2].x) + (poly.verticies[2].x - poly.verticies[1].x) * (y - poly.verticies[2].y)) / area;
        double w2 = ((poly.verticies[2].y - poly.verticies[0].y) * (x - poly.verticies[2].x) + (poly.verticies[0].x - poly.verticies[2].x) * (y - poly.verticies[2].y)) / area;
        double w3 = 1 - w1 - w2;

        double r = (w1 * colors[0].r + w2 * colors[1].r + w3 * colors[2].r) / (w1 + w2 + w3);
		double g = (w1 * colors[0].g + w2 * colors[1].g + w3 * colors[2].g) / (w1 + w2 + w3);
		double b = (w1 * colors[0].b + w2 * colors[1].b + w3 * colors[2].b) / (w1 + w2 + w3);

        return Color(static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b));
    }
};
