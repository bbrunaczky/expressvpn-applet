cmake_minimum_required (VERSION 3.27)

project(expressvpn-applet)
set(APP expressvpn-applet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
    -Wall
    -Werror
    -fdiagnostics-color=always)

include_directories(BEFORE ./ ../util)


add_compile_options(
    "$<$<CONFIG:DEBUG>:-fsanitize=address>")

add_link_options(
    "$<$<CONFIG:DEBUG>:-fsanitize=address>")


set(SOURCE_FILES
  src/controller.cpp
  src/evpn-base.cpp
  src/evpn-process.cpp
  src/main.cpp
  src/model.cpp
  src/process.cpp
  src/view.cpp
  )

find_package(Boost 1.4.0 COMPONENTS filesystem REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(${APP} ${SOURCE_FILES})
target_include_directories(${APP} PRIVATE ${Boost_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS} src/)
target_link_libraries(${APP} ${Boost_FILESYSTEM_LIBRARY} ${GTKMM_LIBRARIES})
