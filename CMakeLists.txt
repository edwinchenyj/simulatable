# Based on https://github.com/robertmaynard/cmake_openmp_demo
# Based on the work from https://github.com/TartanLlama/expected
# Thanks to Isabella Muerte
cmake_minimum_required(VERSION 3.16)
project(Simulatable
  HOMEPAGE_URL https://github.com/edwinchenyj/simulatable
  DESCRIPTION "A collection of physics simulation algorithms"
  VERSION 0.0.1
  LANGUAGES CXX
  )

include(FetchContent)
include(CTest)

### C++ Standard Level

# State that C++14 is explicitly required to build our library
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()



### Add Library
set(BUILD_TESTING OFF) # to avoid building eigen3 tests
add_subdirectory(external)


# Build the lib that contains algorithms that
# We setup an alias of our library to what it would look like
# after installed
file(GLOB simulatable-src CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cxx)
message(STATUS "simulatable-src " simulatable-src)
add_library(libsimulatable STATIC ${simulatable-src})
add_library(Simulatable::libsimulatable ALIAS libsimulatable)


# Specify the installable headers of the lib. By specifying the headers
# as PUBLIC_HEADER
file(GLOB simulatable-include "include/*.h")
set_target_properties(libsimulatable PROPERTIES
  PUBLIC_HEADER
    siulatable-include
  )

#### Properties of our library


# Setup dependencies

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# using Visual Studio C++
	target_link_libraries(libsimulatable INTERFACE Eigen3::Eigen)
	target_link_libraries(libsimulatable PRIVATE Spectra)
else()
	find_package(OpenMP 4.0 REQUIRED COMPONENTS CXX)
	target_link_libraries(libsimulatable INTERFACE Eigen3::Eigen)
	target_link_libraries(libsimulatable PRIVATE OpenMP::OpenMP_CXX Spectra)
endif()

# After running "brew install libomp", I had to include the path for
# "omp.h" manually for unknown reason
if(APPLE)
  set(CMAKE_CXX_FLAGS "-I/usr/local/include")
endif(APPLE)



# Setup our include directory
# The INSTALL_INTERFACE is what is evaluated when consumers build
# linking to this target. Relative paths are allowed, and are evaluated
# as being relative to the install location.
# Otherwise the BUILD_INTERFACE is evaluated, and those always need
# to be absolute paths
target_include_directories(libsimulatable PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:"include/">
  )

# State to consumers that at least C++14 is required to use our library
target_compile_features(libsimulatable PUBLIC cxx_std_14)

# State that we need our library built with position independent code so
# that it can be consumed by a dynamic library (.so/.dylib/.dll)
set_target_properties(libsimulatable PROPERTIES POSITION_INDEPENDENT_CODE ON)

option(BUILD_PYTHON_BINDING "Build python binding with pybind11" OFF)
if(BUILD_PYTHON_BINDING)
  pybind11_add_module(pysimulatable python/Simulatable.cxx)
  set_target_properties(pysimulatable PROPERTIES
  PUBLIC_HEADER
    simulatable-include
  )
  target_link_libraries(pysimulatable PRIVATE Eigen3::Eigen Simulatable::libsimulatable)
  target_include_directories(pysimulatable PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:"include/">)
  target_compile_definitions(pysimulatable PRIVATE VERSION_INFO=${VERSION_INFO})
endif()

### Setup the project to be installable / exportable / importable

# CMake has the concept of an importable project. This means a project
# that can be found using `find_package` without a custom FindProject being
# written by the consumer. Instead the project to be found, exports itself.
#
# Exporting is primarily done by generating two files:
#   <project_name>-targets.cmake which contains all the targets consumers can
#   use
#   <project_name>-config.cmake which is the primary entry point for cmake
#   and contains any required `find_package` calls to use your project and
#   includes <project_name>-targets.cmake.

# Setup where our headers will be installed. We don't want to use the default
# of `include` as that could cause our headers to overwrite existing files
if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR "include/Simulatable")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Configure the config.cmake file
# processes the <project_name>-config.cmake template and place it in the build
# directory so it doesn't pollute our source directory
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/share/cmake/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "share/cmake/${PROJECT_NAME}"
  )

# Add an install rule for the static library we built
# `EXPORT` states that lib is part of the set of targets we want exported
# to <project_name>-targets.cmake.
install(TARGETS libsimulatable EXPORT ${PROJECT_NAME}-targets)

# Install all targets that are part of the export set in the
# <project_name>-targets.cmake under the namespace 'Simulatable'.
install(EXPORT ${PROJECT_NAME}-targets
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NAMESPACE Simulatable::
  FILE "${PROJECT_NAME}-targets.cmake"
  )

# Install the <project_name>-config.cmake file that we configured into
# the build directory
install(FILES
  "${PROJECT_BINARY_DIR}/share/cmake/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  )



### Add Tests

# Setup Catch2 for Tests
option(SIMULATABLE_BUILD_TESTING "Build testings with Catch2" ON)
if(SIMULATABLE_BUILD_TESTING)
  # Use a glob that re-runs everytime we build to get all the
  # tests sources
  file(GLOB test-sources CONFIGURE_DEPENDS tests/*.cxx)
  add_executable(${PROJECT_NAME}-tests "${test-sources}")

  # Mark the tests as needing Catch benchmark infrastructure which
  # off by default
  target_compile_definitions(${PROJECT_NAME}-tests
    PRIVATE
    CATCH_CONFIG_ENABLE_BENCHMARKING
    )

  #
  target_link_libraries(${PROJECT_NAME}-tests
    PRIVATE
      Catch2::Catch2
      Simulatable::libsimulatable
    )

  # catch_discover_tests will automatically convert each catch2
  # test case into a separate ctest entry
  include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)
  catch_discover_tests(${PROJECT_NAME}-tests)
endif()
