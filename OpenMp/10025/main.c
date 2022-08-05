#include <stdio.h>
#include <omp.h>
#include <limits.h>
#include <math.h>
#define MAXN 500

unsigned short A[MAXN][MAXN], B[MAXN][MAXN];
int AH, AW, BH, BW;

void input(unsigned short C[][MAXN], int h, int w){
	for (int i = 0; i < h; i ++){
		for (int j = 0; j < w; j ++){
			scanf("%hu", &C[i][j]);
		}
	}
	return;
}


void print(unsigned short C[][MAXN], int h, int w){
	for (int i = 0; i < h; i ++){
		for(int j = 0; j < w; j ++){
			printf("%hu%c", C[i][j], j != w - 1 ? ' ' : '\n');
		}
	}
	return;
}

int Difference(int Ai, int Aj){
	int sum = 0;
	for (int i = 0; i < BH; i ++){
		for (int j = 0; j < BW; j++){
			sum +=(A[Ai + i][Aj + j] - B[i][j]) * (A[Ai + i][Aj + j] - B[i][j]);
		}
	}
	return sum;
}
int main(){
	

	while (scanf("%d%d%d%d", &AH, &AW, &BH, &BW) != EOF){
		input(A, AH, AW);
		input(B, BH, BW);
		
		int minimum = INT_MAX;
		int row = 0, col = 0;


#pragma omp parallel for
		for (int i = 0; i <= AH - BH; i ++){
			for(int j = 0; j <= AW - BW; j ++){
				int difference = Difference(i, j);
				//printf("i j %d %d : %d\n", i, j, difference);
				#pragma omp critical
				{
				if (difference < minimum || (difference == minimum && (i < row - 1|| (i == row - 1 && j < col - 1)))){
					minimum = difference;
					row = i + 1;
					col = j + 1;
				}
				}

			}
		}
		printf("%d %d\n", row, col);
	}
}
