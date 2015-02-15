cmake_minimum_required(VERSION 2.8)

project(nix_compress)

file(GLOB SHARED_SOURCES source/*.c)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	file(GLOB OS_SOURCES source/linux/*.c)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	file(GLOB OS_SOURCES source/windows/*.c)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable(nc ${SHARED_SOURCES} ${OS_SOURCES})
target_link_libraries(nc m)
