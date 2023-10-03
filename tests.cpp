#include <iostream>
#include <cstring>
#include <cnpy.h>
#include "solver.h"
#include "tests.h"

using namespace std;

struct test_01_res_struct test_01_res;

bool test_01() {
    // Pass counter
    int res = 6;

    // Test data
    cnpy::NpyArray A = cnpy::npy_load("test_data/A_01.npy");
    cnpy::NpyArray b = cnpy::npy_load("test_data/b_01.npy");
    cnpy::NpyArray b_norm2 = cnpy::npy_load("test_data/b_norm2_01.npy");
    cnpy::NpyArray bn_dot_prod = cnpy::npy_load("test_data/bn_dot_prod_01.npy");
    vector<double> n = {3, 2, 1};

    // Call to the routine under test
    StaticSource X;
    vector<double> loss(A.shape[0] * A.shape[1]);
    solve(A.shape, A.data<double>(), n.data(), STATIC_SOURCE_MIN_MSE, &X, loss.data());

    // Compare results with the reference ones
    cout << endl << "test_01_res.n_norm2 = " << test_01_res.n_norm2 <<
        ", expected = 14" << endl;
    if (test_01_res.n_norm2 == 14) res--;

    cout << endl << "A_01[0, 0, :, :] = " << endl;
    print_2d_slice(A.shape, A.data<double>(), {0, 0, -1, -1});
    cout << endl << "test_01_res.b[0, 0, :, :] = " << endl;
    print_2d_slice(A.shape, test_01_res.b.data(), {0, 0, -1, -1});
    cout << "expected = " << endl;
    print_2d_slice(b.shape, b.data<double>(), {0, 0, -1, -1});

    cout << endl << "A_01[0, 1, :, :] = " << endl;
    print_2d_slice(A.shape, A.data<double>(), {0, 1, -1, -1});
    cout << endl << "test_01_res.b[0, 1, :, :] = " << endl;
    print_2d_slice(A.shape, test_01_res.b.data(), {0, 1, -1, -1});
    cout << "expected = " << endl;
    print_2d_slice(b.shape, b.data<double>(), {0, 1, -1, -1});

    if (!memcmp(test_01_res.b.data(), b.data<double>(),
        b.shape[0] * b.shape[1] * b.shape[2] * b.shape[3] * sizeof(double))) res--;

    vector<size_t> bn_shape = {A.shape[0], A.shape[1], A.shape[3]};
    cout << endl << "test_01_res.b_norm2[0, :, :] = " << endl;
    print_2d_slice(bn_shape, test_01_res.b_norm2.data(), {0, -1, -1});
    cout << "expected = " << endl;
    print_2d_slice(bn_shape, b_norm2.data<double>(), {0, -1, -1});
    if (!memcmp(test_01_res.b_norm2.data(), b_norm2.data<double>(),
        bn_shape[0] * bn_shape[1] * bn_shape[2] * sizeof(double))) res--;

    cout << endl << "test_01_res.b_norm2[0, :, :] = " << endl;
    print_2d_slice(bn_shape, test_01_res.bn_dot_prod.data(), {0, -1, -1});
    cout << "expected = " << endl;
    print_2d_slice(bn_shape, bn_dot_prod.data<double>(), {0, -1, -1});
    if (!memcmp(test_01_res.bn_dot_prod.data(), bn_dot_prod.data<double>(),
        bn_shape[0] * bn_shape[1] * bn_shape[2] * sizeof(double))) res--;

    // Reshape the data to test parallel computing
    size_t tmp = A.shape[0];
    A.shape[0] = A.shape[1];
    A.shape[1] = tmp;
    tmp = b.shape[0];
    b.shape[0] = b.shape[1];
    b.shape[1] = tmp;
    fill(test_01_res.b.begin(), test_01_res.b.end(), -1);

    // Call to the routine under test
    solve(A.shape, A.data<double>(), n.data(), STATIC_SOURCE_MIN_MSE, &X, loss.data());

    // Compare results with the reference ones
    cout << endl << "test_01_res.n_norm2 = " << test_01_res.n_norm2 <<
        ", expected = 14" << endl;
    if (test_01_res.n_norm2 == 14) res--;

    cout << endl << "A_01_reshaped[0, 0, :, :] = " << endl;
    print_2d_slice(A.shape, A.data<double>(), {0, 0, -1, -1});
    cout << endl << "test_01_res.b[0, 0, :, :] = " << endl;
    print_2d_slice(A.shape, test_01_res.b.data(), {0, 0, -1, -1});
    cout << "expected = " << endl;
    print_2d_slice(b.shape, b.data<double>(), {0, 0, -1, -1});

    cout << endl << "A_01_reshaped[1, 0, :, :] = " << endl;
    print_2d_slice(A.shape, A.data<double>(), {1, 0, -1, -1});
    cout << endl << "test_01_res.b[1, 0, :, :] = " << endl;
    print_2d_slice(A.shape, test_01_res.b.data(), {1, 0, -1, -1});
    cout << "expected = " << endl;
    print_2d_slice(b.shape, b.data<double>(), {1, 0, -1, -1});

    if (!memcmp(test_01_res.b.data(), b.data<double>(),
        b.shape[0] * b.shape[1] * b.shape[2] * b.shape[3] * sizeof(double))) res--;

    // Result
    return !res;
}

int main() {
    int res = 1;

    cout << "===== START test_01()" << endl;
    if (test_01()) {
        cout << endl << "===== test_01() PASSED" << endl;
        res--;
    } else {
        cout << endl << "!!!!! test_01() FAILED" << endl;
    }

    if (!res) {
        cout << endl << "All tests PASSED" << endl;
    } else {
        cout << endl << "Some tests FAILED!!!" << endl;
    }

    return res;
}

