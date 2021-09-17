
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)


FetchContent_Declare(
  Eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
set(EIGEN_BUILD_DOC OFF)
# note: To disable eigen tests,
# you should put this code in a add_subdirectory to avoid to change
# BUILD_TESTING for your own project too since variables are directory
# scoped
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_PKGCONFIG OFF)
set( OFF)

# Get Catch2
FetchContent_Declare(Catch2 URL
https://github.com/catchorg/Catch2/archive/v2.13.7.zip)
  # Use FetchContent to to download and make Catch2 part of our CMake build.
  set(CATCH_INSTALL_HELPERS OFF)
  set(CATCH_BUILD_EXAMPLES OFF)
  set(CATCH_BUILD_EXTRA_TESTS OFF)
  set(CATCH_BUILD_TESTING OFF)
  set(CATCH_INSTALL_DOCS OFF)

# Get Spectra
FetchContent_Declare(Spectra
GIT_REPOSITORY https://github.com/yixuan/spectra.git
GIT_TAG 148d0190ff25c14fb26087250c10a8ba8975d85f # 1.0.0
)

# Get pybind11
FetchContent_Declare(pybind11
GIT_REPOSITORY https://github.com/pybind/pybind11.git
GIT_TAG 787d2c88cafa4d07fb38c9519c485a86323cfcf4 # 2.7.1
)

FetchContent_MakeAvailable(Eigen Spectra Catch2 pybind11)

