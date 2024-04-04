cmake_minimum_required(VERSION 3.25)
project(tgp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest
  GIT_TAG v2.4.11)

FetchContent_Declare(
  nanobench
  GIT_REPOSITORY https://github.com/martinus/nanobench
  GIT_TAG v4.3.11)

FetchContent_MakeAvailable(doctest nanobench)

add_executable(tgp-bench src/main.cc)
target_link_libraries(tgp-bench PRIVATE doctest nanobench)
target_compile_options(
  tgp-bench PRIVATE $<$<CONFIG:Debug,RelWithDebInfo>:-fsanitize=address
                    -fsanitize=undefined -fno-omit-frame-pointer>)
target_link_options(
  tgp-bench PRIVATE $<$<CONFIG:Debug,RelWithDebInfo>:-fsanitize=address
  -fsanitize=undefined>)
