#include <iostream>
#include <vector>
#include <cmath>
#include <cilk/cilk.h>
#include "ctimer.h"

using namespace std;

// Variables used in the functions
int first, last;

// Split input array into p chunks and return vector of sub-arrays
vector<vector<double>>mydividedarray(int n, int p, double* myarray) {
  int myarraysize = ceil((double)n/p);
  std::vector<std::vector<double>>myvector(p);
  // Here we split up the array and return the vector of subarrays
  cilk_scope {
     for (int i = 0; i < p; i++) {
        first = i * myarraysize;
        last = i + 1 * myarraysize;
        myvector[i].resize(last - first);
        for (int j = first; j < last; j++) {
            myvector[i][j - first] = myarray[j];
        }
    }
  }
  return myvector;
}

// Perform prefix sum on a subarray by adding the last element of the previous subarray to the first element of the current subarray
void prefix_sum(vector<double>& sub_array, double lastnumber) {
  cilk_scope {
    for (int i = 0; i < sub_array.size(); i++) {
        sub_array[i] = sub_array[i] + lastnumber;
        lastnumber = lastnumber +  sub_array[i];
    }
  }
}

int main (int argc, char *argv[]) {
    //long n = pow(10, 6);
    //if (argc > 1) {
      //  n = atol(argv[1]);
   // }


  double arr[] = {1,2,3,4,5,6,7,8,9,10};
    

  double lastnumber = 0;
  int p = 5;
  int n = 10;
  
  ctimer_t t;
  ctimer_start(&t);

  vector<vector<double>> sub_arrays = mydividedarray(n, p, arr);

  // Perform prefix sum on each subarray
     
    for (int i = 0; i < p; ++i) {
        prefix_sum(sub_arrays[i], lastnumber);
        if (i < p - 1) {
        lastnumber = sub_arrays[i][sub_arrays[i].size() -1];
        }
    }

    ctimer_stop(&t);
    ctimer_measure(&t);

  // Combine sub-arrays into one output array
  std::vector<double> output_array;
  for (int i = 0; i < p; ++i) {
    output_array.insert(output_array.end(), sub_arrays[i].begin(), sub_arrays[i].end());
  }

  // Print output array
  std::cout << "Output array: ";
  for (int i = 0; i < output_array.size(); ++i) {
    std::cout << output_array[i] << " ";
  }
     ctimer_print(t, "Prefix");

  return 0;
}
// SO we have taken a array, than split it into many subarrays, after than we have computed the prefix sum also we return to the user the final prefix sum
// we just neeed to see what robert is looking for in terms of the output.