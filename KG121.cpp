#include "KG121.h"

void ScanLine::ReadFile(string filename) {
	ifstream file(filename);
	vector<Vector3f> vertices;
	if (file.is_open()) {
		string line;
		while (file.good()) {
			char c;
			file >> c;
			cout << c;
			switch (c) {
				case 'v':
					double x, y, z;
					file >> x >> y >> z;
					Vector3f vertex(x, y, z);
					if (vertices.size() == 0) {
						xmin = x;
						xmax = x;
						ymin = y;
						ymax = y;
					}
					else {
						if (x < xmin) xmin = x;
						if (x > xmax) xmax = x;
						if (y < ymin) ymin = y;
						if (y > ymax) ymax = y;
					}

					vertices.push_back(vertex);
					n_vertices++;
					break;
				case 'f':
					int vertex_index;
					vector<int> vertex_indices;
					vector<vector<int>> face_indices;
					for ()
					break;

			}
		}
	}
	else {
		cout << "Could not open file" << endl;
		exit(1);
	}
}





int main() {
	RenderWindow window(VideoMode(1000, 1000), "KG121", Style::Close);
	ScanLine scan_line;
	scan_line.ReadFile("polygons.obj");

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear();

		window.display();
	}
}