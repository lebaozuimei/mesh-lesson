cmake_minimum_required(VERSION 3.1...3.15)
project(mesh-lesson)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall")
include_directories(${PROJECT_SOURCE_DIR}/deps)

set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

find_package(CGAL REQUIRED OPTIONAL_COMPONENTS)
find_package(Boost COMPONENTS filesystem system)

add_executable(lesson_basic src/lesson_basic.cpp)
target_link_libraries(lesson_basic CGAL::CGAL ${Boost_SYSTEM_LIBRARY})

add_executable(lesson_simplification src/lesson_simplification.cpp)
target_link_libraries(lesson_simplification CGAL::CGAL ${Boost_SYSTEM_LIBRARY})
