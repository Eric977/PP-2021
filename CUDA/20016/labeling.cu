#include "labeling.h"
#include <thrust/tabulate.h>
#include <thrust/scan.h>
#include <thrust/functional.h>
#include <thrust/execution_policy.h>
 
 
struct is_char
{
    const char* s;
 
    is_char(const char* s) : s(s) {}
 
    __host__ __device__
    int operator()(const int i) const {
                return s[i] == ' ' ? i : -1;
    }
};
 
struct compute{
        const int* arr;
        compute(const int *arr): arr(arr) {};
        __host__ __device__
        int operator()(const int i) const{
                return i - arr[i];
        }
};
 
void labeling(const char *cuStr, int *cuPos, int n){
        thrust::tabulate(thrust::device, cuPos, cuPos + n,  is_char(cuStr));
        thrust::inclusive_scan(thrust::device, cuPos, cuPos + n, cuPos, thrust::maximum<int>());
        thrust::tabulate(thrust::device, cuPos, cuPos + n, compute(cuPos));
}