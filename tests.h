#ifndef __TESTS_H__
#define __TESTS_H__

#include <vector>

extern struct test_01_res_struct {
    double n_norm2;
    std::vector<double> b;
    std::vector<double> b_norm2, bn_dot_prod;
} test_01_res;

#endif

