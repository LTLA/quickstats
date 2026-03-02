include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/73a63ea05dc8ca29ec1d2c1d66481dd0de1950f1.zip
)

# Avoid installing GoogleTest when installing this project.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(
    libtest
    src/SingleQuantile.cpp
)

target_link_libraries(
    libtest
    quickstats
    sanisizer
    gtest_main
)

target_compile_options(libtest PRIVATE -Wall -Wextra -Wpedantic -Werror)

option(CODE_COVERAGE "Enable coverage testing" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(libtest PRIVATE -O0 -g --coverage)
    target_link_options(libtest PRIVATE --coverage)
endif()

gtest_discover_tests(libtest)
