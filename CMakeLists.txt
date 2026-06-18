cmake_minimum_required(VERSION 3.16)
project(catatan_qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

add_executable(catatan_qt 
    main.cpp 
    mainwindow.cpp 
    mainwindow.h
)

target_link_libraries(catatan_qt PRIVATE 
    Qt6::Widgets 
    Qt6::WebEngineWidgets
)

# If you have resources
qt6_add_resources(catatan_qt "styles" FILES main.qss)
