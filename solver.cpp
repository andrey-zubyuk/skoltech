#include <cmath>
#include <thread>
#include <mutex>
#include "solver.h"

using namespace std;

// Specific solver for the STATIC_SOURCE_MIN_MSE case and its helper functions

tuple<int, double, double> solve_static_source_min_mse_at_xy(
    const vector<size_t> &shape, const double *A, int x, int y,
    const double *n, double n_norm2, double tol = 1e-12
) {
    int T_opt = 0;
    double loss_opt = INFINITY, intensity_opt = 0;
    for (int T = 0; T < shape[3]; T++) {
        /*
         * Pay attention at "td = T", the alternative is "td = 0".
         *
         * Ideally, any matrix A[x,y,:,:] has to be triangular because the
         * future may not have an effect in the past. However, for some
         * reason it is not the case in the example of A provided.
         * In the algorithm below such artifacts of A are not taken into
         * account (are eliminated).
         *
         * Similar considerations are valid for "ts <= td" below.
         */
        double b_norm2 = 0, bn_dot_prod = 0;
        for (int td = T; td < shape[2]; td++) {
            double b_td = 0;
            for (int ts = T; ts <= td /* < shape[3] */; ts++) {
                int flat_idx = ravel_multi_index(shape, {x, y, td, ts});
                b_td += A[flat_idx];
            }
            b_norm2 += b_td * b_td;
            bn_dot_prod += b_td * n[td];
        }

        double loss = n_norm2 - bn_dot_prod * bn_dot_prod / (b_norm2 + tol);
        if (loss < loss_opt) {
            loss_opt = loss;
            intensity_opt = bn_dot_prod / (b_norm2 + tol);
        }
    }
    return make_tuple(T_opt, intensity_opt, loss_opt);
}

static mutex solve_static_source_min_mse_at_x_mutex;

void solve_static_source_min_mse_at_x(
    const vector<size_t> &shape, const double *A, int x,
    const double *n, double n_norm2, volatile double *loss_opt, StaticSource *X, double *loss
) {
    vector<size_t> loss_shape = {shape[0], shape[1]};
    for (int y = 0; y < shape[1]; y++) {
        auto res = solve_static_source_min_mse_at_xy(shape, A, x, y, n, n_norm2);
        solve_static_source_min_mse_at_x_mutex.lock();
        if (get<2>(res) < *loss_opt) {
            *loss_opt = get<2>(res);
            X->T = get<0>(res);
            X->intensity = get<1>(res);
            X->x = x;
            X->y = y;
        }
        solve_static_source_min_mse_at_x_mutex.unlock();
        if (loss) {
            loss[ravel_multi_index(loss_shape, {x, y})] = get<2>(res);
        }
    }
}

void solve_static_source_min_mse(
    const vector<size_t> &shape, const double *A, const double *n, StaticSource *X, double *loss
) {
    // Is data correct?
    if (shape.size() != 4) {
        ostringstream ss;
        ss << __FILE__ << ":" << __LINE__ << ": Incorrect shape";
        throw MyException(ss.str());
    }

    // Precompute ||n||^2 which does not depend on other arguments
    double n_norm2 = 0;
    for (int td = 0; td < shape[2]; td++) {
        n_norm2 += n[td] * n[td];
    }

    // Main computations
    vector<thread> threads(shape[0]);
    X->x = X->y = X->T = 0;
    volatile double loss_opt = INFINITY;
    for (int x = 0; x < shape[0]; x++) {
        threads[x] = thread(
            solve_static_source_min_mse_at_x,
            shape, A, x, n, n_norm2, &loss_opt, X, loss
        );
    }
    for (int x = 0; x < shape[0]; x++) {
        threads[x].join();
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

