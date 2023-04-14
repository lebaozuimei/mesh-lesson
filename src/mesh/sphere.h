#ifndef  __MESH_SPHERE_H_
#define  __MESH_SPHERE_H_

#include <vector>
#include <cmath>

#include "../core/basetypes.h"
#include "../core/gltf/primitive.h"
#include "../core/math/point3.h"
#include "../utils/io_util.h"

namespace mesh {

class MeshSphere : public Primitive {

public:

    int generate(const Point3 &center, double r = 100.0f, int segment_ring = 30, int segment_hy = 20) {
        std::vector<std::vector<Point3>> ring_list;

        // 1. add points ring by ring from north(top) to south(bottom)
        add_point(Point3(center.x, center.y + r, center.z)); // north pole
        for (int k = 1; k <= segment_hy * 2 - 1; ++k) {
            double theta = M_PI / (segment_hy * 2) * k;
            double y = center.y + r * cos(theta);
            double ring_r = r * sin(theta);
            std::vector<Point3> ring_points;
            _generate_ring_points(Point3(center.x, y, center.z), ring_r, segment_ring, ring_points);
            ring_list.push_back(ring_points);
            for (int i = 0; i < ring_points.size(); ++i) {
                add_point(ring_points[i]);
            }
        }
        add_point(Point3(center.x, center.y - r, center.z)); // south pole

        // 2. add face(triangle) around north pole
        int north_pole_index = 0;
        for (int i = 1; i <= segment_ring; ++i) {
            int a = segment_ring + i;
            int b = segment_ring + i + 1;
            if (i == segment_ring) {
                b -= segment_ring;
            }
            add_triangle(north_pole_index, b, a);
        }

        // 3. add face(quad) between every two rings
        int ring_count = 2 * segment_hy - 1;
        for (int k = 0; k < ring_count - 1; ++k) {
            for (int i = 1; i <= segment_ring; ++i) {
                int a = segment_ring * k + i + 1;
                int b = segment_ring * k + i;
                int c = segment_ring * (k + 1) + i;
                int d = segment_ring * (k + 1) + i + 1;
                if (i == segment_ring) {
                    a -= segment_ring;
                    d -= segment_ring;
                }
                add_quad(a, d, c, b);
            }
        }

        // 4. add face(triangle) around south pole
        int south_pole_index = last_point_index();
        for (int i = 1; i <= segment_ring; ++i) {
            int a = (ring_count - 1) * segment_ring + i;
            int b = (ring_count - 1) * segment_ring + i + 1;
            if (i == segment_ring) {
                b -= segment_ring;
            }
            add_triangle(a, b, south_pole_index);
        }

        return 0;
    }

protected:

    void _generate_ring_points(const Point3 &center, double r, int segment, std::vector<Point3> &result) {
        for (int i = 0; i < segment; ++i) {
            double theta = 2 * M_PI / segment * i;
            double x = center.x + r * cos(theta);
            double z = center.z + r * sin(theta);
            result.push_back(Point3(x, center.y, z));
        }
    }
};
}

#endif