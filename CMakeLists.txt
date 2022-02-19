cmake_minimum_required(VERSION 3.10)

# set the project name
project(gps-sdr-sim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)


if(NOT DEFINED ${LIMESDR_HEADER_DIR})
set(LIMESDR_HEADER_DIR "C:/Program Files/PothosSDR/include/" FILEPATH)
message ("Set LIMESDR_HEADER_DIR to ${LIMESDR_HEADER_DIR}")
endif(NOT DEFINED ${LIMESDR_HEADER_DIR})


if(NOT DEFINED ${LIMESDR_LIB_DIR})
set(LIMESDR_LIB_DIR "C:/Program Files/PothosSDR/lib/" FILEPATH)
message ("Set LIMESDR_LIB_DIR to ${LIMESDR_LIB_DIR}")
endif(NOT DEFINED ${LIMESDR_LIB_DIR})


find_path(GETOPT_INCLUDE_DIR getopt.h)
find_library(GETOPT_LIB_DIR getopt)
include_directories(${GETOPT_INCLUDE_DIR})


include_directories(${LIMESDR_HEADER_DIR})
link_directories(${LIMESDR_LIB_DIR})


find_package(Catch2 CONFIG REQUIRED)
find_package(Boost REQUIRED program_options)
find_package(Boost REQUIRED filesystem)
find_package(ZLIB REQUIRED)

add_executable(gpssim "main.cpp" "gpssim.c" "gpssim.h" "gpssim.cpp")


target_link_libraries(gpssim PRIVATE LimeSuite)
target_link_libraries(gpssim PRIVATE pthreadVC2)
target_link_libraries(gpssim PRIVATE Catch2::Catch2)
target_link_libraries(gpssim PRIVATE Boost::boost Boost::program_options Boost::filesystem)
target_link_libraries(gpssim PRIVATE ${GETOPT_LIB_DIR})
target_link_libraries(gpssim PRIVATE ZLIB::ZLIB)


target_compile_features(gpssim PRIVATE cxx_std_20)
set_property(TARGET gpssim PROPERTY CXX_STANDARD 20)

file(COPY "resources/brdc3540.14n" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "resources/GEN_TEST_1_ref.iq" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})