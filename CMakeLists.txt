cmake_minimum_required(VERSION 3.14)

project(TD2Radio LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_library(TD2Radio SHARED
  TD2Radio_global.h
  config.cpp
  config.h
  config.ui
)

target_include_directories(TD2Radio PUBLIC ./include)

target_link_libraries(TD2Radio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(TD2Radio PRIVATE TD2RADIO_LIBRARY)
