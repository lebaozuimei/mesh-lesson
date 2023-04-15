#ifndef  __MESH_CUBE_H_
#define  __MESH_CUBE_H_

#include "../core/basetypes.h"
#include "../core/gltf/primitive.h"
#include "../core/math/point3.h"
#include "../utils/io_util.h"

namespace mesh {

class MeshCube : public Primitive {

public:

    int generate(const Point3 &center, 
        double extend_x, double extend_y, double extend_z) {
        
        double hw = extend_x * 0.5;
        double hh = extend_y * 0.5;
        double hd = extend_z * 0.5;

        add_point(center.x + hw, center.y + hh, center.z - hd);
        add_point(center.x - hw, center.y + hh, center.z - hd);
        add_point(center.x - hw, center.y + hh, center.z + hd);
        add_point(center.x + hw, center.y + hh, center.z + hd);
        add_point(center.x + hw, center.y - hh, center.z - hd);
        add_point(center.x - hw, center.y - hh, center.z - hd);
        add_point(center.x - hw, center.y - hh, center.z + hd);
        add_point(center.x + hw, center.y - hh, center.z + hd);
        
        add_quad(3, 2, 6, 7);
        add_quad(1, 0, 4, 5);
        add_quad(2, 1, 5, 6);
        add_quad(0, 3, 7, 4);
        add_quad(0, 1, 2, 3);
        add_quad(5, 4, 7, 6);
        return 0;
    }


};
}

#endif