# CMake build script for scpi-server-tools.
# Intended to be integrated into a larger project, not built standalone.

add_library(scpi-server-tools STATIC
	BridgeSCPIServer.cpp
	SCPIServer.cpp)

target_include_directories(scpi-server-tools
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../log
	)
