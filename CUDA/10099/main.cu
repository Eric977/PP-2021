#include <stdio.h>
#include <cuda.h>
#include <assert.h>
#include <omp.h>
#include <inttypes.h>
#include <stdint.h>
#include "utils.h"
 
#define MAXGPU 8
#define MAXCODESZ 32767
#define MAXN 16777216
#define LOCALSIZE 32
uint32_t A[MAXN], B[MAXN], C[MAXN];
 
 
__device__ static inline uint32_t Rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
__device__ static inline uint32_t Encrypt(uint32_t m, uint32_t key) {
    return (Rotate_left(m, key&31) + key)^key;
}
 
__global__ void vecdot(int key1, int key2, int N, uint32_t* C){
        __shared__ int partial_sum[LOCALSIZE];
        int localID = threadIdx.x;
        int gid = blockIdx.x * blockDim.x + threadIdx.x;
 
 
        int l = gid * LOCALSIZE, r = l + LOCALSIZE;
        int sum = 0;
        for (int i = l; i < r && i < N; ++ i){
                sum += Encrypt(i, key1) * Encrypt(i, key2);
        }
        partial_sum[localID] = sum;
        __syncthreads();
 
        #pragma unroll 8
        for (int i = LOCALSIZE >> 1; i > 0; i >>= 1){
                if (localID < i){
                        partial_sum[localID] += partial_sum[localID + i];
                }
                __syncthreads();
        }
        if (localID == 0){
                C[blockIdx.x] = partial_sum[0];
        }
 
}
int main(int argc, char *argv[]) {
    omp_set_num_threads(4);
    int N;
    uint32_t key1, key2;
        uint32_t *cuC;           
    cudaMalloc((void**)&cuC, MAXN / LOCALSIZE);
 
        while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {    
                int n = (N - 1) / LOCALSIZE + 1; 
 
                dim3 block(LOCALSIZE);
                dim3 grid(n);
                vecdot<<<grid, block>>> (key1, key2, N, cuC);
                cudaMemcpy(C, cuC, n, cudaMemcpyDeviceToHost);
                uint32_t sum = 0;
                #pragma omp parallel for schedule(static) reduction(+: sum)
                for (int i = 0; i < n; i ++){
                        sum += C[i];
                }
                printf("%" PRIu32 "\n", sum);
    }
    return 0;
}