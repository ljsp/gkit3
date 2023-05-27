// exemple de code test a compiler pour verifier que tout est ok
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include <vec.h>
#include <array>

float orientation(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool isInTriangle(const Point& p, const Point& a, const Point& b, const Point& c, float& lambdaA, float& lambdaB, float& lambdaC) {
    float det = orientation(a, b, c);
    lambdaA = orientation(p, b, c) / det;
    lambdaB = orientation(a, p, c) / det;
    lambdaC = orientation(a, b, p) / det;

    return (lambdaA >= 0 && lambdaB >= 0 && lambdaC >= 0);
}

bool isFacingCamera(const Point& a, const Point& b, const Point& c, const Point& camera) {
    Vector n = normalize(cross(Vector(a, b), Vector(a, c)));
    Vector view = normalize(camera - a);

    return dot(n, view) < 0;
}

// Ex 3
int main() {
    Image image(1024, 1024);
    std::vector<float> zbuffer(image.width() * image.height(), std::numeric_limits<float>::max());

    Point triangle1[] = { Point(-0.5, -0.5, -1), Point(0.5, -0.5, -1), Point(0, 0.8, -1) };
    Point triangle2[] = { Point(0.8, 0.3, 1), Point(0, 0, -1), Point(0, 0.8, 1) };

    Point* triangles[] = { triangle1, triangle2 };
    int numTriangles = 2;

    Color colors[] = { Red(), Green() };

    Point o(0, 0, 0);

    for (int t = 0; t < numTriangles; t++) {
        Point a = triangles[t][0];
        Point b = triangles[t][1];
        Point c = triangles[t][2];
        Color triangleColor = colors[t];

        if (!isFacingCamera(a, b, c, o)) {
            std::swap(a, b);
        }

        for (int py = 0; py < image.height(); py++) {
            for (int px = 0; px < image.width(); px++) {
                float x = static_cast<float>(px) / image.width() * 2 - 1;
                float y = static_cast<float>(py) / image.height() * 2 - 1;
                float z = -1;
                Point p(x, y, z);

                Vector d = p - o;

                float lambdaA, lambdaB, lambdaC;
                if (isInTriangle(p, a, b, c, lambdaA, lambdaB, lambdaC)) {
                    float zValue = lambdaA * a.z + lambdaB * b.z + lambdaC * c.z;
                    int zIndex = py * image.width() + px;
                    if (zValue < zbuffer[zIndex]) {
                        zbuffer[zIndex] = zValue;
                        image(px, py) = triangleColor;
                    }
                }
            }
        }
    }

    write_image(image, "ex3_image.png");
    return 0;
}
