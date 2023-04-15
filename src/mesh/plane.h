#ifndef  __MESH_PLANE_H_
#define  __MESH_PLANE_H_

#include "../core/basetypes.h"
#include "../core/gltf/primitive.h"
#include "../core/math/point3.h"
#include "../core/math/vector3.h"
#include "../utils/io_util.h"

namespace mesh {

class MeshPlane : public Primitive {

public:

    int generate(const Point3 &center, double width, double height, Vector3 up, Vector3 front) {
        up.normalize();
        front.normalize();
        Vector3 side = up.cross(front);
        // up.debug("up");
        // front.debug("front");
        // side.debug("side");
        Point3 p0 = center + front * width * 0.5 + side * height * 0.5;
        Point3 p1 = center - front * width * 0.5 + side * height * 0.5;
        Point3 p2 = center - front * width * 0.5 - side * height * 0.5;
        Point3 p3 = center + front * width * 0.5 - side * height * 0.5;

        // p0.debug("p0");
        // p1.debug("p1");
        // p2.debug("p2");
        // p3.debug("p3");

        add_point(p0);
        add_point(p1);
        add_point(p2);
        add_point(p3);

        add_quad(0, 1, 2, 3);
        return 0;
    }

protected:

    

};
}

#endif