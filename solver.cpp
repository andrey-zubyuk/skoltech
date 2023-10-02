#include "solver.h"

using namespace std;

// Specific solver for the STATIC_SOURCE_MIN_MSE case and its helper functions

void solve_cumsum_at_xy(
    const vector<size_t> &shape, const double *A, int x, int y, double *B
) {
    for (int td = 0; td < shape[2]; td++) {
        /*
         * Pay attention at "T <= td", the alternative is "T < shape[3]".
         *
         * Ideally, any matrix A[x,y,:,:] has to be triangular because the
         * future may not have an effect in the past. However, for some
         * reason it is not the case in the example of A provided.
         * In the algorithm below such artifacts of A are not taken into
         * account (are eliminated).
         *
         * The same considerations are valid for "ts <= td" below.
         */
        for (int T = 0; T <= td /* < shape[3] */; T++) {
            int B_flat_idx = ravel_multi_index(shape, {x, y, td, T});
            for (int ts = T; ts <= td /* < shape[3] */; ts++) {
                int A_flat_idx = ravel_multi_index(shape, {x, y, td, ts});
                B[B_flat_idx] += A[A_flat_idx];
            }
        }
    }
}

void solve_mse_at_xyT(
    const vector<size_t> &shape, const double *B, const double *n,
    int x, int y, int T, double *dev, double tol = 1e-12
) {
    double b_norm2 = 0, n_norm2 = 0, bn_dot_prod = 0;
    for (int td = 0; td < shape[2]; td++) {
        int idx = ravel_multi_index(shape, {x, y, td, T});
        b_norm2 += B[idx] * B[idx];
        n_norm2 += n[td] * n[td];
        bn_dot_prod += B[idx] * n[td];
    }
    vector<size_t> dev_shape = {shape[0], shape[1], shape[3], 2};
    dev[ravel_multi_index(dev_shape, {x, y, T, 0})] = n_norm2 - bn_dot_prod * bn_dot_prod / (b_norm2 + tol);
    dev[ravel_multi_index(dev_shape, {x, y, T, 1})] = bn_dot_prod / (b_norm2 + tol);
}

void solve_static_source_min_mse(
    const vector<size_t> &shape, const double *A, const double *n, StaticSource *X, double *dev
) {
    if (shape.size() != 4) {
        ostringstream ss;
        ss << __FILE__ << ":" << __LINE__ << ": Incorrect shape";
        throw MyException(ss.str());
    }

    int len = 1;
    for (int i = 0; i < shape.size(); i++) len *= shape[i];
    vector<double> B(len, 0);
    unique_ptr<double[]> dev_sp;
    if (!dev) {
        // two output channels: squared error, source intensity
        dev_sp = make_unique<double[]>(len / shape[2] * 2);
        dev = dev_sp.get();
    }
    X->x = X->y = X->T = 0;
    int X_dev_idx = 0;
    vector<size_t> dev_shape = {shape[0], shape[1], shape[3], 2};
    for (int x = 0; x < shape[0]; x++) {
        for (int y = 0; y < shape[1]; y++) {
            solve_cumsum_at_xy(shape, A, x, y, B.data());
            for (int T = 0; T < shape[3]; T++) {
                solve_mse_at_xyT(shape, B.data(), n, x, y, T, dev);
                if (dev[ravel_multi_index(dev_shape, {x, y, T, 0})] <= dev[X_dev_idx]) {
                    X->intensity = dev[ravel_multi_index(dev_shape, {x, y, T, 1})];
                    X->x = x;
                    X->y = y;
                    X->T = T;
                    X_dev_idx = ravel_multi_index(dev_shape, {x, y, T, 0});
                }
            }
        }
    }
}

// Solver interface
void solve(
    const vector<size_t> &shape, const double *A, const double *n, int method,
    void *X, void *extra
) {
    switch (method) {
        case STATIC_SOURCE_MIN_MSE:
            solve_static_source_min_mse(shape, A, n,
                static_cast<StaticSource *>(X), static_cast<double *>(extra));
            return;
        default:
            ostringstream ss;
            ss << __FILE__ << ":" << __LINE__ << ": method = " << method << " is not supported";
            throw MyException(ss.str());
    }
}

