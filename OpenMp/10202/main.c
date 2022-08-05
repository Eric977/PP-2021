#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#define MAXN 20
 
char T[MAXN][MAXN];
int valid[MAXN];
int n;
 
void input(int N){
    for (int i = 0; i < N; i ++){
        scanf("%s", T[i]);
    }
    return;
}


 
int queen(int r, int c, int fir, int sec){
      if (r >= n)
      	return 1;
      int sum = 0;
	  int pos = ((1<<n) - 1) & (~c) &  (~fir) & (~ sec) &valid[r];
	  int p;

	  while (pos){
		p = pos & (-pos);
		pos -= p;
		sum += queen(r+1, c|p, (fir|p)<<1, (sec|p)>>1);
	  }
      return sum;
}
 
int main(){
    int r = 1;
    while (scanf("%d", &n) != EOF){
        input(n);
        int numSolution = 0;
		for (int i = 0; i < n; i ++){
			valid[i] = (1 << n) - 1;
			for (int j = 0; j < n; j ++){
				if (T[i][j] == '*')
					valid[i] ^= (1 << j);

			}
		}
		int pos = ((1<<n)-1) & valid[0];
		int p;
#pragma omp parallel for reduction(+ : numSolution) \
                schedule(dynamic, 1)
		for (int i = 0; i < n; i++) {
			p = 1 << i;
        	if (pos & p){
				numSolution += queen(1, p, p<<1, p>>1);
				
			} 
		}
        printf("Case %d: %d\n", r, numSolution);
        r ++;
    }
      return 0;
}
