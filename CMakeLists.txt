cmake_minimum_required( VERSION 3.0 )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()
# set( CMAKE_CXX_FLAGS "-Wall -Wextra" )
# set( CMAKE_CXX_FLAGS_DEBUG "-g" )
# set( CMAKE_CXX_FLAGS_RELEASE "-O3" )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )

project( OpenGL-Starter )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw )

set(INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb
)

# find_package(Boost REQUIRED COMPONENTS date_time program_options)
# if(Boost_FOUND)
#   list(APPEND INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
# else()
#   message(FATAL_ERROR "Boost NOT found")
# endif()

include_directories(${INCLUDE_DIRS})
link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/extern
)

file(GLOB EXAMPLES LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}/examples" "${CMAKE_SOURCE_DIR}/examples/*")
foreach(example ${EXAMPLES})
  message(STATUS "Found Example: ${example}")
endforeach()

set( COMMON_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vapp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LoadShaders.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CameraController.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Axes.cpp
  # ${Boost_INCLUDE_DIRS}
  )
set( COMMON_LIBS
  glfw
  # ${Boost_LIBRARIES}
)

foreach( EXAMPLE ${EXAMPLES} )
  add_executable( ${EXAMPLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE}/${EXAMPLE}.cpp
    ${COMMON_SOURCE} )
  target_link_libraries( ${EXAMPLE} ${COMMON_LIBS} )
endforeach( EXAMPLE )
