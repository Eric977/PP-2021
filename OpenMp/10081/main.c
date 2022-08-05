#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
 
#define MAXN 2048
char A[2][MAXN][MAXN] = {0};
int N, M;
#define set(v,j) v |= (1 << j)
#define clear(v,j) v &= ~(1 << j)
#define test(v,j) ((v>>j) & 1)
#define Test(A, i, j) test(A[i], j)
 
#define chunk 64
 
 
#define nLiveRight(A, i, j, cur)\
    A[cur][i-1][j+1] + A[cur][i][j+1] + A[cur][i+1][j + 1]
#define nLiveLeft(A, i, j, cur) \
    A[cur][i-1][j - 1] + A[cur][i][j - 1] + A[cur][i + 1][j - 1]
#define nLiveCur(A, i, j, cur) \
    A[cur][i - 1][j] + A[cur][i + 1][j]
 
#define nLive(A, i, j, cur) \
    A[cur][i-1][j-1] + A[cur][i-1][j] + A[cur][i-1][j+1] + \
    A[cur][i][j-1] + A[cur][i][j+1] + \
    A[cur][i+1][j-1] + A[cur][i+1][j] + A[cur][i+1][j+1] 
 
void print(char A[][MAXN][MAXN], int cur){
    for (int i = 1; i <= N; i ++){
        for (int j = 1; j <= N; j ++){
            printf("%c", A[cur][i][j] + '0');
        }
        printf("\n");
    }
    return;
 
}
 
void Simulate(int cur){
#pragma omp parallel for schedule(dynamic, chunk)
    for (int i = 1; i <= N; i ++){
        int l = nLiveLeft(A, i, 1, cur);
        int mid = nLiveCur(A, i, 1, cur);
        int r = nLiveRight(A, i, 1, cur);
        int v = l + mid + r;
        int live = A[cur][i][1];
        A[!cur][i][1] = (live && (v == 2 || v == 3)) || (!live && (v == 3));
        mid += live;
        for (int j = 2; j <= N; j ++){
            live = A[cur][i][j];
            l = mid; mid = r - live; r = nLiveRight(A, i, j, cur);
            v = l + mid + r;
            A[!cur][i][j] = (live && (v == 2 || v == 3)) || (!live && (v == 3));
            mid += live;
        }
    }
 
}
 
int main(void){
 
    scanf("%d%d", &N, &M);
    for (int i = 1; i <= N; i ++){
        scanf("%s", A[0][i]+1);
        int v = 0;
        for (int j = 1; j <= N; j ++){
            A[0][i][j] -= '0';    
        }
    }
    for (int i = 0; i < M; i ++){
        Simulate(i & 1);
    }
 
    print(A, M & 1);
 
 
 
 
    return 0;
}