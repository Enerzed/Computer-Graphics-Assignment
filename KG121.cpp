// KG121.cpp : This file contains the 'main' function
#include "KG121.h"
#include <utility>


void IntervalScan::Init(string filename, double d, double camera_x) 
{
	ifstream file(filename);
	double xobj = 0, yobj = 0, zobj = 0;
	int counter = 0;
	if (file.is_open()) {
		while (!file.eof()) {
			char c;
			file >> c;
			switch (c) {
			case 'v':
				double x, y, z;
				file >> x >> y >> z;
				if (VT.size() == 0) {
					xmax = x;
					xmin = x;
					ymax = y;
					ymin = y;
					zmax = z;
					zmin = z;
				}
				else {
					if (x > xmax) xmax = x;
					if (x < xmin) xmin = x;
					if (y > ymax) ymax = y;
					if (y < ymin) ymin = y;
					if (z > zmax) zmax = z;
					if (z < zmin) zmin = z;
				}
				xobj += x;
				yobj += y;
				zobj += z;
				VT.push_back(Vector3f(x, y, z));
				n_verticies++;
				break;
			case 'f':
				Face face;
				Indicies indicies;
				int vertex_index;
				face.id = n_faces;
				while (file.peek() != '\n' && file.peek() != EOF)
				{
					file >> vertex_index;
					indicies.indicies.push_back(vertex_index - 1);
				}
				face.verticies.push_back(indicies);
				FT.push_back(face);
				n_faces++;
				break;
			}
		}
		file.close();
		double w = WINDOW_HEIGHT * 0.6 / (ymax - ymin);
		xobj /= n_verticies;
		yobj /= n_verticies;
		zobj /= n_verticies;
		xmax = -1e9;
		xmin = 1e9;
		ymax = -1e9;
		ymin = 1e9;
		zmax = -1e9;
		zmin = 1e9;
		for (int i = 0; i < n_verticies; i++) {
			VT[i].x = (VT[i].x - xobj) * w + WINDOW_HEIGHT / 2;
			VT[i].y = (VT[i].y - yobj) * w + WINDOW_HEIGHT / 2;
			VT[i].z = (VT[i].z - zobj) * w + WINDOW_HEIGHT / 2;
			if (VT[i].x > xmax) xmax = VT[i].x;
			if (VT[i].x < xmin) xmin = VT[i].x;
			if (VT[i].y > ymax) ymax = VT[i].y;
			if (VT[i].y < ymin) ymin = VT[i].y;
			if (VT[i].z > zmax) zmax = VT[i].z;
			if (VT[i].z < zmin) zmin = VT[i].z;
		}
		this->d = d;
		this->camera_x = camera_x;
		is_init = true;

		// Apply perspective projection
		for (int i = 0; i < n_verticies; i++) {
			// Perspective projection calculations
			VT[i].x = (VT[i].x  + camera_x) * d / (d + VT[i].z);
			VT[i].y = VT[i].y * d / (d + VT[i].z);


			if (VT[i].x > xmax) xmax = VT[i].x;
			if (VT[i].x < xmin) xmin = VT[i].x;
			if (VT[i].y > ymax) ymax = VT[i].y;
			if (VT[i].y < ymin) ymin = VT[i].y;
			if (VT[i].z > zmax) zmax = VT[i].z;
			if (VT[i].z < zmin) zmin = VT[i].z;

		}

		// build edges and polygons
		for (int i = 0; i < n_faces; i++) {
			// build edges
			for (int j = 0; j < FT[i].verticies[0].indicies.size(); j++) {
				int current = FT[i].verticies[0].indicies[j];
				int next;
				if (j != FT[i].verticies[0].indicies.size() - 1)
					next = FT[i].verticies[0].indicies[j + 1];
				else
					next = FT[i].verticies[0].indicies[0];
				Edge edge;
				edge.id = i;
				//cout << edge.id << endl;
				if (VT[current].y != VT[next].y)
					edge.dx = (VT[current].x - VT[next].x) / (VT[current].y - VT[next].y);
				else
					edge.dx = 0;
				if (VT[current].y > VT[next].y) {
					edge.ymax = VT[current].y;
					edge.ymin = VT[next].y;
					edge.x = VT[current].x;
				}
				else {
					edge.ymax = VT[next].y;
					edge.ymin = VT[current].y;
					edge.x = VT[next].x;
				}
				ET.push_back(edge);
			}
			// build polygons
			Poly* polygon = new Poly;
			polygon->id = i;
			polygon->is_in = false;
			Vec4 abcd = SolveABCD(VT, FT[i].verticies[0]);
			polygon->a = abcd.x;
			polygon->b = abcd.y;
			polygon->c = abcd.z;
			polygon->d = abcd.w;
			polygon->indicies_.push_back(FT[i].verticies[0].indicies[0]);
			polygon->indicies_.push_back(FT[i].verticies[0].indicies[1]);
			polygon->indicies_.push_back(FT[i].verticies[0].indicies[2]);
			polygon->verticies.push_back(VT[FT[i].verticies[0].indicies[0]]);
			polygon->verticies.push_back(VT[FT[i].verticies[0].indicies[1]]);
			polygon->verticies.push_back(VT[FT[i].verticies[0].indicies[2]]);
			//cout << polygon->verticies[0].x << " " << polygon->verticies[0].y << " " << polygon->verticies[0].z << endl;
			//cout << polygon->verticies[1].x << " " << polygon->verticies[1].y << " " << polygon->verticies[1].z << endl;
			//cout << polygon->verticies[2].x << " " << polygon->verticies[2].y << " " << polygon->verticies[2].z << endl;
			polygon->CalculateNormal();
			//cout << polygon.normal.x << " " << polygon.normal.y << " " << polygon.normal.z << endl;
			polygon->color = Color::White;
			PT.push_back(*polygon);
		}

		// По умолчанию нормали к вершинам равны нормали полигона
		for (auto it = PT.begin(); it != PT.end(); it++) {
			it->normals_.push_back(it->normal);
			it->normals_.push_back(it->normal);
			it->normals_.push_back(it->normal);
		}

		// Поиск соседних полигонов и усреднение их нормалей
		for (auto it = PT.begin(); it != PT.end(); it++)
			for (auto it2 = PT.begin(); it2 != PT.end(); it2++) {
				if (it->indicies_[0] == it2->indicies_[0]) {
					it->normals_[0] = Normalize((it->normals_[0] + it2->normals_[0]));
					it2->normals_[0] = Normalize((it->normals_[0] + it2->normals_[0]));
				}
				if (it->indicies_[1] == it2->indicies_[0]) {
					it->normals_[1] = Normalize((it->normals_[1] + it2->normals_[0]));
					it2->normals_[0] = Normalize((it->normals_[1] + it2->normals_[0]));
				}
				if (it->indicies_[2] == it2->indicies_[0]) {
					it->normals_[2] = Normalize((it->normals_[2] + it2->normals_[0]));
					it2->normals_[0] = Normalize((it->normals_[2] + it2->normals_[0]));
				}
				if (it->indicies_[0] == it2->indicies_[1]) {
					it->normals_[0] = Normalize((it->normals_[0] + it2->normals_[1]));
					it2->normals_[1] = Normalize((it->normals_[0] + it2->normals_[1]));
				}
				if (it->indicies_[1] == it2->indicies_[1]) {
					it->normals_[1] = Normalize((it->normals_[1] + it2->normals_[1]));
					it2->normals_[1] = Normalize((it->normals_[1] + it2->normals_[1]));
				}
				if (it->indicies_[2] == it2->indicies_[1]) {
					it->normals_[2] = Normalize((it->normals_[2] + it2->normals_[1]));
					it2->normals_[1] = Normalize((it->normals_[2] + it2->normals_[1]));
				}
				if (it->indicies_[0] == it2->indicies_[2]) {
					it->normals_[0] = Normalize((it->normals_[0] + it2->normals_[2]));
					it2->normals_[2] = Normalize((it->normals_[0] + it2->normals_[2]));
				}
				if (it->indicies_[1] == it2->indicies_[2]) {
					it->normals_[1] = Normalize((it->normals_[1] + it2->normals_[2]));
					it2->normals_[2] = Normalize((it->normals_[1] + it2->normals_[2]));
				}
				if (it->indicies_[2] == it2->indicies_[2]) {
					it->normals_[2] = Normalize((it->normals_[2] + it2->normals_[2]));
					it2->normals_[2] = Normalize((it->normals_[2] + it2->normals_[2]));
				}
			}


		// Background
		{
			Poly background;
			background.id = PT.size() - 1;
			background.is_in = false;
			background.a = 0;
			background.b = 0;
			background.c = -1e12;
			background.d = WINDOW_HEIGHT;
			background.color = Color::Black;
			background.indicies_.push_back(-1);
			background.indicies_.push_back(-1);
			background.indicies_.push_back(-1);
			background.verticies.push_back(Vector3f(0, 0, -1e12));
			background.verticies.push_back(Vector3f(0, WINDOW_HEIGHT, -1e12));
			background.verticies.push_back(Vector3f(WINDOW_WIDTH, WINDOW_HEIGHT, -1e12));
			background.normal = Vector3f(0, 0, 0);
			background.normals_.push_back(background.normal);
			background.normals_.push_back(background.normal);
			background.normals_.push_back(background.normal);
			PT.push_back(background);

			Edge background_edge1;
			background_edge1.id = PT.size() - 1;
			background_edge1.dx = 0;
			background_edge1.ymax = WINDOW_HEIGHT - 1;
			background_edge1.ymin = 0;
			background_edge1.x = 0;
			ET.push_back(background_edge1);

			Edge background_edge2;
			background_edge2.id = PT.size() - 1;
			background_edge2.dx = 0;
			background_edge2.ymax = WINDOW_HEIGHT - 1;
			background_edge2.ymin = 0;
			background_edge2.x = WINDOW_WIDTH;
			ET.push_back(background_edge2);


			Poly background2;
			background2.id = PT.size() - 1;
			background2.is_in = false;
			background2.a = 0;
			background2.b = 0;
			background2.c = -1e12;
			background2.d = WINDOW_HEIGHT;
			background2.color = Color::Black;
			background2.indicies_.push_back(-1);
			background2.indicies_.push_back(-1);
			background2.indicies_.push_back(-1);
			background2.verticies.push_back(Vector3f(0, 0, -1e12));
			background2.verticies.push_back(Vector3f(WINDOW_WIDTH, 0, -1e12));
			background2.verticies.push_back(Vector3f(WINDOW_WIDTH, WINDOW_HEIGHT, -1e12));
			background2.normal = Vector3f(0, 0, 0);
			background2.normals_.push_back(background2.normal);
			background2.normals_.push_back(background2.normal);
			background2.normals_.push_back(background2.normal);
			PT.push_back(background2);

			Edge background_edge12;
			background_edge12.id = PT.size() - 1;
			background_edge12.dx = 0;
			background_edge12.ymax = WINDOW_HEIGHT - 1;
			background_edge12.ymin = 0;
			background_edge12.x = 0;
			ET.push_back(background_edge12);

			Edge background_edge22;
			background_edge22.id = PT.size() - 1;
			background_edge22.dx = 0;
			background_edge22.ymax = WINDOW_HEIGHT - 1;
			background_edge22.ymin = 0;
			background_edge22.x = WINDOW_WIDTH;
			ET.push_back(background_edge22);
			
		}
	}
	else {
		cout << "Unable to open file";
		exit(1);
	}
}

