#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include "mat.h"
#include <vec.h>
#include <array>

const float inf = std::numeric_limits<float>::infinity();

struct Triangle {
    Point a;
    Point b;
    Point c;
    Color color;
};

struct Light
{
    Point p;
    Color color;
};

struct Hit
{
    float t;
    Vector n;
    Color color;

    operator bool() { return t >= 0 && t < inf; } // renvoie vrai si l'intersection existe et quelle est valide...
};

Hit intersect(const Point& o, const Vector& d, const Triangle triangle) {
    
    // Normale
    Vector n = cross(Vector(triangle.a, triangle.b), Vector(triangle.a, triangle.c));

    // Intersection avec le rayon o , d
    float t = dot(n, Vector(o, triangle.a)) / dot(n, d);

    // point d'intersection
    Point p = o + t * d;

    // l'intersection n'est pas valide / derriere l'origine du rayon
    if ((dot(n, cross(Vector(triangle.a, triangle.b), Vector(triangle.a, p))) < 0)
        || (dot(n, cross(Vector(triangle.b, triangle.c), Vector(triangle.b, p))) < 0)
        || (dot(n, cross(Vector(triangle.c, triangle.a), Vector(triangle.c, p))) < 0)
        ) {
        return { INFINITY, Vector(), triangle.color };
    }

    return { t, normalize(n), triangle.color };
    
}
                                                   

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

bool isRayInTriangle(std::array<Vector, 3>& currentTriangleNormals, Vector& rayDirection) {
    
    std::array<float, 3> dotProducts;
    
    for (int i = 0; i < 3; i++) {
        dotProducts[i] = dot(currentTriangleNormals[i], rayDirection);
    }
    return (dotProducts[0] >= 0) && (dotProducts[1] >= 0) && (dotProducts[2] >= 0);
}

void render(Image& image, const MeshIOData& data) {
    
    Point origin(0, 0, 0);
    Light light = { Point(0, 0, 0), Color(0.1f,0.1f,0.2f,1.0f) };
    Triangle triangle;
    Color color;
    Point a, b, c;

    for (int py = 0; py < image.height(); py++) {
        for (int px = 0; px < image.width(); px++) {
            
            float x = (static_cast<float>(px) / image.width()) * 2 - 1;
            float y = (static_cast<float>(py) / image.height()) * 2 - 1;
            float z = -1;
            
            Point rayOrigin = Point(x, y, z);
            Vector rayDirection = Vector(origin, rayOrigin);

            float t = inf;

            for (unsigned i = 0; i + 2 < data.indices.size(); i += 3) {
    
                a = data.positions[data.indices[i + 0]];
                b = data.positions[data.indices[i + 1]];
                c = data.positions[data.indices[i + 2]];
                color = data.materials.materials[data.material_indices[i / 3]].diffuse;

                triangle = { a, b, c, color };

                Hit hit = intersect(rayOrigin, rayDirection, triangle);

			    if (hit && hit.t < t) {

                    t = hit.t;

                    Point p = rayOrigin + hit.t * rayDirection;
                    p = p + hit.n * 0.001; // on décale d'un epsilon
                    Color pixel = Color(0, 0, 0);

                    Vector l = Vector(p, light.p);

                    float cos_theta = std::max(float(0), dot(normalize(hit.n), normalize(l)));

                    Hit i = intersect(p, l, triangle);

                    if (!i || (i.t > 1)) {  // Vérifier ombre
                        pixel = pixel + (light.color * hit.color * cos_theta);
                    }

                    image(px, py) = Color(pixel, 1);
                }
                
            }

        }
    }
}

Image load_mesh(const char* filename) {
   
    Image image(1024, 1024);
    Point pmin, pmax;
	MeshIOData data = read_meshio_data(filename);
    
    movePositions(data.positions, Vector(0, -300, -600));
    //movePositions(data.positions, Vector(0, -2, -8));
    getBoundingBox(data.positions, pmin, pmax);
    
    render(image, data);

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