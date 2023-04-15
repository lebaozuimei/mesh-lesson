#include <iostream>
#include <math.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

#include "mesh/cube.h"
#include "mesh/sphere.h"
#include "mesh/plane.h"
#include "core/math/point3.h"
#include "core/math/vector3.h"

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
    sphere.generate(mesh::Point3(30, 30, 0), 60, 20, 10);

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

    // {
    //     mesh::Primitive out;
    //     int ret = algo.corefine_union(cube, sphere, out);

    //     mesh::Mesh mesh_out;
    //     mesh_out.add_primitive(out);
    //     mesh::Gltf gltf_out;
    //     gltf_out.add_mesh(mesh_out);
    //     gltf_out.write_to_file("../output/corefine_union.glb");
    // }

    return 0;
}

int test_clip() {
    mesh::MeshCube cube;
    cube.generate(mesh::Point3(0, 0, 0), 80, 80, 80);
    mesh::MeshPlane plane;
    plane.generate(mesh::Point3(0, 0, 25), 160, 160, mesh::Vector3(0, 2, 2), mesh::Vector3(0, 1, 2));

    mesh::Mesh mesh;
    mesh.add_primitive(cube);
    mesh.add_primitive(plane);

    mesh::Gltf gltf;
    gltf.add_mesh(mesh);

    gltf.write_to_file("../output/cube_and_plane.glb");

    mesh::CGALClip algo;
    {
        mesh::Primitive out;
        int ret = algo.clip(cube, plane, out, false);

        mesh::Mesh mesh_out;
        mesh_out.add_primitive(out);
        mesh::Gltf gltf_out;
        gltf_out.add_mesh(mesh_out);
        gltf_out.write_to_file("../output/clip.glb");
    }

    {
        mesh::Primitive out;
        int ret = algo.clip(cube, plane, out, true);

        mesh::Mesh mesh_out;
        mesh_out.add_primitive(out);
        mesh::Gltf gltf_out;
        gltf_out.add_mesh(mesh_out);
        gltf_out.write_to_file("../output/clip_closed.glb");
    }

    // {
    //     mesh::MeshSphere sphere;
    //     sphere.generate(mesh::Point3(31.1, 32.2, 3.3), 60, 30, 20);
    //     mesh::Primitive out;
    //     int ret = algo.clip(cube, sphere, out, true);

    //     mesh::Mesh mesh_out;
    //     mesh_out.add_primitive(out);
    //     mesh::Gltf gltf_out;
    //     gltf_out.add_mesh(mesh_out);
    //     gltf_out.write_to_file("../output/clip_with_sphere.glb");
    // }

    // {
    //     mesh::Primitive out;
    //     int ret = algo.corefine_intersection(cube, plane, out);

    //     mesh::Mesh mesh_out;
    //     mesh_out.add_primitive(out);
    //     mesh::Gltf gltf_out;
    //     gltf_out.add_mesh(mesh_out);
    //     gltf_out.write_to_file("../output/clip.glb");
    // }

    // {
    //     mesh::Primitive out;
    //     int ret = algo.clip(plane, cube, out, false);

    //     mesh::Mesh mesh_out;
    //     mesh_out.add_primitive(out);
    //     mesh::Gltf gltf_out;
    //     gltf_out.add_mesh(mesh_out);
    //     gltf_out.write_to_file("../output/clip.glb");
    // }

    return 0;
}

int main() {
    generate_cube();
    generate_sphere();
    test_corefinement();
    test_clip();
}