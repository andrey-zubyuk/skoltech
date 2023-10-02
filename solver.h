#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include "cnpy.h"

// Utilities

class MyException : public std::exception {
    protected:
        std::string descr;
    public:
        MyException(std::string descr) {
            this->descr = descr;
        }
        const char* what() const noexcept {
            return descr.c_str();
        }
};

template<class T> inline int ravel_multi_index(
    const std::vector<T> &shape, const std::vector<int> &multi_index
) {
    int idx = multi_index[0];
    for (int i = 1; i < multi_index.size(); i++) {
        idx = idx * shape[i] + multi_index[i];
    }
    return idx;
}

template<class T> void print_2d_slice(
    const std::vector<T> &shape, double *data, const std::vector<int> &multi_idx
) {
    std::vector<int> idx = multi_idx;
    auto dim1_it = std::find(multi_idx.begin(), multi_idx.end(), -1);
    int dim1 = dim1_it - multi_idx.begin();
    auto dim2_it = std::find(dim1_it + 1, multi_idx.end(), -1);
    int dim2 = dim2_it - multi_idx.begin();
    if (dim1 >= shape.size() || dim2 >= shape.size()) {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __LINE__ << ": No dimensions specified to get a 2d slice";
        throw MyException(ss.str());
    }
    for (idx[dim1] = 0; idx[dim1] < shape[dim1]; idx[dim1]++) {
        std::cout << "[ ";
        for (idx[dim2] = 0; idx[dim2] < shape[dim2]; idx[dim2]++) {
            int flat_idx = ravel_multi_index(shape, idx);
            std::cout << std::setw(7) << data[flat_idx] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

// Solver methods available
enum {
    /*
     * The case of a point constant-intensity source localized in the point
     * (x_s, y_s):
     *     X_{x,y,t}       = 0,     x != x_s, y != y_s, any t,
     *     X_{x_s, y_s, t} = 0,     t < T,
     *     X_{x_s, y_s, t} = const, t >= T,
     * where T is a source emission start time. Solver criteria is MSE between
     * the left-hand and right-hand sides of the equation
     *     A_{x,y,t_d,t_s} X_{x,y,t_s} = n_{t_d}.
     */
    STATIC_SOURCE_MIN_MSE
};

// Specific resulting structure for the case of a static point source
struct StaticSource {
    int x, y, T;
    double intensity;
};

// Solver interface
void solve(
    const std::vector<size_t> &shape, const double *A, const double *n, int method,
    void *X, void *extra
);

#endif

