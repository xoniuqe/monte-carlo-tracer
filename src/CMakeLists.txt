cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(hpc VERSION 0.1 LANGUAGES CXX)

set(DCMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 11)

message("project sourc dir:")
message(${PROJECT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/)
message(${CMAKE_MODULE_PATH})

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Include Boost as an imported target
find_package(Boost COMPONENTS program_options REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

# Some other library that we import that was also built using CMake
# and has an exported target.
#find_package(MyOtherLibrary REQUIRED)

# Targets that we develop here
enable_testing()
#add_subdirectory(liblib)
add_subdirectory(cmake/modules)
add_subdirectory(app)
