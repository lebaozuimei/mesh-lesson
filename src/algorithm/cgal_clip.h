#ifndef  __ALGORITHM_CGAL_CLIP_H_
#define  __ALGORITHM_CGAL_CLIP_H_

#include "cgal_base.h"

#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/repair_self_intersections.h>
#include <CGAL/boost/graph/selection.h>
#include <CGAL/Polygon_mesh_processing/stitch_borders.h>
#include <CGAL/Polygon_mesh_processing/clip.h>
#include <CGAL/Polygon_mesh_processing/locate.h>

#include <iomanip>

namespace mesh {

namespace pmp = CGAL::Polygon_mesh_processing;
namespace params = pmp::parameters;

class CGALClip : public CGALBase {

public:

    typedef boost::graph_traits<SurfaceMesh>::edge_descriptor edge_descriptor;

    int corefine_difference(const Primitive &main, const Primitive &aux, Primitive &out) {
        SurfaceMesh surface_mesh_main;
        SurfaceMesh surface_mesh_aux;
        _gltf_primitive_to_surface_mesh(main, surface_mesh_main);
        _gltf_primitive_to_surface_mesh(aux, surface_mesh_aux);

        SurfaceMesh surface_mesh_out;
        bool valid = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(
            surface_mesh_main, surface_mesh_aux, surface_mesh_out
        );

        if (valid) {
            _surface_mesh_to_gltf_primitive(surface_mesh_out, out);
        } else {
            return -1;
        }
        return 0;
    }

    int corefine_intersection(const Primitive &main, const Primitive &aux, Primitive &out) {
        SurfaceMesh surface_mesh_main;
        SurfaceMesh surface_mesh_aux;
        _gltf_primitive_to_surface_mesh(main, surface_mesh_main);
        _gltf_primitive_to_surface_mesh(aux, surface_mesh_aux);

        SurfaceMesh surface_mesh_out;
        bool valid = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(
            surface_mesh_main, surface_mesh_aux, surface_mesh_out
        );

        if (valid) {
            _surface_mesh_to_gltf_primitive(surface_mesh_out, out);
        } else {
            return -1;
        }
        return 0;
    }

    // int corefine_union(const Primitive &main, const Primitive &aux, Primitive &out) {
    //     SurfaceMesh surface_mesh_main;
    //     SurfaceMesh surface_mesh_aux;
    //     _gltf_primitive_to_surface_mesh(main, surface_mesh_main);
    //     _gltf_primitive_to_surface_mesh(aux, surface_mesh_aux);\

    //     CGAL::Polygon_mesh_processing::stitch_borders(surface_mesh_main);
    //     CGAL::Polygon_mesh_processing::stitch_borders(surface_mesh_aux);

    //     SurfaceMesh surface_mesh_out;
    //     bool valid = CGAL::Polygon_mesh_processing::corefine_and_compute_union(
    //         surface_mesh_main, surface_mesh_aux, surface_mesh_out
    //     );

    //     if (valid) {
    //         _surface_mesh_to_gltf_primitive(surface_mesh_out, out);
    //     } else {
    //         return -1;
    //     }
    //     return 0;
    // }

    int clip(const Primitive &main, const Primitive &aux, Primitive &out, bool clip_volume = false) {
        SurfaceMesh surface_mesh_main;
        SurfaceMesh surface_mesh_aux;
        _gltf_primitive_to_surface_mesh(main, surface_mesh_main);
        _gltf_primitive_to_surface_mesh(aux, surface_mesh_aux);

        // DEBUG(CGAL::Polygon_mesh_processing::does_bound_a_volume(surface_mesh_aux));
        CGAL::Polygon_mesh_processing::reverse_face_orientations(surface_mesh_aux);

        bool valid = CGAL::Polygon_mesh_processing::clip(
            surface_mesh_main,
            surface_mesh_aux,
            params::clip_volume(clip_volume), // parameters for surface_mesh_main
            params::all_default() // parameters for surface_mesh_aux
        );

        if (valid) {
            _surface_mesh_to_gltf_primitive(surface_mesh_main, out);
        } else {
            return -1;
        }
        return 0;
    }

    // int corefine_difference(const Primitive &main, const Primitive &aux, Primitive &out) {
    //     SurfaceMesh surface_mesh_main;
    //     SurfaceMesh surface_mesh_aux;
    //     SurfaceMesh surface_mesh_out;
    //     _gltf_primitive_to_surface_mesh(main, surface_mesh_main);
    //     _gltf_primitive_to_surface_mesh(aux, surface_mesh_aux);

    //     // CGAL::Polygon_mesh_processing::stitch_borders(surface_mesh_main);
    //     // CGAL::Polygon_mesh_processing::stitch_borders(surface_mesh_aux);

    //     // SurfaceMesh::Property_map<edge_descriptor,bool> is_constrained_map =
    //     //     surface_mesh_main.add_property_map<edge_descriptor, bool>("e:is_constrained", false).first;

    //     // bool valid = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(
    //     //     surface_mesh_main,
    //     //     surface_mesh_aux,
    //     //     surface_mesh_out,
    //     //     params::throw_on_self_intersection(false), // default parameters for surface_mesh_main
    //     //     params::all_default(), // default parameters for surface_mesh_aux
    //     //     params::edge_is_constrained_map(is_constrained_map)
    //     // );

    //     bool valid = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(
    //     // bool valid = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(
    //         surface_mesh_main,
    //         surface_mesh_aux,
    //         surface_mesh_out
    //     );

    //     if (valid) {
    //         // int vertex_count_before_process = surface_mesh_out.num_vertices();
    //         // int delta = surface_mesh_out.num_vertices() - surface_mesh_main.num_vertices();
    //         // if (delta > 0) {
    //         //     std::cout << "corefinement successfully with vertex delta: " << delta << std::endl;
    //         // }
    //         _surface_mesh_to_gltf_primitive(surface_mesh_out, out);
    //     } else {
    //         // std::cout << "corefinement failed" << std::endl;
    //         return -1;
    //     }
    //     return 0;
    // }

};
}

#endif