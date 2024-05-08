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
  controller.cpp
  main.cpp
  model.cpp
  view.cpp
  )

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(${APP} ${SOURCE_FILES})
target_include_directories(${APP} PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_libraries(${APP} ${GTKMM_LIBRARIES})
