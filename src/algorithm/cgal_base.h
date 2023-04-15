#ifndef  __ALGORITHM_CGAL_BASE_H_
#define  __ALGORITHM_CGAL_BASE_H_

#include <iostream>
#include <fstream>
#include <utility>
#include <chrono>
#include <unordered_set>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

#include "../utils/io_util.h"
#include "../core/basetypes.h"
#include "../core/gltf/primitive.h"

namespace mesh {

class CGALBase {

public:
    typedef CGAL::Simple_cartesian<double> Kernel;
    // typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    // typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;

    typedef Kernel::Point_3 CGALPoint3;
    typedef Kernel::Vector_3 CGALVector3;
    typedef Kernel::Segment_3 CGALSegment3;
    typedef CGAL::Surface_mesh<CGALPoint3> SurfaceMesh;
    typedef boost::graph_traits<SurfaceMesh>::halfedge_descriptor HalfedgeDescriptor;
    typedef boost::graph_traits<SurfaceMesh>::edge_descriptor EdgeDescriptor;
    typedef boost::graph_traits<SurfaceMesh>::vertex_descriptor VertexDescriptor;
    typedef boost::graph_traits<SurfaceMesh>::face_descriptor FaceDescriptor;
    typedef typename SurfaceMesh::Vertex_index VertexIndex;
    typedef typename SurfaceMesh::Face_index FaceIndex;

    struct UV {
        TexcoordType u;
        TexcoordType v;
        UV() : u(-1), v(-1) {}
        UV(TexcoordType _u, TexcoordType _v) : u(_u), v(_v) {}
    };
    typedef SurfaceMesh::Property_map<VertexDescriptor, UV> UVMap;

    typedef SurfaceMesh::Property_map<VertexDescriptor, CGALVector3> NormalMap;

protected:

    int _check_vertices_usage(const Primitive &primitive) const {
        // size_t position_count = primitive.positions.size();
        size_t point_count = primitive.positions.size() / 3;

        std::vector<bool> used(point_count, false);
        
        const IndexType *index_ptr = &primitive.indices[0];
        size_t face_count = primitive.indices.size() / 3;
        for (size_t k = 0; k < face_count; ++k) {
            used[index_ptr[k * 3 + 0]] = true; 
            used[index_ptr[k * 3 + 1]] = true; 
            used[index_ptr[k * 3 + 2]] = true;
        }
        for (size_t k = 0; k < point_count; ++k) {
            if (!used[k]) {
                std::cout << "find non-used vertex at: " << k << std::endl;
                return -1;
            }
        }
        // std::cout << "check vertices succeed" << std::endl;
        return 0;
    }

    int _gltf_primitive_to_surface_mesh_shrink(const Primitive &primitive, SurfaceMesh &surface_mesh) {
        // size_t position_count = primitive.positions.size();
        size_t point_count = primitive.positions.size() / 3;
        size_t face_count = primitive.indices.size() / 3;

        std::vector<bool> used(point_count, false);
        const IndexType *index_ptr = &primitive.indices[0];
        for (size_t k = 0; k < face_count; ++k) {
            used[index_ptr[k * 3 + 0]] = true; 
            used[index_ptr[k * 3 + 1]] = true; 
            used[index_ptr[k * 3 + 2]] = true;
        }

        std::vector<int> remap(point_count);
        for (size_t k = 0; k < point_count; ++k) {
            remap[k] = k;
        }
        for (size_t k = 0; k < point_count; ++k) {
            if (!used[k]) {
                for (size_t i = k + 1; i < point_count; ++i) {
                    --remap[i];
                }
            }
        }

        // for (size_t k = 0; k < point_count; ++k) {
        //     if (used[k]) {
        //         DEBUG2(k, remap[k]);
        //     }
        // }

        const PositionType *position_ptr = &primitive.positions[0];
        for (size_t k = 0; k < point_count; ++k) {
            if (used[k]) {
                surface_mesh.add_vertex(CGALPoint3(position_ptr[k * 3], position_ptr[k * 3 + 1], position_ptr[k * 3 + 2]));
            }
        }

        const auto &vertices = surface_mesh.vertices();
        // const IndexType *index_ptr = &primitive.indices[0];
        // size_t face_count = primitive.indices.size() / 3;
        for (size_t k = 0; k < face_count; ++k) {
            surface_mesh.add_face(
                *(vertices.begin() + remap[index_ptr[k * 3 + 0]]), 
                *(vertices.begin() + remap[index_ptr[k * 3 + 1]]), 
                *(vertices.begin() + remap[index_ptr[k * 3 + 2]])
            );
        }
        // std::cout << "break 1" << std::endl;

        if (primitive.texcoords0.size() > 0) {
            assert(primitive.texcoords0.size() / 2 == primitive.positions.size() / 3);
            UVMap uv_map = surface_mesh.add_property_map<VertexDescriptor, UV>("v:uv", UV(-1.0, -1.0)).first;
            _build_uv_map(primitive, surface_mesh, uv_map, used, remap);
        }

        if (primitive.normals.size() > 0) {
            assert(primitive.normals.size() / 3 == primitive.positions.size() / 3);
            NormalMap normal_map = surface_mesh.add_property_map<VertexDescriptor, CGALVector3>("v:normal", CGAL::NULL_VECTOR).first;
            _build_normal_map(primitive, surface_mesh, normal_map, used, remap);
        }

        return 0;
    }

