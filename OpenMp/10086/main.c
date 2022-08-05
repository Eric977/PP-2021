#include <stdio.h>
#include <omp.h>
#define MAXN 1000
 
char T[MAXN][MAXN];
int N;
 
void clean(){
#pragma omp parallel for
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < N; j ++){
            if (T[i][j] == 'N')
                T[i][j] = 'W';
        }
    }
}
void moveR(){
#pragma omp parallel for
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < N; j ++){
            if (T[i][j] == 'R' && T[i][(j + 1) % N] == 'W'){
                T[i][(j + 1) % N] = 'R';
                T[i][j] = 'N';
                j ++;
            }
        }
    }
}
 
void moveB(){
#pragma omp parallel for
    for (int j = 0; j < N; j ++){
        for (int i = 0; i < N; i ++){
            if (T[i][j] == 'B' && T[(i + 1) % N][j] == 'W'){
                T[(i + 1) % N][j] = 'B';
                T[i][j] = 'N';
                i ++;
            }
        }
    }
}
void print(void){
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < N; j ++){
            printf("%c", T[i][j]);
        }
        printf("\n");
    }
}
int main(){
    int M;
    scanf("%d%d", &N, &M);
 
    char buf[MAXN];
    for (int i = 0; i < N; i ++){
        scanf("%s", T[i]);
    }
    for (int i = 0; i < M; i ++){
        moveR();
        clean();
        moveB();
        clean();
    }
 
    print();
    return 0;
}