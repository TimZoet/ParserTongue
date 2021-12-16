cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

################################################################################
# Project.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/modules/parsertongue")
include(ParserTongueVersionString)
project(ParserTongue VERSION ${PARSERTONGUE_VERSION} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

################################################################################
# CMake modules.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
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

################################################################################
# Set startup project.
################################################################################

get_property(STARTUP_PROJECT GLOBAL PROPERTY STARTUP_PROJECT)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJECT})