    int _gltf_primitive_to_surface_mesh(const Primitive &primitive, SurfaceMesh &surface_mesh) {
        size_t position_count = primitive.positions.size();
        // size_t point_count = primitive.positions.size() / 3;

        const PositionType *position_ptr = &primitive.positions[0];
        // const TexcoordType *texcoord_ptr = &primitive.texcoords0[0];
        for (size_t k = 0; k < position_count; k += 3) {
            surface_mesh.add_vertex(CGALPoint3(position_ptr[k], position_ptr[k + 1], position_ptr[k + 2]));
            // if (k < 10) {
            //     DEBUG(position_ptr[k]);
            //     DEBUG(position_ptr[k + 1]);
            //     DEBUG(position_ptr[k + 2]);
            // }
        }
        const auto &vertices = surface_mesh.vertices();
        const IndexType *index_ptr = &primitive.indices[0];
        size_t face_count = primitive.indices.size() / 3;
        for (size_t k = 0; k < face_count; ++k) {
            surface_mesh.add_face(
                *(vertices.begin() + index_ptr[k * 3 + 0]), 
                *(vertices.begin() + index_ptr[k * 3 + 1]), 
                *(vertices.begin() + index_ptr[k * 3 + 2])
            );
        }
        
        // DEBUG(primitive.texcoords0.size());
        if (primitive.texcoords0.size() > 0) {
            assert(primitive.texcoords0.size() / 2 == primitive.positions.size() / 3);
            UVMap uv_map = surface_mesh.add_property_map<VertexDescriptor, UV>("v:uv", UV(-1.0, -1.0)).first;
            _build_uv_map(primitive, surface_mesh, uv_map);
        }

        if (primitive.normals.size() > 0) {
            assert(primitive.normals.size() / 3 == primitive.positions.size() / 3);
            NormalMap normal_map = surface_mesh.add_property_map<VertexDescriptor, CGALVector3>("v:normal", CGAL::NULL_VECTOR).first;
            _build_normal_map(primitive, surface_mesh, normal_map);
        }

        return 0;
    }

