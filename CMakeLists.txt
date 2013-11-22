cmake_minimum_required(VERSION 2.8)
project(nptools)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++11)
endif()

find_package(Boost REQUIRED COMPONENTS system filesystem locale)

set(CMAKE_BUILD_TYPE Debug)

include_directories(
${CMAKE_SOURCE_DIR}/../libnpa/include
${Boost_INCLUDE_DIRS}
)
link_directories(
${Boost_LIBRARY_DIRS}
${CMAKE_SOURCE_DIR}/../libnpa/
)

add_executable(npaextract src/npaextract.cpp)
add_executable(npapack src/npapack.cpp)
add_executable(nsbparse src/nsbparse.cpp)
add_executable(nsbcompile src/nsbcompile.cpp)
add_executable(npinstall src/npinstall.cpp)

target_link_libraries(npaextract ${Boost_LIBRARIES} npa)
target_link_libraries(npapack ${Boost_LIBRARIES} npa)
target_link_libraries(nsbparse ${Boost_LIBRARIES} npa)
target_link_libraries(nsbcompile ${Boost_LIBRARIES} npa)
