cmake_minimum_required(VERSION 3.22)
project(CPURAYTRACER)

set(CMAKE_CXX_STANDARD 23)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "-lX11 -lGL -lpthread -lpng -lstdc++fs") #flags for olcPixelGameEngine
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -mavx -ffast-math")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 3.3 REQUIRED NO_MODULES)

add_executable(CPURAYTRACER main.cpp Engine.h Renderer.h Camera.h Vertex.h LinearAlgebra.h Triangle.h Texture.h Model.h ResourceLoader.h Octree.h RayHit.h Box.h)

include_directories(. /usr/include/ stb)

target_link_libraries(CPURAYTRACER Eigen3::Eigen /usr/lib/libassimp.so)
