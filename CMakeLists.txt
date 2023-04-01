cmake_minimum_required(VERSION 3.24)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

################################################################################
# Project.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")
include(parsertongueVersionString)
project(ParserTongue VERSION ${PARSERTONGUE_VERSION} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

################################################################################
# CMake modules.
################################################################################

set(TZ_CMAKE_MODULES_DIR "" CACHE PATH "Path to the cmake-modules package directory")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_BINARY_DIR};${TZ_CMAKE_MODULES_DIR}")
include(InstallTarget)
include(ISA)
include(MakeTarget)

################################################################################
# Add subdirectories.
################################################################################

add_subdirectory(modules)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
