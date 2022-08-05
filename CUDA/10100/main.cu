#include <stdio.h>
#include <stdint.h>
#include <cuda.h>
// #define DEBUG
#define UINT uint32_t
#define MAXN 1024
#define Size 1024
#define b    8
 
 
 
int N;
__global__ void matrixmul(UINT A[], UINT B[], UINT C[], int N)
{
    int row = blockIdx.x * b + threadIdx.x;
    int column = blockIdx.y * b + threadIdx.y;
    __shared__ int sA[b][b];
    __shared__ int sB[b][b];
 
    int sum = 0;
    for (int r = 0; r < N / b; r++) {
        sA[threadIdx.x][threadIdx.y] = A[row * N + r * b + threadIdx.y];
        sB[threadIdx.x][threadIdx.y] = B[(r * b  + threadIdx.x) * N + column];
        __syncthreads();
        for (int k = 0; k < b; k++)
            sum += sA[threadIdx.x][k] * sB[k][threadIdx.y];
        __syncthreads();
  }
  C[row * N + column] = sum;
 
}
 
void rand_gen(UINT c, int N, UINT A[]) {
    UINT x = 2, n = N*N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x = (x * x + c + i + j)%n;
            A[i * N + j] = x;
        }
    }
}
void print_matrix(int N, UINT A[]) {
    for (int i = 0; i < N; i++) {
        fprintf(stderr, "[");
        for (int j = 0; j < N; j++)
            fprintf(stderr, " %u", A[i * N + j]);
        fprintf(stderr, " ]\n");
    }
}
UINT signature(int N, UINT A[]) {
    UINT h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            h = (h + A[i * N + j]) * 2654435761LU;
    }
    return h;
}
UINT A[MAXN * MAXN], B[MAXN * MAXN], C[MAXN * MAXN];
int main() {
    uint32_t S1, S2;
    scanf("%d %u %u", &N, &S1, &S2);
    rand_gen(S1, N, A);
    rand_gen(S2, N, B);
 
    int size = N * Size * sizeof(UINT);
    UINT *cuA, *cuB, *cuC;
    cudaMalloc((void**)&cuA, size);
    cudaMalloc((void**)&cuB, size);
    cudaMalloc((void**)&cuC, size);
 
    cudaMemcpy(cuA, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(cuB, B, size, cudaMemcpyHostToDevice);
    dim3 blocks(b, b);
    dim3 grid(N / b, N / b);
    matrixmul <<< grid, blocks >>> (cuA, cuB, cuC, N);
 
    cudaMemcpy(C, cuC, size, cudaMemcpyDeviceToHost);
 
    //print_matrix(N, C);
    printf("%u\n", signature(N, C));
    return 0;
}