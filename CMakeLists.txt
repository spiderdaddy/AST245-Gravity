cmake_minimum_required(VERSION 3.9)
project(Gravity)

set(CMAKE_CXX_STANDARD 11)

include_directories( BEFORE common )

add_executable(
        Gravity
        main.cpp
        graphics.cpp
        graphics.h
        Structure.cpp
        Structure.h
        common/shader.cpp
        common/shader.hpp
        GravityCalculator.cpp
        GravityCalculator.h
        DirectNBodyGravityCalculator.cpp
        DirectNBodyGravityCalculator.h OctaTree.cpp OctaTree.h OctaTreeMultipoleGravityCalculator.cpp OctaTreeMultipoleGravityCalculator.h)

target_link_libraries(Gravity glut GL GLEW GLU pthread freeimage glfw )