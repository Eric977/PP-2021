#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#define MAXN 1000005

uint32_t A[MAXN][3], B[MAXN][3];
int Na, Nb;

static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
static inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}

void print(int arr[], int n){
	for (int i = 0; i < n; i ++){
		printf("%d%c", arr[i], i == n - 1 ? '\n' : ' ');
	}
	printf("\n");
}
void Print(uint32_t arr[][3], int n){
	for (int i = 0; i < n; i ++){
		printf("%d %d %d\n", arr[i][0], arr[i][1], arr[i][2]);
	}
	printf("\n");
}
int compare(const void *a, const void*b){
	int *x = (int *) a;
	int *y = (int *) b;
	if (x[0] == y[0])
		return x[1] > y[1];
	return x[0] > y[0];
}

int main(void){
	int N, M, R;
	scanf("%d %d %d", &N, &M, &R);
	
	scanf("%d %d", &Na, &Nb);
	for (int i = 0; i < Na; i ++){
		scanf("%u %u %u", &A[i][0], &A[i][1], &A[i][2]);
	}
	for (int i = 0; i < Nb; i ++)
		scanf("%u %u %u", &B[i][1], &B[i][0], &B[i][2]);
	qsort(B, Nb, sizeof(B[0]), compare);
	
	int n = 0, m = 0;
	int idxA[MAXN], idxB[MAXN];
	for (int i = 0, p = -1; i < Na; i ++){
		if (A[i][0] != p){
			idxA[n ++] = i;
			p = A[i][0];
		}
	}
	idxA[n] = Na;
	for (int i = 0, p = -1; i < Nb; i ++){
		if (B[i][0] != p){
			idxB[m ++] = i;
			p = B[i][0];
		}
	}
	idxB[m] = Nb;
	//print(idxA, n);
	//print(idxB, m);
	//Print(A, Na);
	//Print(B, Nb);
	uint32_t hash = 0;
#pragma omp parallel for reduction(+:hash)	
	for (int i = 0; i < n; i ++){
		for (int j = 0; j < m; j ++){
			uint32_t sum = 0;
			int l = idxA[i], r = idxA[i + 1];
			int l1 = idxB[j], r1 = idxB[j + 1];
			int ii = A[l][0], jj = B[l1][0];
			//printf("i j = %d %d\n", ii, jj);
			while (l < r && l1 < r1){
				int k1 = A[l][1], k2 = B[l1][1];
				if (k1 == k2){
					sum += A[l][2] * B[l1][2];
					l ++;
					l1 ++;
				}
				else if (k1 > k2)
					l1 ++;
				else
					l ++;
			}
			//printf("%d %d sum = %d\n", ii, jj, sum);
			if (sum)
				hash += encrypt((ii+1)*(jj+1), sum);
		}
	}
	printf("%u\n", hash);
	
	return 0;
}

