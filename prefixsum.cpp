#include <iostream>
#include <cstdlib>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

using namespace std;

void prefix_sum(double* arr, int n, int p) {
    int block_size = n / p;
    double* block_sums = new double[p];
    cilk_for(int i = 0; i < p; i++) {
        double sum = 0;
        for (int j = i * block_size; j < (i + 1) * block_size; j++) {
            sum += arr[j];
            arr[j] = sum;
        }
        block_sums[i] = sum;
    }
    if (p > 1) {
        prefix_sum(block_sums, p, p / 2);
        cilk_for(int i = 1; i < p; i++) {
            int block_start = i * block_size;
            arr[block_start] += block_sums[i - 1];
        }
    }
    delete[] block_sums;
}

int main(int argc, char* argv[]) {
    int n = 1000000;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    double* arr = new double[n];
    double M = 100.0;
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = (double)rand() / RAND_MAX * 2 * M - M;
    }
    prefix_sum(arr, n, __cilkrts_get_nworkers());
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    delete[] arr;
    return 0;
}
