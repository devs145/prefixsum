#include <iostream>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <ctime>
#include <cstdlib>
#include <chrono>

using namespace std;

void parallel_prefix_sum(double* arr, int n, int p) {
    // Step 1: Divide the array into p sub-arrays of roughly equal size.
    int chunk_size = n / p;
    double* partial_sums = new double[p];
    cilk_for(int i = 0; i < p; i++) {
        int start = i * chunk_size;
        int end = (i == p-1) ? n : (i+1) * chunk_size;
        for (int j = start+1; j < end; j++) {
            arr[j] += arr[j-1];
        }
        partial_sums[i] = arr[end-1];
    }
    // Step 2: Perform a binary tree reduction on the last element of each sub-array.
    for (int s = p/2; s > 0; s /= 2) {
        cilk_for(int i = 0; i < s; i++) {
            int index = (i+1) * 2 * chunk_size - 1;
            if (index < n) {
                arr[index] += arr[index - chunk_size];
            }
        }
    }
    // Step 3: Recursively apply the same algorithm to the sequence of partial sums until we have a single element representing the prefix sum of the entire array.
    if (p > 1) {
        parallel_prefix_sum(partial_sums, p, p/2);
    }
    // Step 4: Add the prefix sum of the last sub-array to each element of the previous sub-arrays.
    cilk_for(int i = 1; i < p; i++) {
        int start = i * chunk_size;
        int end = (i == p-1) ? n : (i+1) * chunk_size;
        double prefix_sum = partial_sums[i-1];
        for (int j = start; j < end; j++) {
            arr[j] += prefix_sum;
        }
    }
    delete[] partial_sums;
}

bool verify_prefix_sum(double* arr, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
        if (i > 0 && arr[i] != sum) {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 1000000;
    int p = __cilkrts_get_nworkers();
    double M = 1000.0;
    double* arr = new double[n];
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = (double) rand() / RAND_MAX * 2 * M - M;
    }
    auto start = chrono::high_resolution_clock::now();
    parallel_prefix_sum(arr, n, p);
    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Parallel prefix sum computation took " << duration << " ms." << endl;
    if (verify_prefix_sum(arr, n)) {
        cout << "Prefix sum array is correct." << endl;
    } else {
        cout << "Prefix sum array is incorrect." << endl;
    }
    return 0;
}
