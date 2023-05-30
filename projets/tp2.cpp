#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include "mat.h"
#include <vec.h>
#include <array>

void movePositions(std::vector<Point>& positions, Vector& direction) {
    for (unsigned i = 0; i < positions.size(); i++) {
		positions[i] = positions[i] + direction;
	}
}

void getBoundingBox(const std::vector<Point>& positions, Point& pmin, Point& pmax) {
	pmin = positions[0];
	pmax = positions[0];
    for (unsigned i = 1; i < positions.size(); i++) {
		pmin = min(pmin, positions[i]);
		pmax = max(pmax, positions[i]);
	}
}

bool isFaceCulled(std::array<Point, 3> currentTriangle, Point origin) {
	
    Vector edge1 = currentTriangle[1] - currentTriangle[0];
	Vector edge2 = currentTriangle[2] - currentTriangle[0];
	Vector triangleNormal = cross(edge1, edge2);

	if (dot(triangleNormal, origin - currentTriangle[0]) < 0) {
		return true;
	}
	return false;
}

bool isRayInTriangle(std::array<Vector, 3>& currentTriangleNormals, Vector& rayDirection) {
    
    std::array<float, 3> dotProducts;
    
    for (int i = 0; i < 3; i++) {
        dotProducts[i] = dot(currentTriangleNormals[i], rayDirection);
    }
    return (dotProducts[0] >= 0) && (dotProducts[1] >= 0) && (dotProducts[2] >= 0);
}

void drawTriangles(Image& image, const MeshIOData& data) {
    
    std::array<Point, 3> currentTriangle;
    std::array<Vector, 3> currentTriangleNormals;
    std::array<float, 3> dotProducts, lambdas;
    Point origin(0, 0, 0);
    
    // parcours tous les triangles
    for (unsigned i = 0; i + 2 < data.indices.size(); i += 3)
    {
        currentTriangle[0] = data.positions[data.indices[i + 0]];
        currentTriangle[1] = data.positions[data.indices[i + 1]];
        currentTriangle[2] = data.positions[data.indices[i + 2]];

		if (isFaceCulled(currentTriangle, origin)) {
            continue;
		}

        for (int i = 0; i < 3; i++) {
            currentTriangleNormals[i] = normalize(cross(currentTriangle[(i + 2) % 3] - origin, currentTriangle[(i + 1) % 3] - currentTriangle[(i + 2) % 3]));
        }

        for (int py = 0; py < image.height(); py++) {
            for (int px = 0; px < image.width(); px++) {
                float x = (static_cast<float>(px) / image.width()) * 2 - 1;
                float y = (static_cast<float>(py) / image.height()) * 2 - 1;
                float z = -1;
                Point rayOrigin = Point(x, y, z);
                Vector rayDirection = normalize(Vector(origin, rayOrigin));

				if (isRayInTriangle(currentTriangleNormals, rayDirection)) {
                    Color color = data.materials.materials[data.material_indices[i / 3]].diffuse;
                    image(px, py) = color;
                }
            }
        }

    }
}

Image load_mesh(const char* filename) {
   
    Image image(1024, 1024);
    Point pmin, pmax;
	MeshIOData data = read_meshio_data(filename);
    
    movePositions(data.positions, Vector(-300, -300, -800));
    //movePositions(data.positions, Vector(0, -2, -8));
    getBoundingBox(data.positions, pmin, pmax);
    
    drawTriangles(image, data);

    printf("%d triangles\n", int(data.positions.size() / 3));
    printf("bounds [%f %f %f]x[%f %f %f]\n", pmin.x, pmin.y, pmin.z, pmax.x, pmax.y, pmax.z);
    std::cout << "fin de processus" << std::endl;

    return image;
}

int main() {
    
    Image image = load_mesh("data/geometry.obj");
    write_image(image, "image_Scene.png"); // par defaut en .png
    return 0;
}