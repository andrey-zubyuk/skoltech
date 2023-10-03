#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cnpy.h>
#include <matplot/matplot.h>
#include "solver.h"

using namespace std;
using namespace matplot;

int main(int argc, char **argv) {
    // Verify arguments
    if (argc < 3) {
        cerr << "Usage:" << endl;
        cerr << "  " << argv[0] << " <path_to_tensor_A_npy> <path_to_tensor_n_csv>" << endl;
        return 1;
    }

    // Load the tensor A
    cnpy::NpyArray A = cnpy::npy_load(argv[1]);
    if (A.word_size != sizeof(double)) {
        cerr << "\"double\" is the only type supported.";
        return 1;
    }
    cout << "The tensor A loaded, shape: ";
    size_t A_len = 1;
    for (int i = 0; i < A.shape.size(); i++) {
        A_len *= A.shape[i];
        cout << A.shape[i];
        if (i < A.shape.size() - 1) cout << "x";
    }
    double *A_data = A.data<double>();
    double A_min, A_max;
    A_min = A_max = A_data[0];
    for (int i = 0; i < A_len; i++) {
        A_min = std::min(A_min, A_data[i]);
        A_max = std::max(A_max, A_data[i]);
    }
    cout << ", A.min(): " << A_min;
    cout << ", A.max(): " << A_max << "." << endl;

    // Load the tensor n
    vector<double> n(A.shape[2]);
    double n_min, n_max;
    ifstream fs(argv[2]);
    for (int i = 0; i < A.shape[2]; i++) {
        if (!(fs >> n[i])) {
            ostringstream ss;
            ss << __FILE__ << ":" << __LINE__ << ": No data to read";
            throw MyException(ss.str());
        }
        if (!i) n_min = n_max = n[0];
        else {
            n_min = std::min(n_min, n[i]);
            n_max = std::max(n_max, n[i]);
        }
    }
    cout << "The tensor n loaded, n.min(): " <<
        n_min << ", n.max(): " << n_max << "." << endl;

    // Solve the equation
    StaticSource X;
    vector<double> loss(A.shape[0] * A.shape[1]);
    solve(A.shape, A_data, n.data(), STATIC_SOURCE_MIN_MSE, &X, loss.data());

    // Print the results
    cout << "The source detected at x = " << X.x << ", y = " << X.y << "," << endl;
    cout << "    intensity = " << X.intensity << "," << endl;
    cout << "    emission start time = " << X.T << "." << endl;

    // Plot the results
    vector<size_t> loss_shape = {A.shape[0], A.shape[1]};
    vector<vector<double>> loss_plt(A.shape[1]);
    for (int y = 0; y < A.shape[1]; y++) {
        loss_plt[y].resize(A.shape[0]);
        for (int x = 0; x < A.shape[0]; x++) {
            loss_plt[y][x] = loss[ravel_multi_index(loss_shape, {x, y})];
        }
    }
    image(0, A.shape[0] - 1, 0, A.shape[1] - 1, loss_plt, true);
    colorbar();
    xlabel("x");
    ylabel("y");
    gca()->y_axis().reverse(false);
    hold(on);
    plot({(double)X.x}, {(double)X.y}, "or");
    textarrow(
        std::min(X.x + 0.1 * A.shape[0], 0.95 * A.shape[0]),
        std::min(X.y + 0.1 * A.shape[1], 0.95 * A.shape[1]),
        (double)X.x, (double)X.y, "detected source"
    );
    title("Residual sum of squares (RSS) and the detected source");
    show();

    // Exit
    return 0;
}

