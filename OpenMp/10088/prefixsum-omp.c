#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];
void print(uint32_t prefix[], int n){
	for (int i = 1; i <= n; i ++){
		printf("%d%c", prefix[i], i == n ? '\n' : ' ');
	}
}
int main() {
    int n;
    uint32_t key;
	omp_set_num_threads(MAX_THREAD);
    while (scanf("%d %" PRIu32, &n, &key) == 2) {
        uint32_t sum = 0;
        /*for (int i = 1; i <= n; i++) {
            sum += encrypt(i, key);
            prefix_sum[i] = sum;
        }*/
		uint32_t partial[5] = {0};	
		uint32_t size = n / MAX_THREAD;
		uint32_t m = MAX_THREAD * size;
#pragma omp parallel for firstprivate(sum)
		for (int i = 0; i < MAX_THREAD; i ++){
			for (int j = i * size + 1; j <= (i + 1) * size; j ++){
				sum += encrypt(j, key);
				prefix_sum[j] = sum;
			}
			partial[omp_get_thread_num()] = sum;
		}
		for (int i = 1; i < MAX_THREAD; i ++)
			partial[i] += partial[i - 1];
#pragma omp parallel for
		for (int i = 0; i < MAX_THREAD; i ++){
			if (i == 0)
				continue;
			for (int j = i * size + 1; j <= (i + 1) * size; j ++){
				prefix_sum[j] += partial[i - 1];
			}
		}

		for (int i = m + 1; i <= n; i ++){
			prefix_sum[i] = prefix_sum[i - 1] + encrypt(i, key);
		}


		//print(partial, 4);
		//print(prefix_sum, n);
		output(prefix_sum, n);
	}
    return 0;
}
