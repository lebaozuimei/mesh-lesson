#include <iostream>
#include <math.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

// #include "reader/off_reader.h"
// #include "writer/obj_writer.h"
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Edge_collapse_visitor_base.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Constrained_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>

// #include <CGAL/Polygon_mesh_processing/repair_self_intersections.h>

#include "core/gltf/gltf.h"
#include "algorithm/cgal_simplification.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Surface_mesh<Kernel::Point_3> Surface_mesh;
namespace SMS = CGAL::Surface_mesh_simplification;

int simplify_cube() {
    std::cout << "================ simplify cube ================" << std::endl;

    Surface_mesh mesh;
    std::string input_path = "../data/model/cube.off";
    if (!CGAL::IO::read_OFF(input_path, mesh)) {
        std::cerr << "read input failed: " << input_path << std::endl;
        return -1;
    }

    std::cout << " ---- original ----" << std::endl;
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;

    SMS::Count_ratio_stop_predicate<Surface_mesh> stop(0.3);
    int removed = SMS::edge_collapse(mesh, stop);

    std::cout << " ---- after collapse ----" << std::endl;
    std::cout << "edge removed: " << removed << std::endl;
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;


    std::cout << " ---- after collect garbage ----" << std::endl;
    mesh.collect_garbage();
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;
    
    std::string output_path = "../output/cube_simplified.obj";
    if (!CGAL::IO::write_OBJ(output_path, mesh)) {
        std::cerr << "write failed: " << output_path << std::endl;
        return -1;
    }

    std::cout << "simplify cube complete, model saved to: " << output_path << std::endl;
    return 0;
}

struct Border_is_constrained_edge_map {
    typedef boost::graph_traits<Surface_mesh>::edge_descriptor edge_descriptor;

    Border_is_constrained_edge_map(const Surface_mesh &mesh) : mesh_ptr(&mesh) {

    }

    friend bool get(const Border_is_constrained_edge_map &bem, const edge_descriptor &edge) {
        return CGAL::is_border(edge, *bem.mesh_ptr);
    }

    const Surface_mesh *mesh_ptr;
};

int simplify_plane_without_border() {
    std::cout << "================ simplify plane (without border) ================" << std::endl;

    Surface_mesh mesh;
    std::string input_path = "../data/model/plane.off";
    if (!CGAL::IO::read_OFF(input_path, mesh)) {
        std::cerr << "read input failed: " << input_path << std::endl;
        return -1;
    }

    std::cout << " ---- original ----" << std::endl;
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;

    SMS::Count_ratio_stop_predicate<Surface_mesh> stop(0.01);
    int removed = SMS::edge_collapse(mesh, stop);

    std::cout << " ---- after collapse ----" << std::endl;
    std::cout << "edge removed: " << removed << std::endl;
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;


    std::cout << " ---- after collect garbage ----" << std::endl;
    mesh.collect_garbage();
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;
    
    std::string output_path = "../output/plane_without_border_simplified.obj";
    if (!CGAL::IO::write_OBJ(output_path, mesh)) {
        std::cerr << "write failed: " << output_path << std::endl;
        return -1;
    }

    std::cout << "simplify plane without border complete, model saved to: " << output_path << std::endl;

    return 0;
}

int simplify_plane_with_border() {
    std::cout << "================ simplify plane (with border) ================" << std::endl;

    Surface_mesh mesh;
    std::string input_path = "../data/model/plane.off";
    if (!CGAL::IO::read_OFF(input_path, mesh)) {
        std::cerr << "read input failed: " << input_path << std::endl;
        return -1;
    }

    std::cout << " ---- original ----" << std::endl;
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;

    SMS::Count_ratio_stop_predicate<Surface_mesh> stop(0.01);
    Border_is_constrained_edge_map bem(mesh);
    int removed = SMS::edge_collapse(mesh, stop, CGAL::parameters::edge_is_constrained_map(bem));

    std::cout << " ---- after collapse ----" << std::endl;
    std::cout << "edge removed: " << removed << std::endl;
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;

    std::cout << " ---- after collect garbage ----" << std::endl;
    mesh.collect_garbage();
    std::cout << "vertex count: " << mesh.num_vertices() << std::endl;
    std::cout << "edge count: " << mesh.number_of_edges() << std::endl;
    std::cout << "face count: " << mesh.num_faces() << std::endl;
    
    std::string output_path = "../output/plane_with_border_simplified.obj";
    if (!CGAL::IO::write_OBJ(output_path, mesh)) {
        std::cerr << "write failed: " << output_path << std::endl;
        return -1;
    }

    std::cout << "simplify plane with border complete, model saved to: " << output_path << std::endl;

    return 0;
}

int simplify_with_texture() {
    mesh::Gltf gltf;
    gltf.read_from_file("../data/model/duck.glb");

    mesh::CGALSimplification algo;
    algo.run(gltf, 0.4);

    gltf.write_to_file("../output/duck_simplified.glb");

    return 0;
}

int main() {
    simplify_cube();
    simplify_plane_without_border();
    simplify_plane_with_border();
    simplify_with_texture();
}