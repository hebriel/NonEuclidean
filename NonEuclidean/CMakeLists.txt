cmake_minimum_required(VERSION 3.14)
project(NonEuclidean)

set(CMAKE_CXX_STANDARD 17)

add_executable(NonEuclidean
        Camera.cpp
        Camera.hpp
        Collider.cpp
        Collider.hpp
        Engine.cpp
        Engine.hpp
        Floorplan.hpp
        FrameBuffer.cpp
        FrameBuffer.hpp
        GameHeader.hpp
        Ground.hpp
        House.hpp
        Input.cpp
        Input.hpp
        Level1.cpp
        Level1.hpp
        Level2.cpp
        Level2.hpp
        Level3.cpp
        Level3.hpp
        Level4.cpp
        Level4.hpp
        Level5.cpp
        Level5.hpp
        Level6.cpp
        Level7.hpp
        Level7.cpp
        Level6.hpp
        Main.cpp
        Mesh.cpp
        Mesh.hpp
        Object.cpp
        Object.hpp
        Physical.cpp
        Physical.hpp
        Pillar.hpp
        PillarRoom.hpp
        Player.cpp
        Player.hpp
        Portal.cpp
        Portal.hpp
        Resources.cpp
        Resources.hpp
        Scene.hpp
        Shader.cpp
        Shader.hpp
        Sky.hpp
        Sphere.hpp
        Statue.hpp
        Texture.cpp
        Texture.hpp
        Tunnel.hpp
        Vector.hpp
        Time.hpp
        Time.cpp)

find_library(OpenGL, GL_LIBRARY)

target_link_libraries(NonEuclidean -lSDL2)
target_link_libraries(NonEuclidean -lglew)
target_link_libraries(NonEuclidean "-framework OpenGL")

find_package (PkgConfig REQUIRED)
find_package (SDL2 REQUIRED)
find_package (GLEW REQUIRED STATIC)