cmake_minimum_required(VERSION 3.24)
project(RGZ_savin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/asavin/Qt/6.4.2/gcc_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(RGZ_savin main.cpp gui/scene.cpp gui/scene.h
        model/graph/graph.h
        model/graph/graph.cpp
        model/graph/shortest_path.h
        model/graph/shortest_path.cpp
        )

target_link_libraries(RGZ_savin
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )

