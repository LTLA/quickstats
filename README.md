# Quickly compute simple statistics

![Unit tests](https://github.com/LTLA/quickstats/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/quickstats/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/LTLA/quickstats/branch/master/graph/badge.svg?token=U5W91W4X5W)](https://codecov.io/gh/ltla/quickstats)

## Overview

Pretty much as it says in the title.
This library implements some utilities to compute simple statistics like quantiles and variances. 
Nothing too exotic or difficult, just annoying to re-implement for each application.
Most of this code was migrated from [**tatami_stats**](https://github.com/tatami-inc/tatami_stats) or [**libscran**](https://github.com/libscran) for more general use.

Check out the [reference documentation](https://ltla.github.io/quickstats) for more details.

## Medians

Pretty much as it says - works for dense and sparse vectors:

```cpp
std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
auto med = quickstats::median<double>(x.size(), x.data());

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> x_nonzero{ 1., 2., 3. };
auto med_sparse = quickstats::median<double>(10, 3, x.data());
```

## Single quantiles

The `SingleQuantileFixedNumber` class will compute a pre-specified quantile from a fixed number of elements:

```cpp
quickstats::SimpleQuantileFixedNumber<double, int> qfixed(10, 0.2);

std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
auto fq20 = qfixed(x.data());

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> x_nonzero{ 1., 2., 3. };
auto fq20_sparse = qfixed(3, x_nonzero.data());
```

The `SingleQuantileVariableNumber` class will compute a pre-specified quantile from a variable number of elements, up to a maximum: 

```cpp
quickstats::SimpleQuantileVariableNumber<double, int> qvar(10, 0.2);

std::vector<double> y{ 1., 2., 3., 4., 5., 6. };
auto vq20 = qvar(y.size(), y.data());

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0]
std::vector<double> y_nonzero{ 1., 2., 3. };
auto vq20_sparse = qvar(6, 3, y_nonzero.data());
```

## Multiple quantiles

To compute multiple quantiles at once, we can construct a `MultipleQuantilesFixedNumber` instance.
This is more efficient than creating and invoking multiple `SingleQuantileFixedNumber` instances.

```cpp
std::vector<double> quantiles{ 0.1, 0.2, 0.4, 0.8 }; // probabilities should be sorted.
quickstats::SimpleQuantileFixedNumber<double, int> qfixed(10, quantiles);

std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
std::vector<double> res(quantiles.size());
qfixed(x.data(), [&](int q, double val) -> void {
    res[q] = val;
});

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> x_nonzero{ 1., 2., 3. };
qfixed(3, x_nonzero.data(), [&](int q, double val) -> void {
    res[q] = val;
});
```

Similarly, the `MultipleQuantilesVariableNumber` class computes multiple quantiles from a variable number of elements, up to a maximum: 

```cpp
quickstats::MultipleQuantilesVariableNumber<double, int> qvar(10, &quantiles);

std::vector<double> y{ 1., 2., 3., 4., 5., 6. };
qvar(y.size(), y.data(), [&](int q, double val) -> void {
    res[q] = val;
});

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0]
std::vector<double> y_nonzero{ 1., 2., 3. };
qvar(6, 3, y_nonzero.data(), [&](int q, double val) -> void {
    res[q] = val;
});
```

## Building projects 

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  quickstats
  GIT_REPOSITORY https://github.com/LTLA/quickstats
  GIT_TAG master # replace with a pinned version
)

FetchContent_MakeAvailable(quickstats)
```

Then you can link to **quickstats** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe quickstats)

# For libaries
target_link_libraries(mylib INTERFACE quickstats)
```

### CMake with `find_package()`

You can install the library by cloning a suitable version of this repository and running the following commands:

```sh
mkdir build && cd build
cmake .. -DQUICKSTATS_TESTS=OFF
cmake --build . --target install
```

Then you can use `find_package()` as usual:

```cmake
find_package(ltla_quickstats CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE ltla::quickstats)
```

### Manual

If you're not using CMake, the simple approach is to just copy the files in the `include/` subdirectory - 
either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
