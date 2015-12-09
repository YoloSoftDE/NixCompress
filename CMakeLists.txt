cmake_minimum_required(VERSION 2.8)

project(nix_compress)

file(GLOB SHARED_SOURCES source/*.c)

#add_definitions(-DVISUALIZATION)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	file(GLOB OS_SOURCES source/linux/*.c)
	include_directories(source/linux)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	file(GLOB OS_SOURCES source/windows/*.c)
	include_directories(source/windows)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable(lz1337 ${SHARED_SOURCES} ${OS_SOURCES})
target_link_libraries(lz1337 m)
