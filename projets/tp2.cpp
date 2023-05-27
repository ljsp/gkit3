// exemple de code test a compiler pour verifier que tout est ok
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include <vec.h>
#include <array>

void movePositions(std::vector<Point>& positions, Vector& direction) {
    for (unsigned i = 0; i < positions.size(); i++) {
		positions[i] = positions[i] + direction;
	}
}

void boundingBox(const std::vector<Point>& positions, Point& pmin, Point& pmax) {
	pmin = positions[0];
	pmax = positions[0];
    for (unsigned i = 1; i < positions.size(); i++) {
		pmin = min(pmin, positions[i]);
		pmax = max(pmax, positions[i]);
	}
}

void drawTriangles(Image& image, const std::vector<Point>& positions) {
    std::array<Point, 3> currentTriangle;
    std::array<Vector, 3> currentTriangleNormals;
    std::array<float, 3> V, lambdas;
    Point origin(0, 0, 0);

    // parcours tous les triangles
    for (unsigned i = 0; i + 2 < positions.size(); i += 3)
    {
        currentTriangle[0] = positions[i + 0];
        currentTriangle[1] = positions[i + 1];
        currentTriangle[2] = positions[i + 2];

        for (int i = 0; i < 3; i++) {
            currentTriangleNormals[i] = normalize(cross(currentTriangle[(i + 2) % 3] - origin, currentTriangle[(i + 1) % 3] - currentTriangle[(i + 2) % 3]));
        }

        for (int py = 0; py < image.height(); py++) {
            for (int px = 0; px < image.width(); px++) {
                // rayon
                float x = (static_cast<float>(px) / image.width()) * 2 - 1;
                float y = (static_cast<float>(py) / image.height()) * 2 - 1;
                float z = -1;
                Point p = Point(x, y, z);
                Vector d = Vector(origin, p);

                for (int i = 0; i < 3; i++) {
                    V[i] = dot(currentTriangleNormals[i], d);
                }

                if ((V[0] >= 0) && (V[1] >= 0) && (V[2] >= 0)) {
                    for (int i = 0; i < 3; i++) {
                        lambdas[i] = V[i] / (V[0] + V[1] + V[2]);
                    }
                    image(px, py) = lambdas[0] * Red() + lambdas[1] * Green() + lambdas[2] * Blue();
                }
            }
        }

    }
}

Image read_mesh(const char* filename) {
   
    Image image(1024, 1024);
    std::vector<Point> positions;
    Point pmin, pmax;

    assert(read_positions(filename, positions) != -1);
    
    movePositions(positions, Vector(0, -2, -4));

    boundingBox(positions, pmin, pmax);

    printf("%d triangles\n", int(positions.size() / 3));
    printf("bounds [%f %f %f]x[%f %f %f]\n", pmin.x, pmin.y, pmin.z, pmax.x, pmax.y, pmax.z);

    drawTriangles(image, positions);

    std::cout << "fin de processus" << std::endl;

    return image;
}

int main() {
    
    const char* filename = "data/robot.obj";
    Image image = read_mesh(filename);
    write_image(image, "image_Robot.png"); // par defaut en .png
    return 0;
}