#include <iostream>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <chrono>
#include <cstdlib> // test this one !

using namespace std;

void prefix_sum(double *array, int n) {
    if (n == 1) {
        return;
    }
    
    int p = __cilkrts_get_nworkers();
    int chunk_size = n / p;
    
    // Step 1: Divide the array into p subarrays of equal size such as n/p
    cilk_for (int i = 0; i < p; i++) {
        int start = i * chunk_size;
        int end = (i + 1) * chunk_size;
        
        if (i == p - 1) {
            end = n;
        }
        
        prefix_sum(array + start, end - start);
        
        // Step 2: Compute prefix sum of each subarray separately using p processors in parallel
        cilk_for (int j = start + 1; j < end; j++) {
            array[j] += array[j - 1];
        }
    }
    
    // Step 3: Add the last element of previous subarray to the first element of the current subarray
    cilk_for (int i = 1; i < p; i++) {
        int prev_end = i * chunk_size - 1;
        array[i * chunk_size] += array[prev_end];
    }
    
    // Recursively apply steps 1-3
    prefix_sum(array, chunk_size);
    
    // Step 4: Binary tree reduction to compute final prefix sum
    cilk_for (int i = 1; i < p; i++) {
        int start = i * chunk_size;
        int end = (i + 1) * chunk_size;
        
        if (i == p - 1) {
            end = n;
        }
        
        double sum = array[start - 1];
        cilk_for (int j = start; j < end; j++) {
            array[j] += sum;
        }
    }
}

int main(int argc, char **argv) {
    int n = 1000000;
    double m = 10.0;
    
    if (argc > 1) {
        n = atoi(argv[1]);
    }

    // Get the array values from the user
    double *array = new double[n];
    for (int i = 0; i < n; i++) {
        cout << "Enter value " << i + 1 << ": ";
        cin >> array[i];
    }
    
    auto start_time = chrono::steady_clock::now();
    prefix_sum(array, n);
    auto end_time = chrono::steady_clock::now();
    
    cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << "ms\n";
    
    // Print prefix sum array
    for (int i = 0; i < n; i++) {
        cout << array[i] << " ";
    }
    cout << endl;
    
    delete[] array;
    
    return 0;
}
