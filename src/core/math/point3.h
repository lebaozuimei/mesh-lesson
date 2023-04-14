#ifndef  __MATH_POINT3_H_
#define  __MATH_POINT3_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

namespace mesh {

using json = nlohmann::json;

class Point3 {

public:

    double x;
    double y;
    double z;

public:

    Point3(double px = 0.0f, double py = 0.0f, double pz = 0.0f) {
        x = px;
        y = py;
        z = pz;
    }

    Point3 operator-(const Point3 &p) {
        return Point3(x - p.x, y - p.y, z - p.z);
    }

    Point3& operator-=(const Point3 &p) {
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }

    Point3& operator+=(const Point3 &p) {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    double operator[](size_t i) const {
        if (i == 0) {
            return x;
        } else if (i == 1) {
            return y;
        } else if (i == 2) {
            return z;
        }
        assert(false);
        return 0.0f;
    }

    void set_to(double px = 0.0f, double py = 0.0f, double pz = 0.0f) {
        x = px;
        y = py;
        z = pz;
    }

    // Point3 operator+(const Vector3 &vec3) const {
    //     return Point3(x + vec3[0], y + vec3[1], z + vec3[2]);
    // }

    // Vector3 operator-(const Point3 &p) const {
    //     return Vector3(x - p.x, y - p.y, z - p.z);
    // }

    double distance(double px, double py, double pz) const {
        return std::sqrt((x - px) * (x - px) + (y - py) * (y - py) + (z - pz) * (z - pz));
    }

    double distance(const Point3 &p) const {
        return std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
    }

    void debug(const std::string &prefix) const {
        std::cout << std::setprecision(12) << prefix << " (" << x << "," << y << "," << z << ")" << std::endl;
    }

    Point3 get_center(const Point3 &p) const {
        // return Point3((x + p.x) * 0.5, (y + p.y) * 0.5, (z + p.y) * 0.5);
        return Point3((x + p.x) * 0.5, (y + p.y) * 0.5, (z + p.z) * 0.5);
    }

    bool is_same_with(const mesh::Point3 &p) {
        return abs(x - p.x) < 1e-6 && abs(y - p.y) < 1e-6 && abs(z - p.z) < 1e-6;
    }

};

// Vector3 operator-(const Point3 &a, const Point3 &b) {
//     return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
// }

}

#endif