#include <iostream>
#include <math.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

#include "mesh/cube.h"
#include "mesh/sphere.h"
#include "core/gltf/gltf.h"
#include "algorithm/cgal_clip.h"

int generate_cube() {
    mesh::MeshCube cube;
    cube.generate(mesh::Point3(0, 0, 0), 100, 100, 100);

    mesh::Mesh mesh;
    mesh.add_primitive(cube);

    mesh::Gltf gltf;
    gltf.add_mesh(mesh);

    gltf.write_to_file("../output/cube.glb");

    return 0;
}

int generate_sphere() {
    mesh::MeshSphere sphere;
    sphere.generate(mesh::Point3(0, 0, 0), 100);

    mesh::Mesh mesh;
    mesh.add_primitive(sphere);

    mesh::Gltf gltf;
    gltf.add_mesh(mesh);

    gltf.write_to_file("../output/sphere.glb");

    return 0;
}

int test_corefinement() {
    mesh::MeshCube cube;
    cube.generate(mesh::Point3(0, 0, 0), 80, 80, 80);
    mesh::MeshSphere sphere;
    sphere.generate(mesh::Point3(30, 30, 0), 60);

    mesh::Mesh mesh;
    mesh.add_primitive(cube);
    mesh.add_primitive(sphere);

    mesh::Gltf gltf;
    gltf.add_mesh(mesh);

    gltf.write_to_file("../output/cube_and_sphere.glb");

    mesh::CGALClip algo;

    {
        mesh::Primitive out;
        int ret = algo.corefine_difference(cube, sphere, out);

        mesh::Mesh mesh_out;
        mesh_out.add_primitive(out);
        mesh::Gltf gltf_out;
        gltf_out.add_mesh(mesh_out);
        gltf_out.write_to_file("../output/corefine_difference.glb");
    }

    {
        mesh::Primitive out;
        int ret = algo.corefine_intersection(cube, sphere, out);

        mesh::Mesh mesh_out;
        mesh_out.add_primitive(out);
        mesh::Gltf gltf_out;
        gltf_out.add_mesh(mesh_out);
        gltf_out.write_to_file("../output/corefine_intersection.glb");
    }

    return 0;
}

int main() {
    generate_cube();
    generate_sphere();
    test_corefinement();
}