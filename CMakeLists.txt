cmake_minimum_required(VERSION 3.16)

project(Blueberry_Wolfz VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

qt_add_executable(Blueberry_Wolfz
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(Blueberry_Wolfz PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
)

if(WIN32)
    set_target_properties(Blueberry_Wolfz PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    set_target_properties(Blueberry_Wolfz PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

qt_finalize_executable(Blueberry_Wolfz)