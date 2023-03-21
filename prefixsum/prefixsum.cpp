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
@param n, val and the input array are being passed in.
@returns the vector which consits of subarrays.
*/
vector<vector<double>>mydividedarray(int n, int val, double* myarray) {
  /*
  Here we assign my arraysize a ceiling of n/p this to ensure we 
  assign it a whole number not a decimal as that is the number of subarrays. 
  */
  int myarraysize = ceil((double)n/val);
  vector<vector<double>>myvector(val);

  cilk_for (int i = 0; i < val; i++) {
    first = i * myarraysize;
    last = i + 1 * myarraysize;
    myvector[i].resize(last - first);
    cilk_for (int j = first; j < last; j++) {
      myvector[i][j - first] = myarray[j];
    }
  }
  /*
  @returns the vector which consits of subarrays.
  */
  return myvector;
}

/*
Performs prefix sum on the subarrays by adding the last element
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
  long val = 10;
  long n = 5;

  if (argc > 1) {
    val = atol(argv[1]);
  }
  double arr[val];
  //double testarray[val];

  for (int i = 0; i < val; i++) {
    arr[i] = rand() % 200 - 100;
  }
  /* 
  Here we print out the randomly generated array prior to the prefix sum being computed
  this is simply so the user is able to see the randomly generated list before and after being computed.
  */
  cout << "Prior Computation: " << endl;
  for (int j = 0; j < val; j++) {
    cout << arr[j] << " ";
  }
  cout << endl;
  /*
  Here we are using the ctimer.h library and starting our timer prior to calling the function 
  which is responsible for dividing the array. Also the function which computes the prefix sum of the subarrays.
  */
  ctimer_t t;
  ctimer_start(&t);
  vector<vector<double>> mysubarray = mydividedarray(n, val, arr);
  /*
   Here we are performing the prefix sum on every subarray.
  */
  for (int i = 0; i < val; i++) {
    prefixsum(mysubarray[i], lastnumber);
    if (i < val - 1) {
      lastnumber = mysubarray[i][mysubarray[i].size() -1];
    }
  }
  /*
  The timer stops after we collected our measurments and have called the functions this is so 
  we can get a accurate result.
  */
  ctimer_stop(&t);
  ctimer_measure(&t);
  /*
  Here we create a prefixsum vector than we combine subarrays and store them into the prefixsumvector 
  this is used for the purpose of outputting the results back to the console.
  */
  vector<double> prefixsumvector;
  for (int i = 0; i < val; i++) {
    prefixsumvector.insert(prefixsumvector.end(), mysubarray[i].begin(), mysubarray[i].end());
  }
  /*
  Here we print the results of the computation to the console back to the user.
  */
  cout << "Prefix Sum: " << endl;
  for (int i = 0; i < prefixsumvector.size(); i++) {
    cout << prefixsumvector[i] << " ";
  }

  cout << endl;
  ctimer_print(t, "Prefix");

  /*
  Verifiying sequentially the code was tested to ensure the prefix sum is computed 
  correctly this part was taken out after computing measurements.
  */
  /*
  testarray[0] = arr[0];
  for (int i = 1; i < val; i++) {
    testarray[i] = testarray[i - 1] + arr[i];
  }
  for (int j = 0; j < val; j++) {
    if (testarray[j] != prefixsumvector[j]) {
      cout << "The prefix is correct" << endl;
    }
    else {
      cout << "The prefix is wrong" << endl;
    }
  }
  */

  return 0;
}
