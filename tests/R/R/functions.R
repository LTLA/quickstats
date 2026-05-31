#' @importFrom Rcpp sourceCpp
#' @useDynLib quickstats.test
#' @export
pairwise.sum <- function(y, simple = FALSE) {
    pairwise_sum(y, simple = simple)
}
