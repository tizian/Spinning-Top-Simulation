cmake_minimum_required(VERSION 3.21)

project(spinningtops)

if (NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/src)
    message(FATAL_ERROR
        "Seems like some of the required dependencies are missing. "
        "This can happen if you did not clone the project with the --recursive flag. "
        "It is possible to recover by calling \"git submodule update --init --recursive\""
    )
endif()

include(CheckCXXCompilerFlag)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if (MSVC)
    add_definitions (/D "__TBB_NO_IMPLICIT_LINKAGE")

    # parallel builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # static build
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
    )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

# enable modern c++ on clang or gcc
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    elseif (HAS_CPP11_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        message(FATAL_ERROR "Unsupported compiler. At least C++11 support is required.")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

include_directories(
	include
	ext/stb
	ext/glfw/include
	ext/gl3w/include
	ext/glm
	ext/tinyobjloader
)

find_package(OpenGL REQUIRED)

add_subdirectory("${PROJECT_SOURCE_DIR}/ext/gl3w")

set(GLFW_BUILD_DOCS     OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL " " FORCE)
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/glfw")

# compiler warnings
if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

file(GLOB_RECURSE MAIN_FILES
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

set(EXT_FILES
	ext/tinyobjloader/tiny_obj_loader.cc
)

if (WIN32)
    add_executable(SpinningTops
        ${MAIN_FILES}
        ${EXT_FILES}
        ${PROJECT_SOURCE_DIR}/res/info.rc
    )
    file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif (APPLE)
    # create .app file
    add_executable(SpinningTops MACOSX_BUNDLE
        ${MAIN_FILES}
        ${EXT_FILES}
        ${PROJECT_SOURCE_DIR}/res/spinningtops.icns
    )
    set_target_properties(SpinningTops PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/res/info.plist)
    set_source_files_properties(res/spinningtops.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SpinningTops.app/Contents/Resources)

    # create binary executable for easier debugging
    add_executable(SpinningTopsTMP
        ${MAIN_FILES}
        ${EXT_FILES}
    )
    file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(SpinningTopsTMP
        ${OPENGL_LIBRARIES}
        gl3w
        glfw
    )
set_target_properties(SpinningTopsTMP PROPERTIES  OUTPUT_NAME SpinningTops)
else()
    add_executable(SpinningTops
        ${MAIN_FILES}
        ${EXT_FILES}
    )
    file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(SpinningTops
	${OPENGL_LIBRARIES}
	gl3w
	glfw
)