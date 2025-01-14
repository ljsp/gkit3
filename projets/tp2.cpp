#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include "mat.h"
#include <vec.h>
#include <array>

struct Triangle {
    Point a;
    Point b;
    Point c;
    Color color;
};

struct Light
{
    Point p;
    float radius;
    Color color;
};

struct Hit
{
    float t;
    Vector n;
    Color color;

    operator bool() { return t >= 0 && t < INFINITY; } // renvoie vrai si l'intersection existe et quelle est valide...
};

Hit intersect(const Point& o, const Vector& d, const Triangle triangle) {
    
    Vector n = cross(Vector(triangle.a, triangle.b), Vector(triangle.a, triangle.c));
    float t = dot(n, Vector(o, triangle.a)) / dot(n, d);
    Point p = o + t * d;

    if ((dot(n, cross(Vector(triangle.a, triangle.b), Vector(triangle.a, p))) < 0)
        || (dot(n, cross(Vector(triangle.b, triangle.c), Vector(triangle.b, p))) < 0)
        || (dot(n, cross(Vector(triangle.c, triangle.a), Vector(triangle.c, p))) < 0)
        ) {
        return { INFINITY, Vector(), triangle.color };
    }

    return { t, normalize(n), triangle.color };
    
}

bool isRayInTriangle(std::array<Vector, 3>& currentTriangleNormals, Vector& rayDirection) {

    std::array<float, 3> dotProducts;

    for (int i = 0; i < 3; i++) {
        dotProducts[i] = dot(currentTriangleNormals[i], rayDirection);
    }
    return (dotProducts[0] >= 0) && (dotProducts[1] >= 0) && (dotProducts[2] >= 0);
}
                                                   

void movePositions(std::vector<Point>& positions, Vector& direction) {
    for (unsigned i = 0; i < positions.size(); i++) {
		positions[i] = positions[i] + direction;
	}
}

void rotatePositionsY(std::vector<Point>& positions, const float angle) {
    for (unsigned i = 0; i < positions.size(); i++) {
        Transform rot = RotationY(angle);
        positions[i] = rot(positions[i]);
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


void render(Image& image, const MeshIOData& data) {
    
    Point origin(0, 0, 0);
    Light light = { Point(400.0, 315.0, -400.0), 4.0f, White()};
    Triangle triangle;
    Color materialColor;
    Point a, b, c;

    for (int py = 0; py < image.height(); py++) {
        for (int px = 0; px < image.width(); px++) {
            
            float x = (static_cast<float>(px) / image.width()) * 2 - 1;
            float y = (static_cast<float>(py) / image.height()) * 2 - 1;
            float z = -1;
            
            Point rayOrigin = Point(x, y, z);
            Vector rayDirection = Vector(origin, rayOrigin);

            float t = INFINITY;

            for (unsigned i = 0; i + 2 < data.indices.size(); i += 3) {
    
                a = data.positions[data.indices[i + 0]];
                b = data.positions[data.indices[i + 1]];
                c = data.positions[data.indices[i + 2]];
                materialColor = data.materials.materials[data.material_indices[i / 3]].diffuse;

                triangle = { a, b, c, materialColor };

                Hit hit = intersect(rayOrigin, rayDirection, triangle);

			    if (hit && hit.t < t) {

                    t = hit.t;

                    Point intersectingPoint = rayOrigin + hit.t * rayDirection;
                    intersectingPoint = intersectingPoint + hit.n * 0.001; // on d�cale d'un epsilon

                    Vector lightDirection = Vector(intersectingPoint, light.p);

                    float cos_theta = std::max(float(0),dot(normalize(hit.n), normalize(lightDirection)));
                    
                    Color pixel = Black();
                    Triangle triangle2;
                    Color materialColor2;
                    Point a2, b2, c2;
                    
					float intensity = 0;
                    float delta = 1.0 / 25.0;

                    for (int k = 0; k < 25; k++) {
                        
                        float randLightX = rand() * 2 + 1;
                        float randLightY = rand() * 2 + 1;
						float randLightZ = rand() * 2 + 1;

						//std::cout << "randLightX : " << randLightX << " randLightY : " << randLightY << std::endl;
                        
                        Point randLightPoint = Point(light.p.x + randLightX, light.p.y + randLightY, light.p.z + randLightZ);
                        
                        lightDirection = Vector(intersectingPoint, randLightPoint);
                        
                        for (unsigned j = 0; j + 2 < data.indices.size(); j += 3) {

                            a2 = data.positions[data.indices[j + 0]];
                            b2 = data.positions[data.indices[j + 1]];
                            c2 = data.positions[data.indices[j + 2]];
                            materialColor2 = data.materials.materials[data.material_indices[j / 3]].diffuse;

                            triangle2 = { a2, b2, c2, materialColor2 };

                            Hit shadowRay = intersect(intersectingPoint, lightDirection, triangle2);

                            if (!shadowRay && shadowRay.t > 1) {  // V�rifier ombre
                                pixel = (pixel + (White() * hit.color * light.color * cos_theta));
                            }
                            
							if (shadowRay) {
								intensity += delta;
							}
                        }
                    }
                    
                    if (intensity != 0) {
                        pixel = pixel * (1 - intensity);
                    }
                    
                    image(px, py) = Color(pixel, 1 - intensity);

                }
                
            }

        }
    }
}

Image load_mesh(const char* filename) {
   
    Image image(1024, 1024);
    Point pmin, pmax;
	MeshIOData data = read_meshio_data(filename);
    
    movePositions(data.positions, Vector(0, -200, -600));
    //movePositions(data.positions, Vector(0, -2, -8));
    //rotatePositionsY(data.positions, 10);
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