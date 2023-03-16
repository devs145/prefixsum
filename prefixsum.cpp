#include <iostream>
#include <cmath> 
#include <cstdio>

using namespace std;

#include <random>

double* generateRandomArray(int N, double M) {
    // set up random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-M, M);

    // generate the array
    double* array = new double[N];
    for (int i = 0; i < N; ++i) {
        array[i] = dis(gen);
    }

    return array;
}


int main() {
    // generate the array with default values
    double* array = generateRandomArray(1000000, 100.0);

    // print the array
    for (int i = 0; i < 1000000; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;

    // free the memory
    delete[] array;

    return 0;
}
