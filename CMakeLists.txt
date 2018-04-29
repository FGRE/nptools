cmake_minimum_required(VERSION 2.8)
project(nptools)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# set version
set(NPTOOLS_VERSION_MAJOR "0")
set(NPTOOLS_VERSION_MINOR "5")
set(NPTOOLS_VERSION_PATCH "1")
set(NPTOOLS_VERSION "${NPTOOLS_VERSION_MAJOR}.${NPTOOLS_VERSION_MINOR}.${NPTOOLS_VERSION_PATCH}")

# append git revision if available
include(get-git-revision)
get_git_revision(NPTOOLS_VERSION_GIT)
if(NOT "${NPTOOLS_VERSION_GIT}" STREQUAL "")
    set(NPTOOLS_VERSION "${NPTOOLS_VERSION}-${NPTOOLS_VERSION_GIT}")
endif()

message(STATUS "Configuring nptools version " ${NPTOOLS_VERSION})

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(ZLIB REQUIRED)

# libunshield doesn't work in MSVC, not a big deal,
# npinstall isn't needed on Windows anyway.
if (NOT WIN32)
	find_package(LIBUNSHIELD)
	if (NOT LIBUNSHIELD_FOUND)
		message(FATAL_ERROR "Could not find libunshield")
	endif()
endif()

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

add_executable(npaextract src/npaextract.cpp)
add_executable(npapack src/npapack.cpp)
add_executable(nsbparse src/nsbparse.cpp)
add_executable(nsbcompile2 src/nsbcompile2.cpp)
add_executable(nsbparse2 src/nsbparse2.cpp)
add_executable(npcrypt src/npcrypt.cpp)
add_executable(npfparse src/npfparse.cpp)

# We don't need npinstall on Windows
if (NOT WIN32)
	add_executable(npinstall src/npinstall.cpp)
endif()

target_link_libraries(npaextract npa ${ZLIB_LIBRARIES})
target_link_libraries(npapack ${Boost_LIBRARIES} npa)
target_link_libraries(nsbparse npa)
target_link_libraries(nsbcompile2 npa)
target_link_libraries(nsbparse2 npa)
target_link_libraries(npcrypt npa)
target_link_libraries(npfparse npa)

# We don't need npinstall on Windows
if (NOT WIN32)
	target_link_libraries(npinstall npa -lunshield)
endif()
