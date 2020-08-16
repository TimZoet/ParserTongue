cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

################################################################################
# Project.
################################################################################

project(ParserTongue VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

################################################################################
# CMake modules.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ISA)
include(MakeTarget)

################################################################################
# Add subdirectories.
################################################################################

add_subdirectory(examples)
add_subdirectory(modules)

# Can run these after add_subdirectory calls.
# get_property(TARGETS_ALL GLOBAL PROPERTY TARGETS_ALL)
# get_property(TARGETS_APPLICATIONS GLOBAL PROPERTY TARGETS_APPLICATIONS)
# get_property(TARGETS_MODULES GLOBAL PROPERTY TARGETS_MODULES)
# get_property(TARGETS_TESTS GLOBAL PROPERTY TARGETS_TESTS)

################################################################################
# Set startup project.
################################################################################

get_property(STARTUP_PROJECT GLOBAL PROPERTY STARTUP_PROJECT)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJECT})
