// exemple de code test a compiler pour verifier que tout est ok
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include <vec.h>
#include <array>

// Ex 2
int main()
{
    Image image(1024, 1024);
    Point a(-0.5, -0.5, -1);
    Point b(0.5, -0.5, -1);
    Point c(0, 0.8, -1);
    Point o = Point(0, 0, 0);

    std::array<Point, 3> points{a, b, c};
    std::array<Vector, 3> normals;
    std::array<float, 3> V;
    std::array<float, 3> lambdas;

    for (int i = 0; i < 3; i++) {
        normals[i] = normalize(cross(points[(i + 2) % 3] - o, points[(i + 1) % 3] - points[(i + 2) % 3]));
    }

    for (int py = 0; py < image.height(); py++) {
        for (int px = 0; px < image.width(); px++) {
            // rayon
            float x = (static_cast<float>(px) / image.width()) * 2 - 1;
            float y = (static_cast<float>(py) / image.height()) * 2 - 1;
            float z = -1;
            Point p = Point(x, y, z);
            Vector d = Vector(o, p);

            for (int i = 0; i < 3; i++) {
                V[i] = dot(normals[i], d);
            }

            if ((V[0] >= 0) && (V[1] >= 0) && (V[2] >= 0)) {
                for (int i = 0; i < 3; i++) {
                    lambdas[i] = V[i] / (V[0] + V[1] + V[2]);
                }
                image(px, py) = lambdas[0] * Red() + lambdas[1] * Green() + lambdas[2] * Blue();
            }
        }
    }

    write_image(image, "ex2_image.png");
    return 0;
}