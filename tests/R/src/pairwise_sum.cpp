#include "Rcpp.h"

#include <algorithm>

#include "quickstats/quickstats.hpp"

//[[Rcpp::export(rng=false)]]
double pairwise_sum(Rcpp::NumericVector input, bool simple) {
    const auto n = input.size();
    const auto ptr = static_cast<const double*>(input.begin());
    if (simple) {
        return std::accumulate(ptr, ptr + n, 0.0);
    } else {
        quickstats::PairwiseSumWorkspace<double> wrk;
        return quickstats::pairwise_sum(n, ptr, wrk);
    }
}
