cmake_minimum_required( VERSION 3.0 )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()
set( CMAKE_CXX_FLAGS "-Wall -Wextra" )
set( CMAKE_CXX_FLAGS_DEBUG "-g" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3" )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )

project( OpenGL-Starter )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/include
)
link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/extern)

set( EXAMPLES
  triangle
)

set( COMMON_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vapp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LoadShaders.cpp)
set( COMMON_LIBS
  glfw)

foreach( EXAMPLE ${EXAMPLES} )
  add_executable( ${EXAMPLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE}.cpp
    ${COMMON_SOURCE} )
  target_link_libraries( ${EXAMPLE} ${COMMON_LIBS} )
endforeach( EXAMPLE )
