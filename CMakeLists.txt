cmake_minimum_required(VERSION 3.24)
project(RayCast)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/6.4.2/mingw_64/lib/cmake")


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(RayCast main.cpp Ray.cpp Polygon.cpp Controller.cpp MainWindow.cpp ControllerWidget.cpp icon.qrc)
target_link_libraries(RayCast
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )