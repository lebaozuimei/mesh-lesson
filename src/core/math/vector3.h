#ifndef  __MATH_VECTOR3_H_
#define  __MATH_VECTOR3_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "../basetypes.h"
#include "../../utils/io_util.h"

#include "point3.h"

namespace mesh {

using json = nlohmann::json;

class Vector3 {

public:

    double data[3];

public:

    Vector3(double x = 0.0f, double y = 0.0f, double z = 0.0f) {
        data[0] = x;
        data[1] = y;
        data[2] = z;
    }

    Vector3(const Point3 &from, const Point3 &to) {
        data[0] = to.x - from.x;
        data[1] = to.y - from.y;
        data[2] = to.z - from.z;
    }

    double operator[](size_t i) const {
        return data[i];
    }

    Point3 operator+(const Point3 &p) const {
        return Point3(p.x + data[0], p.y + data[1], p.z + data[2]);
    }

    Vector3 operator+(const Vector3 &vec3) const {
        return Vector3(vec3[0] + data[0], vec3[1] + data[1], vec3[2] + data[2]);
    }

    Vector3 operator-(const Vector3 &vec3) const {
        return Vector3(vec3[0] - data[0], vec3[1] - data[1], vec3[2] - data[2]);
    }

    Vector3 operator*(const double v) const {
        return Vector3(data[0] * v, data[1] * v, data[2] * v);
    }

    double dot(const Vector3 &v) const {
        return data[0] * v.data[0] + data[1] * v.data[1] + data[2] * v.data[2];
    }

    void add(const Vector3 &vec) {
        data[0] += vec[0];
        data[1] += vec[1];
        data[2] += vec[2];
    }

    void normalize() {
        double t = std::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
        data[0] /= t;
        data[1] /= t;
        data[2] /= t;
    }

    Vector3 cross(const Vector3 &vec) const {
        double x = data[1] * vec[2] - data[2] * vec[1];
        double y = data[2] * vec[0] - data[0] * vec[2];
        double z = data[0] * vec[1] - data[1] * vec[0];
        double t = std::sqrt(x * x + y * y + z * z);
        return Vector3(x / t, y / t, z / t);
    }

    void debug(const std::string &prefix) const {
        std::cout << std::setprecision(12) << prefix << " (" << data[0] << "," << data[1] << "," << data[2] << ")" << std::endl;
    }
};

Point3 operator+(const Point3 &p, const Vector3 &vec3) {
    return Point3(p.x + vec3[0], p.y + vec3[1], p.z + vec3[2]);
}

Point3 operator-(const Point3 &p, const Vector3 &vec3) {
    return Point3(p.x - vec3[0], p.y - vec3[1], p.z - vec3[2]);
}

}

#endif