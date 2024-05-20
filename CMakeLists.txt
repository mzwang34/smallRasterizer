# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required( VERSION 3.15 )

set(VCPKG_ROOT_DIR "D:/software/vcpkg" CACHE PATH "set vcpkg path")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake")

# Set the project name
project ( smallRasterizer )

message( STATUS "CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}" )
message( STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}" )

# Add an executable
add_executable( smallRasterizer main.cpp model.h model.cpp shader.h tgaimage.h tgaimage.cpp geometry.h "transform.h" "pbrShader.h" shadowShader.h)