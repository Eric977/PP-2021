#define MAXN 1024
#define uint32_t unsigned int
 
__kernel void matrixmul(const __global uint32_t * matrixA,
                        const __global uint32_t * matrixB,
                        __global uint32_t * matrixC)
{
    int row = get_global_id(0);
    int col = get_global_id(1);
    uint32_t sum = 0;
 
    int N = get_global_size(0);
 
    for (int i = 0; i < N; i++){
        sum += matrixA[MAXN * row + i] * matrixB[MAXN * col + i];
    }
    matrixC[MAXN * row + col] = sum;
 
 
}