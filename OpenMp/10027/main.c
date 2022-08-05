#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#define MAXN 65536
 
#define set(v,j) (v |= (1 << j))
#define clear(v,j) (v ^= (1 << j))
#define test(v,j) ((v>>j) & 1)
 
int C[81][2], N;
int R[9][3] = {0};
int depth; 
int States[MAXN*9][9][3];
int num = 0;
int conflict(int try, int i, int j, int Num[9][3]){
    return test(Num[i][0], try) || test(Num[j][1], try) || test(Num[i/3*3+j/3][2], try);
}
 
void Set(int i, int x, int y, int state[9][3]){
    set(state[x][0], i);
    set(state[y][1], i);
    set(state[x/3*3+y/3][2], i);
}
 
void Clear(int num, int i, int j, int Num[9][3]){
    clear(Num[i][0], num);
    clear(Num[j][1], num);
    clear(Num[i/3*3+j/3][2], num);
}
int dfs(int k, int state[][3]) {
    if (k == N) {
        return 1;
    }
 
    int count = 0;
    int x = C[k][0], y = C[k][1];
    int c = x/3*3 + y/3;
    for (int i = 1; i <= 9; i++) 
    {
        int mask = 1<<i;
        if ( test(state[x][0], i) || test(state[y][1], i) || test(state[x/3*3+y/3][2], i))
            continue;
 
        set(state[x][0], i);
        set(state[y][1], i);
        set(state[x/3*3+y/3][2], i);
        count += dfs(k+1, state);
        clear(state[x][0], i);
        clear(state[y][1], i);
        clear(state[x/3*3+y/3][2], i);
    }
    return count;
}
 
void bfs(int k, int state[9][3]){
    if (k == depth){
       memcpy(States[num], state, sizeof(States[0]));
       num ++;
    }
    int x = C[k][0], y = C[k][1];
    for (int i = 1; i <= 9; i ++){
        if ( test(state[x][0], i) || test(state[y][1], i) || test(state[x/3*3+y/3][2], i))
            continue;
 
        state[x][0] |= (1 << i);
        state[y][1] |= (1 << i);
        state[x/3*3+y/3][2] |= (1 << i);
        bfs(k+1, state);
        state[x][0] ^= (1 << i);
        state[y][1] ^= (1 << i);
        state[x/3*3+y/3][2] ^= (1 << i);
    }
    return;
}
int main() {
    int sudoku[9][9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            scanf("%d", &sudoku[i][j]);
    N = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (sudoku[i][j]) {
                Set(sudoku[i][j], i, j, R);
            } 
            else {
                C[N][0] = i, C[N][1] = j;
                N++;
           }
        }
    }
    for (depth = 1; depth <= N; depth ++){
        num = 0;
        bfs(0, R);
        printf("%d %d\n", depth, num);
        if (num >= MAXN)
            break;
    }
    if (depth == N + 1){
        printf("%d\n", num);
        return 0;
    }
    int res = 0;
    #pragma omp parallel for reduction(+:res) schedule(guided)
    for (int i = 0; i < num; i ++){
       res += dfs(depth, States[i]);
    }
    printf("%d\n", res);
    return 0;
}
