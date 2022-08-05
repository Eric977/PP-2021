#define Size 16
#define MAXN 2064
 
 
__kernel void simulate(__global char A[MAXN][MAXN],
                       __global char B[MAXN][MAXN], int N)
{
    int i = get_global_id(0);
    int j = get_global_id(1);
    int localI = get_local_id(0);
    int localJ = get_local_id(1);
    __local char buf[Size][Size];
    int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    char cur = A[i][j];
    buf[localI][localJ] = cur;
    barrier(CLK_LOCAL_MEM_FENCE);
    int count = 0;
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