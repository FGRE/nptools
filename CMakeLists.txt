cmake_minimum_required(VERSION 2.8)
project(nptools)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(Boost REQUIRED COMPONENTS system filesystem locale)
find_package(ZLIB REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(LIBUNSHIELD)
if (NOT LIBUNSHIELD_FOUND)
	message(FATAL_ERROR "Could not find libunshield")
endif ()

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-std=c++11)

include_directories(
${CMAKE_SOURCE_DIR}/../libnpa/include
${CMAKE_SOURCE_DIR}/src
${Boost_INCLUDE_DIRS}
)
link_directories(
${Boost_LIBRARY_DIRS}
${CMAKE_SOURCE_DIR}/../libnpa/
)

flex_target(lexer ${CMAKE_SOURCE_DIR}/src/lexer.l ${CMAKE_SOURCE_DIR}/src/lexer.cpp)
bison_target(parser ${CMAKE_SOURCE_DIR}/src/parser.y ${CMAKE_SOURCE_DIR}/src/parser.cpp)

add_executable(npaextract src/npaextract.cpp)
add_executable(npapack src/npapack.cpp)
add_executable(nsbparse src/nsbparse.cpp)
add_executable(nsbcompile src/nsbcompile.cpp)
add_executable(npinstall src/npinstall.cpp)
add_executable(nsbcompile2 src/nsbcompile2.cpp src/parser.cpp src/lexer.cpp)
add_executable(nsbparse2 src/nsbparse2.cpp)
add_executable(npcrypt src/npcrypt.cpp)

target_link_libraries(npaextract ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} npa)
target_link_libraries(npapack ${Boost_LIBRARIES} npa)
target_link_libraries(nsbparse ${Boost_LIBRARIES} npa)
target_link_libraries(nsbcompile ${Boost_LIBRARIES} npa)
target_link_libraries(npinstall ${Boost_LIBRARIES} -lunshield)
target_link_libraries(nsbcompile2 ${Boost_LIBRARIES} npa)
target_link_libraries(nsbparse2 npa)
target_link_libraries(npcrypt npa)
