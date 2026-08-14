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
auto med = quickstats::median(x.size(), x.data(), {});

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> x_nonzero{ 1., 2., 3. };
auto med_sparse = quickstats::median(10, 3, x.data(), {});
```

Note that this will shuffle the input array.

## Single quantiles

The `SingleQuantileFixedNumber` class will compute a pre-specified quantile from a fixed number of elements:

```cpp
// We compute the 20th percentile from any array of 10 elements.
quickstats::SingleQuantileFixedNumber<double> qfixed(10, 0.2);

std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
auto fq20 = qfixed(x.data());

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> x_nonzero{ 1., 2., 3. };
auto fq20_sparse = qfixed(x_nonzero.size(), x_nonzero.data());
```

The `SingleQuantileVariableNumber` class will compute a pre-specified quantile from a variable number of elements, up to a maximum: 

```cpp
// We compute the 20th percentile from any array of <= 10 elements.
quickstats::SingleQuantileVariableNumber<double> qvar(10, 0.2, {});

std::vector<double> y{ 1., 2., 3., 4., 5., 6. };
auto vq20 = qvar(y.size(), y.data());

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0]
std::vector<double> y_nonzero{ 1., 2., 3. };
auto vq20_sparse = qvar(6, y_nonzero.size(), y_nonzero.data());
```

## Multiple quantiles

To compute multiple quantiles at once, we can construct a `MultipleQuantilesFixedNumber` instance.
This is more efficient than creating and invoking multiple `SingleQuantileFixedNumber` instances.

```cpp
// We want to compute multiple quantiles from an array of 10 elements.
std::vector<double> quantiles{ 0.1, 0.2, 0.4, 0.8 }; // probabilities should be sorted.
quickstats::MultipleQuantilesFixedNumber<double> qfixed(10, quantiles);

std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
std::vector<double> res(quantiles.size());
qfixed(
    x.data(),
    // Custom storing function.
    [&](std::size_t q, double val) -> void {
        res[q] = val;
    }
);

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> x_nonzero{ 1., 2., 3. };
qfixed(
    x_nonzero.size(),
    x_nonzero.data(),
    [&](int q, double val) -> void {
        res[q] = val;
    }
);
```

Similarly, the `MultipleQuantilesVariableNumber` class computes multiple quantiles from a variable number of elements, up to a maximum: 

```cpp
// We want to compute multiple quantiles from an array of 10 elements.
quickstats::MultipleQuantilesVariableNumber<double, decltype(&quantiles)> qvar(10, &quantiles, {});

std::vector<double> y{ 1., 2., 3., 4., 5., 6. };
qvar(
    y.size(),
    y.data(),
    [&](int q, double val) -> void {
        res[q] = val;
    }
);

// Also works for sparse data - in this case, [1, 2, 3, 0, 0, 0]
std::vector<double> y_nonzero{ 1., 2., 3. };
qvar(
    6,
    y_nonzero.size(),
    y_nonzero.data(),
    [&](int q, double val) -> void {
        res[q] = val;
    }
);
```

## Pairwise summation

We can perform pairwise summation to, well, compute the sum of an array.
This mitigates loss of precision due to floating-point round-off error compared to naive `std::accumulate()`.

```cpp
// No precision is lost in this simple example, but it's just a demonstration.
std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
quickstats::PairwiseSumWorkspace<double> work;
auto sum = quickstats::pairwise_sum(x.size(), x.data(), work, {});
```

We can also customize the source of the input data to perform accumulations over an abstract array:

```cpp
// No precision is lost in this simple example, but it's just a demonstration.
auto mean = sum / x.size();
auto rss = quickstats::pairwise_sum_abstract(
    x.size(),
    [&](const std::size_t idx) -> auto {
        const double delta = x[idx] - mean;
        return delta * delta;
    }
    work,
    {}
);
```

## Residual sum of squares

The `rss()` function will compute both the mean and the residual sum of squares (RSS) of an array:

```cpp
std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
quickstats::RssWorkspace<double> work;
auto rssout = quickstats::rss(x.size(), x.data(), work, {});
rssout.mean;
rssout.rss;

// Sparse vectors, e.g., [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]
std::vector<double> y{ 1., 2., 3. };
auto sprssout = quickstats::rss(10, y.size(), y.data(), work, {});
sprssout.mean;
sprssout.rss;
```

In some situations, we might want to update the RSS as new values become available.
This can be done using the `update_rss()` function:

```cpp
double curmean = 0, currss = 0;
for (std::size_t i = 0; i < 5; ++i) {
    quickstats::update_rss(curmean, currss, x[i], i + 1);
}
// Do something with the mean/RSS of the first 5 elements.

for (std::size_t i = 5; i < 10; ++i) {
    quickstats::update_rss(curmean, currss, x[i], i + 1);
}
// Do something with the mean/RSS of the full 'x' array.

// Works with sparse vectors as well.
quickstats::update_rss_with_zeros(curmean, currss, 5, x.size() + 5);
```

We can trivially convert each RSS into a variance estimate by dividing each variance with the number of observations (minus 1, for the sample variance).
Note, some extra care may be required to handle zero denominators at low numbers of observations.

We can also use `recenter_rss()` to adjust the RSS for a different value of the mean.
This is typically used to combine RSS values from multiple subarrays into a single RSS value for the entire array.

## Median absolute deviation

Given the median, the `mad()` function will compute the median absolute deviation.
Note that this will replace the input array with the absolute deviations.

```cpp
std::vector<double> x{ 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
auto med = quickstats::median(x.size(), x.data(), {});
auto mad = quickstats::mad(x.size(), x.data(), med, {});
```

For infinite medians, we can optionally define the difference between infinities of the same sign as zero.
This is occasionally helpful for providing a sane result when infinities are generated by mathematical transformations, e.g., `log(0)`.

```cpp
constexpr auto inf = std::numeric_limits<double>::infinity();
std::vector<double> x{ inf, 0, inf, inf, 0, inf, 0, 2, inf, inf };
auto med = quickstats::median(x.size(), x.data(), {});

quickstats::MadOptions<double> opt;
opt.difference_between_infinities_is_zero = true;
auto mad = quickstats::mad(x.size(), x.data(), med, opt);
```

As usual, the same functions are also available for sparse vectors.

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
