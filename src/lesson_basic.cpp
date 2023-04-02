#include <iostream>
#include <math.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <CGAL/Surface_mesh.h>



// typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
// typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;

// typedef Kernel::Point_2 Point_2;

// typedef CGAL::Surface_mesh<Kernel::Point_3> SurfaceMesh;

// typedef boost::graph_traits<SurfaceMesh>::vertex_descriptor VertexDescriptor;
// typedef SurfaceMesh::Vertex_index VertexIndex;
// typedef boost::graph_traits<SurfaceMesh>::vertex_iterator VertexIterator;

// typedef boost::graph_traits<SurfaceMesh>::edge_descriptor EdgeDescriptor;
// typedef boost::graph_traits<SurfaceMesh>::halfedge_descriptor HalfedgeDescriptor;
// typedef boost::graph_traits<SurfaceMesh>::face_descriptor FaceDescriptor;

// typedef CGAL::Out_edge_iterator<SurfaceMesh> OutEdgeIterator;

// int main() {
//     double a;
//     std::cin >> a;
//     std::cout << ((a / 0.9 == 0.7) ? "true" : "false") << std::endl;
// }

int test_descriptor() {
    typedef CGAL::Simple_cartesian<double> Kernel;
    typedef CGAL::Surface_mesh<Kernel::Point_3> SurfaceMesh;

    typedef boost::graph_traits<SurfaceMesh>::vertex_descriptor VertexDescriptor;
    typedef SurfaceMesh::Vertex_index VertexIndex;
    typedef boost::graph_traits<SurfaceMesh>::vertex_iterator VertexIterator;
    typedef boost::graph_traits<SurfaceMesh>::edge_descriptor EdgeDescriptor;
    typedef boost::graph_traits<SurfaceMesh>::halfedge_descriptor HalfedgeDescriptor;
    typedef boost::graph_traits<SurfaceMesh>::face_descriptor FaceDescriptor;

    SurfaceMesh mesh;
    std::string model_path = "../data/model/cube.off";
    if (!CGAL::IO::read_polygon_mesh(model_path, mesh)) {
        std::cerr << "read model failed: " << model_path << std::endl;
        return -1;
    }

    std::cout << "================ descriptor =================" << std::endl;

    for (VertexDescriptor vd : vertices(mesh)) {
        std::cout << "vertex descriptor: " << vd << std::endl;
        std::cout << " point: " << mesh.points()[vd] << std::endl;
    }

    for (VertexIndex vi : mesh.vertices()) {
        std::cout << "vertex index: " << vi << std::endl;
    }

    std::pair<VertexIterator, VertexIterator> pair = vertices(mesh);
    for (VertexIterator iter = pair.first; iter != pair.second; ++iter) {
        std::cout << "vertex iterator: " << *iter << std::endl;
    }

    for (EdgeDescriptor ed : mesh.edges()) {
        std::cout << "edge descriptor: " << ed << std::endl;
        // std::cout << mesh.edges()[ed] << std::endl;
    }

    for (HalfedgeDescriptor hd : mesh.halfedges()) {
        std::cout << "halfedge descriptor: " << hd << std::endl;
        for (VertexDescriptor vd : CGAL::vertices_around_face(hd, mesh)) {
            std::cout << "  vertex descriptor around face: " << vd << std::endl;
        }
        for (VertexDescriptor vd : CGAL::vertices_around_face(hd, mesh)) {
            std::cout << "  vertex descriptor around target: " << vd << std::endl;
        }
    }

    for (FaceDescriptor fd : mesh.faces()) {
        std::cout << "face descriptor: " << fd << std::endl;
    }

    return 0;
}

int test_kernel_precision() {

    std::cout << "================ collinear precision =================" << std::endl;

// ================ test for simple kernel =================
    typedef CGAL::Simple_cartesian<double> SimpleKernel;
    typedef SimpleKernel::Point_2 SimplePoint_2;

    SimplePoint_2 simple_p(1.0, 0.1);
    SimplePoint_2 simple_q(2.0, 0.2);
    SimplePoint_2 simple_r(3.0, 0.3);
    std::cout << "simple kernel (define directly), collinear: " 
        << (CGAL::collinear(simple_p, simple_q, simple_r) ? "true" : "false") << std::endl;

    std::istringstream simple_iss("1.0 0.1 2.0 0.2 3.0 0.3");
    simple_iss >> simple_p >> simple_q >> simple_r;
    std::cout << "simple kernel (read from stream), collinear: " 
        << (CGAL::collinear(simple_p, simple_q, simple_r) ? "true" : "false") << std::endl;

// ================ test for exact/exact kernel =================
    typedef CGAL::Exact_predicates_exact_constructions_kernel EEKernel;
    typedef EEKernel::Point_2 EEPoint_2;

    EEPoint_2 ee_p(1.0, 0.1);
    EEPoint_2 ee_q(2.0, 0.2);
    EEPoint_2 ee_r(3.0, 0.3);
    std::cout << "exact_exact kernel (define directly), collinear: " 
        << (CGAL::collinear(ee_p, ee_q, ee_r) ? "true" : "false") << std::endl;

    std::istringstream ee_iss("1.0 0.1 2.0 0.2 3.0 0.3");
    ee_iss >> ee_p >> ee_q >> ee_r;
    std::cout << "exact_exact kernel (read from stream), collinear: " 
        << (CGAL::collinear(ee_p, ee_q, ee_r) ? "true" : "false") << std::endl;

// ================ test for exact/inexact kernel =================
    typedef CGAL::Exact_predicates_inexact_constructions_kernel EIKernel;
    typedef EIKernel::Point_2 EIPoint_2;

    EIPoint_2 ei_p(1.0, 0.1);
    EIPoint_2 ei_q(2.0, 0.2);
    EIPoint_2 ei_r(3.0, 0.3);
    std::cout << "exact_inexact kernel (define directly), collinear: " 
        << (CGAL::collinear(ei_p, ei_q, ei_r) ? "true" : "false") << std::endl;

    std::istringstream ei_iss("1.0 0.1 2.0 0.2 3.0 0.3");
    ei_iss >> ei_p >> ei_q >> ei_r;
    std::cout << "exact_inexact kernel (read from stream), collinear: " 
        << (CGAL::collinear(ei_p, ei_q, ei_r) ? "true" : "false") << std::endl;

    return 0;
}

int main() {
    test_descriptor();
    test_kernel_precision();

    return 0;
}
