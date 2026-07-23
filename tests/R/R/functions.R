#' @importFrom Rcpp sourceCpp
#' @useDynLib quickstats.test
#' @export
pairwise.sum <- function(y, simple = FALSE, accumulators = TRUE) {
    pairwise_sum(y, simple = simple, accumulators = accumulators)
}