Image IntervalScan::Scan(bool is_black_white, Light light, bool is_flat) {

	Image image;
	image.create(WINDOW_WIDTH, WINDOW_HEIGHT, Color::Black);

	for (int y = WINDOW_HEIGHT - 1; y >= 0; y--) {
		// Add edges to AET
		for (int i = 0; i < ET.size(); i++) {
			if (ET[i].ymax >= ET[i].ymin && ET[i].ymax == y) {
				if (ET[i].ymax > ET[i].ymin)
					AET.push_back(ET[i]);
				ET[i].ymax--;
				ET[i].x -= ET[i].dx;
			}
		}
		// Sort AET by x
		sort(AET.begin(), AET.end(), [](const Edge& a, const Edge& b) { return a.x < b.x; });
		if (AET.empty()) continue;
		for (size_t i = 0; i < AET.size(); i++) {
			int current_id = AET[i].id;
			// Calculate lightning

			PT[current_id].is_in = !PT[current_id].is_in;
			if (PT[current_id].is_in)
				APT.push_back(PT[current_id]);
			else {
				auto it = std::remove_if(APT.begin(), APT.end(), [&](const Poly& poly) { return poly.id == current_id; });
				APT.erase(it, APT.end());
			}


			if (i + 1 < AET.size()) {
				double x_start = AET[i].x;
				double x_end = AET[i + 1].x;

				for (int x = static_cast<int>(x_start); x < static_cast<int>(x_end); ++x) {
					double zmax = -INFINITY;
					int show_id = -1;
					for (auto& poly : APT) {
						double z = poly.GetZ(x, y);
						if (z > zmax) {
							zmax = z;
							show_id = poly.id;
						}
					}
					std::vector<Color> vertexColor;
					for (int v = 0; v < 3; v++)
					{

						//cout << PT[show_id].verticies[v].x << " " << PT[show_id].verticies[v].y << " " << PT[show_id].verticies[v].z << endl;
						if (is_flat)
							vertexColor.push_back(CalculateLightning(PT[show_id].verticies[v], PT[show_id].normal, light));
						else
							vertexColor.push_back(CalculateLightning(PT[show_id].verticies[v], PT[show_id].normals_[v], light));
						//if ((int)vertexColor[v].r != 0 || (int)vertexColor[v].g != 0 || (int)vertexColor[v].b != 0)
						//	std::cout << (int)vertexColor[v].r << " " << (int)vertexColor[v].g << " " << (int)vertexColor[v].b << std::endl;
					}
					if (show_id != PT.size() - 1 && show_id != PT.size() - 2) {

						Color interpolatedColor = BarycentricInterpolation(PT[show_id], vertexColor, x, y);

						image.setPixel(x, WINDOW_HEIGHT - y - 1, interpolatedColor);
					}
				}
			}
		}
		AET.clear();
	}
	cout << "Done" << endl;
	return image;
}

/*
int main() {
	Light light;
	std::string filename = "sphere.obj";
	double initial = -6000.f;
	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LAB 3");
	IntervalScan intervalScan;
	intervalScan.Init(filename, initial, 0.f);
	while (window.isOpen()) {
		Event event;
		IntervalScan copy;

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Left)
					copy.Init(filename, initial, intervalScan.camera_x - 100);
				if (event.key.code == Keyboard::Right)
					copy.Init(filename, initial, intervalScan.camera_x + 100);
			}
		}
		if (copy.is_init == false)
			copy = intervalScan;
		Image image = intervalScan.Scan(true, light, false);
		intervalScan = copy;

		Sprite sprite;
		Texture texture;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		window.clear();
		window.draw(sprite);
		window.display();
	}
}
*/