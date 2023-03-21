#include <iostream>
#include <vector>
#include <cmath>
#include <cilk/cilk.h>
#include "ctimer.h"

using namespace std;

// Variables used in the functions
int first, last;

/*
The following function splits up the input array.
@param n, p and the input array are being passed in.
@returns the vector which consits of subarrays.
*/
vector<vector<double>>mydividedarray(int n, int p, double* myarray) {
  int myarraysize = ceil((double)n/p);
  vector<vector<double>>myvector(p);

  cilk_for (int i = 0; i < p; i++) {
    first = i * myarraysize;
    last = i + 1 * myarraysize;
    myvector[i].resize(last - first);
    cilk_for (int j = first; j < last; j++) {
      myvector[i][j - first] = myarray[j];
    }
  }
  return myvector;
}

/*
Perform prefix sum on the subarrays by adding the last element
of the previous subarray to the first element of the current subarray.
@param subarray, and lastnumber are being passed in.
*/
void prefixsum(vector<double>&mysubarray, double lastnumber) {
  cilk_for (int i = 0; i < mysubarray.size(); i++) {
    mysubarray[i] = mysubarray[i] + lastnumber;
    lastnumber = lastnumber +  mysubarray[i];
  }
}

int main (int argc, char *argv[]) {
  double lastnumber = 0;
  long p = 10;
  long n = 5;

  if (argc > 1) {
    p = atol(argv[1]);
  }
  double arr[p];

  for (int i = 0; i < p; i++) {
    arr[i] = rand() % 200 - 100;
  }

  cout << "Prior Computation: " << endl;
  for (int j = 0; j < p; j++) {
    cout << arr[j] << " ";
  }
  cout << endl;

  ctimer_t t;
  ctimer_start(&t);
  vector<vector<double>> mysubarray = mydividedarray(n, p, arr);

  // Perform prefix sum on each subarray

  for (int i = 0; i < p; i++) {
    prefixsum(mysubarray[i], lastnumber);
    if (i < p - 1) {
      lastnumber = mysubarray[i][mysubarray[i].size() -1];
    }
  }

  ctimer_stop(&t);
  ctimer_measure(&t);

  // Combine sub-arrays into one output array
  vector<double> prefixsumvector;
  for (int i = 0; i < p; i++) {
    prefixsumvector.insert(prefixsumvector.end(), mysubarray[i].begin(), mysubarray[i].end());
  }

  // Print the prefix sum
  cout << "Prefix Sum: " << endl;
  for (int i = 0; i < prefixsumvector.size(); i++) {
    cout << prefixsumvector[i] << " ";
  }

  cout << endl;
  ctimer_print(t, "Prefix");
  // Verification if it is correct or not do one for if number of entries are greater than 10 as the results are too large to print out on console

  return 0;
}
