cmake_minimum_required (VERSION 3.27)

project(expressvpn-applet)
set(APP expressvpn-applet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
    -Wall
    -Werror
    -fdiagnostics-color=always)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_compile_options(
    "$<$<CONFIG:DEBUG>:-fsanitize=address>")

add_link_options(
    "$<$<CONFIG:DEBUG>:-fsanitize=address>")


set(SOURCE_FILES
  src/config.cpp
  src/config-test.cpp
  src/config-prod.cpp
  src/controller.cpp
  src/evpn-base.cpp
  src/evpn-process.cpp
  src/icon-pix.cpp
  src/main.cpp
  src/model.cpp
  src/process.cpp
  src/view.cpp
  )

configure_file(src/resource.h.in ${CMAKE_CURRENT_BINARY_DIR}/resource.h)

find_package(Boost 1.83.0 COMPONENTS filesystem program_options REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(${APP} ${SOURCE_FILES})
target_include_directories(${APP} PRIVATE ${Boost_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS} src/)
target_link_libraries(${APP} ${Boost_LIBRARIES} ${GTKMM_LIBRARIES})
