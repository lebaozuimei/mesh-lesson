#ifndef  __ALGORITHM_CGAL_SIMPLIFICATION_H_
#define  __ALGORITHM_CGAL_SIMPLIFICATION_H_

#include "cgal_base.h"

#include <iomanip>

#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Edge_collapse_visitor_base.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Constrained_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>

#include <CGAL/Polygon_mesh_processing/locate.h>

namespace mesh {

namespace sms = CGAL::Surface_mesh_simplification;

class CGALSimplification : public CGALBase {

public:

    typedef sms::Edge_profile<SurfaceMesh> Profile;

    struct BorderIsConstrainedEdgeMap {
        const SurfaceMesh* sm_ptr;
        typedef EdgeDescriptor KeyType;
        BorderIsConstrainedEdgeMap(const SurfaceMesh& sm) : sm_ptr(&sm) {}
        friend bool get(const BorderIsConstrainedEdgeMap& m, const KeyType& edge) {
            return CGAL::is_border(edge, *m.sm_ptr);
        }
    };
    // Placement class
    typedef sms::Constrained_placement<sms::Midpoint_placement<SurfaceMesh>, BorderIsConstrainedEdgeMap> Placement;

    struct mesh_visitor : sms::Edge_collapse_visitor_base<SurfaceMesh> {

        const SurfaceMesh &surface_mesh;
        const UVMap &uv_map;

        double last_u = 0.0f;
        double last_v = 0.0f;

        struct vec3_t {
            double x, y, z;
            vec3_t(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
        };

        mesh_visitor(const SurfaceMesh &mesh, const UVMap &map) : surface_mesh(mesh), uv_map(map) {}

        void OnCollapsing(const Profile &profile, boost::optional<CGALPoint3> placement) {
            if (!placement) {
                return;
            }

            auto range = CGAL::faces_around_target(profile.v0_v1(), surface_mesh);
            std::vector<CGALPoint3> plist;
            std::vector<UV> uvlist;
            for (auto iter = range.begin(); iter != range.end(); ++iter) {
                for (VertexDescriptor vd : CGAL::vertices_around_face(surface_mesh.halfedge(*iter), surface_mesh)) {
                    plist.push_back(surface_mesh.points()[vd]);
                    uvlist.push_back(uv_map[vd]);
                }
                break;
            }

            auto bc = CGAL::Polygon_mesh_processing::barycentric_coordinates(plist[0], plist[1], plist[2], *placement);
            // std::cout << "barycentric_coordinates: " << bc[0] << "," << bc[1] << "," << bc[2] << std::endl;
            last_u = uvlist[0].u * bc[0] + uvlist[1].u * bc[1] + uvlist[2].u * bc[2];
            last_v = uvlist[0].v * bc[0] + uvlist[1].v * bc[1] + uvlist[2].v * bc[2];
        }

        // Called after each edge has been collapsed
        void OnCollapsed(const Profile& profile, VertexDescriptor vd) {
            uv_map[vd] = UV(last_u, last_v);
        }
        
    };

public:

    int run(Gltf &gltf, float ratio) {
        for (size_t i = 0; i < gltf.meshes.size(); ++i) {
            for (size_t k = 0; k < gltf.meshes[i].primitives.size(); ++k) {
                int ret = run(gltf.meshes[i].primitives[k], ratio);
                if (ret != 0) {
                    return ret;
                }
            }
        }
        return 0;
    }

    int run(Primitive &primitive, float ratio) {
        SurfaceMesh surface_mesh;
        SurfaceMesh::Property_map<VertexDescriptor, UV> uv_map;
        try {
            _gltf_primitive_to_surface_mesh(primitive, surface_mesh);

            SurfaceMesh::Property_map<HalfedgeDescriptor, std::pair<CGALPoint3, CGALPoint3>> constrained_halfedges = 
                surface_mesh.add_property_map<HalfedgeDescriptor, std::pair<CGALPoint3, CGALPoint3>>("h:vertices").first;
            size_t nb_border_edges = 0;
            for (HalfedgeDescriptor hd : halfedges(surface_mesh)) {
                if (CGAL::is_border(hd, surface_mesh)) {
                    constrained_halfedges[hd] = std::make_pair(
                        surface_mesh.point(source(hd, surface_mesh)),
                        surface_mesh.point(target(hd, surface_mesh))
                    );
                    ++nb_border_edges;
                }
            }

            uv_map = surface_mesh.add_property_map<VertexDescriptor, UV>("v:uv_map", UV(-1.0, -1.0)).first;
            
            if (!CGAL::is_triangle_mesh(surface_mesh)) {
                std::cerr << "input mesh is not triangulated." << std::endl;
                return -2;
            }
            
            _build_uv_map(primitive, surface_mesh, uv_map);
            mesh_visitor vis(surface_mesh, uv_map);
            sms::Count_ratio_stop_predicate<SurfaceMesh> stop(ratio);
            BorderIsConstrainedEdgeMap bem(surface_mesh);
            int edge_removed = sms::edge_collapse(surface_mesh, stop, 
                CGAL::parameters::edge_is_constrained_map(bem).get_placement(Placement(bem)).visitor(vis)
            );
            UNUSED(edge_removed);
        } catch (...) {
            std::cerr << "collapse exception" << std::endl;
            return -1;
        }

        std::vector<PositionType> positions;
        std::vector<NormalType> normals;
        std::vector<TexcoordType> texcoords;
        std::vector<IndexType> indices;
        _collect_output(surface_mesh, uv_map, positions, normals, texcoords, indices);

        primitive.set_positions(positions);
        primitive.set_normals(normals);
        primitive.set_texcoords0(texcoords);
        primitive.set_indices(indices);
        return 0;
    }

protected:

    void _collect_output(
        SurfaceMesh &surface_mesh,
        const UVMap &uv_map,
        std::vector<PositionType> &positions,
        std::vector<NormalType> &normals,
        std::vector<TexcoordType> &texcoords,
        std::vector<IndexType> &indices) {

        positions.reserve(surface_mesh.num_vertices() * 3);
        texcoords.reserve(surface_mesh.num_vertices() * 2);

        uint32_t max_index = 0;

        const SurfaceMesh::Property_map<VertexDescriptor, CGALPoint3> &location = surface_mesh.points();
        std::vector<int> reindex(surface_mesh.num_vertices());
        uint32_t n = 0;
        for (VertexIndex vd : surface_mesh.vertices()){
            const auto &p = location[vd];
            const UV &uv = uv_map[vd];
            positions.push_back(p.x());
            positions.push_back(p.y());
            positions.push_back(p.z());
            texcoords.push_back(uv.u);
            texcoords.push_back(uv.v);
            reindex[vd] = n++;
            max_index = std::max(max_index, n - 1);
        }

        indices.reserve(surface_mesh.num_faces() * 3);
        for (FaceIndex f : surface_mesh.faces()) {
            for (VertexIndex v : CGAL::vertices_around_face(surface_mesh.halfedge(f), surface_mesh)) {
                indices.push_back(reindex[v]);
            }
        }
        // for (Face_index f : surface_mesh.faces()) {
        //     for(Vertex_index v : CGAL::vertices_around_face(surface_mesh.halfedge(f), surface_mesh)) {
        //         indices.push_back(reindex[v]);
        //     }
        // }
    }

};
}

#endif