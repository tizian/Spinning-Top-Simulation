cmake_minimum_required(VERSION 2.6)

project(spinningtops)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

FIND_PACKAGE(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(SOIL REQUIRED)
find_package(GLM REQUIRED)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${OPENGL_INCLUDE_DIR}"
	"${GLFW_INCLUDE_DIR}"
	"${GLEW_INCLUDE_DIR}"
	"${ASSIMP_INCLUDE_DIR}"
	"${SOIL_INCLUDE_DIRS}"
	"${GLM_INCLUDE_DIRS}"
)

file(GLOB_RECURSE ALL_FILES
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(spinningtops ${ALL_FILES})

target_link_libraries(spinningtops ${OPENGL_LIBRARIES} glfw3 ${GLFW_LIBRARIES} glew ${ASSIMP_LIBRARIES} ${SOIL_LIBRARIES})