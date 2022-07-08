cmake_minimum_required( VERSION 3.0 )

add_compile_definitions( NDEBUG )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()

if("${CMAKE_GENERATOR}" MATCHES "(Visual Studio)" )
  # 1. Displays level 1, level 2, and level 3 warnings, and all level 4 (informational) warnings
  # 2. Specifies the exception handling model.
  #   s: Enables standard C++ stack unwinding. Catches only standard C++ exceptions when you use catch(...) syntax.
  #   c: assumes that functions declared as extern "C" never throw a C++ exception.
  set( CMAKE_CXX_FLAGS "/W4 /EHsc" )
else()
  set( CMAKE_CXX_FLAGS "-Wall -Wextra" )
  set( CMAKE_CXX_FLAGS_DEBUG "-g" )
  set( CMAKE_CXX_FLAGS_RELEASE "-O3" )
endif()

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )

project( OpenGL-Starter )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw )

set( INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/cxxtimer
  ${CMAKE_SOURCE_DIR}/extern/imgui
  ${CMAKE_SOURCE_DIR}/extern/imgui/backends
)

include_directories( ${INCLUDE_DIRS} )
link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/extern
)

file( GLOB EXAMPLES LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}/examples" "${CMAKE_SOURCE_DIR}/examples/*" )

file( GLOB COMMON_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/*" )
list( APPEND COMMON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/glad.c
)

add_library( imgui STATIC )
target_include_directories( imgui PUBLIC ${CMAKE_SOURCE_DIR}/extern/imgui )
target_sources( imgui PRIVATE
  ${CMAKE_SOURCE_DIR}/extern/imgui/imgui.h
  ${CMAKE_SOURCE_DIR}/extern/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_tables.cpp
  ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries( imgui glfw )

set( COMMON_LIBS
  glfw
  imgui
)

foreach( EXAMPLE ${EXAMPLES} )
  file( GLOB EXAMPLE_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE}/*.cpp" )

  message(STATUS "Found Example: ${EXAMPLE}")
  message( STATUS "Sources of ${EXAMPLE}: ${EXAMPLE_SOURCES}" )

  add_executable( ${EXAMPLE}
    ${EXAMPLE_SOURCES}
    ${COMMON_SOURCES}
  )
  target_link_libraries( ${EXAMPLE} ${COMMON_LIBS} )
endforeach( EXAMPLE )
