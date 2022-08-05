#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
 
#define MAXITEM 10005
#define MAXWEIGHT 1000005
#define min(x,y) x > y ? y : x
#define max(x,y) x > y ? x : y
 
int W[MAXITEM], V[MAXWEIGHT];
int T[2][MAXWEIGHT] = {0};
 
 
void print(int T[], int n){
    for (int i = 0; i <= n; i ++)
        printf("%d ", T[i]);
    printf("\n");
    return;
}
int main(void){
    int N, M;
    scanf("%d %d", &N, &M);
    for (int i = 0; i < N; i ++){
        scanf("%d %d", &W[i], &V[i]);
    }
    int last = 0;
    for (int i = 0; i < N; i ++){
        int  v = V[i], w = W[i];
        #pragma omp parallel for
        for (int j = 0; j < w; j ++){
            T[!last][j] = T[last][j];
        }
        #pragma omp parallel for
        for (int j = w; j <= M; j ++){
            T[!last][j] = max(T[last][j], T[last][j-w] + v);
        }
        last = !last;
        //print(T[last], M);
 
    }
    printf("%d\n", T[last][M]);
    return 0;
 
 
}