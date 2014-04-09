cmake_minimum_required(VERSION 2.8.0)

project(fluidsim)

find_package(SFML 2 COMPONENTS audio graphics system window REQUIRED)
find_package(GLM REQUIRED)

add_definitions(-Wall -Wextra -g -Og -pedantic -fPIC -std=c++11 -Wshadow -Wno-unused-parameter -fopenmp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(
    "${CMAKE_SOURCE_DIR}/src/"
    SYSTEM ${SFML_INCLUDE_DIRS}
    SYSTEM ${GLM_INCLUDE_DIRS}
)

aux_source_directory(src/ sources)

add_executable(${CMAKE_PROJECT_NAME}
    ${sources}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${SFML_LIBRARIES}
    -lgomp
)
