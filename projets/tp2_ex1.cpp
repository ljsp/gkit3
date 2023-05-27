// exemple de code test a compiler pour verifier que tout est ok
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "mesh_io.h"
#include "vec.h"
#include <vec.h>
#include <array>

float areaTriangle(const Point& a, const Point& b, const Point& c) {
    float base = distance(a, b);
    Point centre = center(a, b);
    float hauteur = distance(centre, c);
    return base * hauteur / 2;
    //return 0.5 * length(cross(Vector(a, b), Vector(a, c)));
}

// Ex 1
int main()
{
    Image image(1024, 1024);
    Point a(300, 750, 0);
    Point b(700, 750, 0);
    Point c(500, 250, 0);

    Vector ab(a, b);
    Vector bc(b, c);
    Vector ca(c, a);

    float aireABC = areaTriangle(a, b, c);

    int xMin = std::min(a.x, std::min(b.x, c.x));
    int xMax = std::max(a.x, std::max(b.x, c.x));
    int yMin = std::min(a.y, std::min(b.y, c.y));
    int yMax = std::max(a.y, std::max(b.y, c.y));

    for (int py = 0; py < image.height(); py++) {
        for (int px = 0; px < image.width(); px++) {
            if (px >= xMin && px <= xMax && py >= yMin && py <= yMax) {

                Point p(px, py, 0);
                Vector ap(a, p);
                Vector bp(b, p);
                Vector cp(c, p);

                if (cross(ab, ap).z < 0 && cross(bc, bp).z < 0 && cross(ca, cp).z < 0) {

                    float airePBC = areaTriangle(p, b, c);
                    float airePCA = areaTriangle(p, c, a);
                    float airePAB = areaTriangle(p, a, b);

                    float alpha = airePBC / aireABC;
                    float beta = airePCA / aireABC;
                    float gamma = airePAB / aireABC;

                    Color color = alpha * Red() + beta * Green() + gamma * Blue();
                    image(px, py) = color;
                }
            }
        }
    }

    write_image(image, "ex1_image.png"); // par defaut en .png
    return 0;
}