    void _surface_mesh_to_gltf_primitive(SurfaceMesh &surface_mesh, Primitive &primitive) {
        primitive.clear();

        std::vector<PositionType> &positions = primitive.positions;
        std::vector<NormalType> &normals = primitive.normals;
        std::vector<TexcoordType> &texcoords = primitive.texcoords0;
        std::vector<IndexType> &indices = primitive.indices;

        positions.reserve(surface_mesh.num_vertices() * 3);
        texcoords.reserve(surface_mesh.num_vertices() * 2);

        uint32_t max_index = 0;

        const SurfaceMesh::Property_map<VertexDescriptor, CGALPoint3> &location = surface_mesh.points();
        std::vector<int> reindex(surface_mesh.num_vertices());
        uint32_t n = 0;
        for (VertexIndex vd : surface_mesh.vertices()) {
            const auto &p = location[vd];
            positions.push_back(p.x());
            positions.push_back(p.y());
            positions.push_back(p.z());
            reindex[vd] = n++;
            max_index = std::max(max_index, n - 1);
        }

        UVMap uv_map;
        bool exist;
        boost::tie(uv_map, exist) = surface_mesh.property_map<VertexDescriptor, UV>("v:uv");
        if (exist) {
            for (VertexIndex vd : surface_mesh.vertices()) {
                const UV &uv = uv_map[vd];
                // DEBUG2(uv.u, uv.v);
                texcoords.push_back(uv.u < 0 ? 0 : uv.u);
                texcoords.push_back(uv.v < 0 ? 0 : uv.v);
            }
        }

        NormalMap normal_map;
        boost::tie(normal_map, exist) = surface_mesh.property_map<VertexDescriptor, CGALVector3>("v:normal");
        if (exist) {
            for (VertexIndex vd : surface_mesh.vertices()) {
                const CGALVector3 &vec3 = normal_map[vd];
                normals.push_back(vec3.x());
                normals.push_back(vec3.y());
                normals.push_back(vec3.z());
                // DEBUG3(vec3.x(), vec3.y(), vec3.z());
            }
        }

        indices.reserve(surface_mesh.num_faces() * 3);
        for (FaceIndex f : surface_mesh.faces()) {
            for (VertexIndex v : CGAL::vertices_around_face(surface_mesh.halfedge(f), surface_mesh)) {
                indices.push_back(reindex[v]);
            }
        }
        // DEBUG(join_list(positions));
        // DEBUG(join_list(indices16));
        // for (Face_index f : surface_mesh.faces()) {
        //     for(Vertex_index v : CGAL::vertices_around_face(surface_mesh.halfedge(f), surface_mesh)) {
        //         indices.push_back(reindex[v]);
        //     }
        // }
    }

    int _build_uv_map(const Primitive &primitive, SurfaceMesh &surface_mesh, UVMap &uv_map) {
        size_t point_count = primitive.positions.size() / 3;
        size_t uv_count = primitive.texcoords0.size() / 2;

        if (point_count == uv_count) {
            const auto &vertices = surface_mesh.vertices();
            const TexcoordType *texcoord_ptr = &primitive.texcoords0[0];
            for (size_t k = 0; k < uv_count; ++k) {
                uv_map[*(vertices.begin() + k)] = UV(texcoord_ptr[k * 2], texcoord_ptr[k * 2 + 1]);
            }
        }
        return 0;
    }

    int _build_normal_map(const Primitive &primitive, SurfaceMesh &surface_mesh, NormalMap &normal_map) {
        size_t point_count = primitive.positions.size() / 3;
        size_t normal_count = primitive.normals.size() / 3;

        if (point_count == normal_count) {
            const auto &vertices = surface_mesh.vertices();
            const NormalType *normal_ptr = &primitive.normals[0];
            for (size_t k = 0; k < normal_count; ++k) {
                normal_map[*(vertices.begin() + k)] = CGALVector3(
                    normal_ptr[k * 3], normal_ptr[k * 3 + 1], normal_ptr[k * 3 + 2]
                );
            }
        }
        return 0;
    }

    int _build_uv_map(const Primitive &primitive, SurfaceMesh &surface_mesh, UVMap &uv_map, 
        const std::vector<bool> &used, const std::vector<int> &remap) {
        size_t point_count = primitive.positions.size() / 3;
        size_t uv_count = primitive.texcoords0.size() / 2;

        if (point_count == uv_count) {
            const auto &vertices = surface_mesh.vertices();
            const TexcoordType *texcoord_ptr = &primitive.texcoords0[0];
            for (size_t k = 0; k < uv_count; ++k) {
                if (used[k]) {
                    uv_map[*(vertices.begin() + remap[k])] = UV(texcoord_ptr[k * 2], texcoord_ptr[k * 2 + 1]);
                }
            }
        }
        return 0;
    }

    int _build_normal_map(const Primitive &primitive, SurfaceMesh &surface_mesh, NormalMap &normal_map,
        const std::vector<bool> &used, const std::vector<int> &remap) {
        size_t point_count = primitive.positions.size() / 3;
        size_t normal_count = primitive.normals.size() / 3;

        if (point_count == normal_count) {
            const auto &vertices = surface_mesh.vertices();
            const NormalType *normal_ptr = &primitive.normals[0];
            for (size_t k = 0; k < normal_count; ++k) {
                if (used[k]) {
                    normal_map[*(vertices.begin() + remap[k])] = CGALVector3(
                        normal_ptr[k * 3], normal_ptr[k * 3 + 1], normal_ptr[k * 3 + 2]
                    );
                }
            }
        }
        return 0;
    }
};
}

#endif