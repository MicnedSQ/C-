cmake_minimum_required(VERSION 3.17)
project(Sieci)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        include
        include/config.hpp
)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Werror=switch)

set(SOURCE_FILES
        src/factory.cpp
        src/helpers.cpp
        src/nodes.cpp
        src/package.cpp
        src/reports.cpp
        src/simulation.cpp
        src/storage_types.cpp
        )

add_executable(Sieci_debug ${SOURCE_FILES} main.cpp)

set(SOURCES_FILES_TESTS
        test/test_storage_types.cpp
        test/test_package.cpp
        test/test_Factory.cpp
        test/test_factory_io.cpp
        test/test_nodes.cpp
#        test/test_reports.cpp
        test/test_simulate.cpp
        )

add_executable(Sieci_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_include_directories(Sieci_test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

add_subdirectory(googletest-master)

target_link_libraries(Sieci_test gmock)