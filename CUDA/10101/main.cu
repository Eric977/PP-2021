#include <stdio.h>
#include <cuda.h>
#define MAXN 2048
#define LOCALSIZE 16
#define Size 16
int n, N, M;
__global__ void simulate(char A[MAXN][MAXN], char B[MAXN][MAXN], int N, int n){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    int localI = threadIdx.x;
    int localJ = threadIdx.y;
    __shared__ char buf[LOCALSIZE][LOCALSIZE];
    int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    char cur = A[i][j];
    buf[localI][localJ] = cur;
    __syncthreads();
        char count = 0;
    for (int k = 0; k < 8; k ++){
        int x = localI + dx[k];
        int y = localJ + dy[k];
        int origin_x = i + dx[k];
        int origin_y = j + dy[k];
 
        if (origin_x < 0 || origin_x >= N || origin_y < 0 || origin_y >= N)
            continue;
        if (x >= 0 && x < Size && y >= 0 && y < Size)
            count += buf[x][y];
        else
            count += A[origin_x][origin_y];
 
    }
    B[i][j] = (count == 3) || (cur && count == 2);
 
}
 
char A[2][MAXN][MAXN];
 
void print(char A[][MAXN][MAXN], int cur, int n){
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < N; j ++){
            printf("%c", A[cur][i][j] + '0');
        }
        printf("\n");
    }
    return;
 
}
 
int main() {
    scanf("%d%d", &N, &M);
 
        int size = MAXN * MAXN * sizeof(char);
 
    n = (1 + N / LOCALSIZE) * LOCALSIZE;
    for (int i = 0; i < N; i ++){
        scanf("%s", A[0][i]);
        for (int j = 0; j < N; j ++){
                A[0][i][j] -= '0';
                }
    }
        char *cuA[2];
        cudaMalloc((void**)&cuA[0], size);
        cudaMalloc((void**)&cuA[1], size);
        cudaMemcpy(cuA[0], A[0], size, cudaMemcpyHostToDevice);
        cudaMemcpy(cuA[1], A[1], size, cudaMemcpyHostToDevice);
 
        int local = LOCALSIZE;
        dim3 block(local, local);
        dim3 grid(n / local, n / local);
        int flag = 0;
        for (int i = 0; i < M; i ++){
                simulate<<<grid, block>>>((char (*)[MAXN])cuA[flag], (char (*)[MAXN])cuA[!flag], N, n);
                flag = !flag;
        }
    cudaDeviceSynchronize();
 
        cudaMemcpy(A[flag], cuA[flag], size, cudaMemcpyDeviceToHost);
        print(A, flag, n);
        return 0;
}