#include "matrix.h"
#include "stdio.h"
#define MAX_THREAD 6
#define MAXN 2048
 
 
int n;
int num_thread;
unsigned long (*mA)[MAXN];
unsigned long (*mB)[MAXN];
unsigned long (*mC)[MAXN];
 
 
void* myMulti(void* data){
    int tid = *(int*)data;
    int size = n / num_thread;
    int start = tid * size;
    int end = start + size;
 
    if (tid == num_thread - 1){
        end = n;        
    }
 
    for (int i = start; i < end; i ++){
        for (int j = 0; j < n; j ++){
            unsigned long count = 0;
            for (int k = 0; k < n; k ++){
                count += (mA[i][k] * mB[j][k]);
            }
            mC[i][j] = count;
        }
    }
    pthread_exit(NULL);
 
}
 
 
void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < i; j ++){
            unsigned long tmp = B[i][j];
            B[i][j] = B[j][i];
            B[j][i] = tmp;
        }
    }
    n  = N;
    mA = A;
    mB = B;
    mC = C;
 
    num_thread = n < MAX_THREAD ? n : MAX_THREAD;
    pthread_attr_t attr;
    pthread_t threads[num_thread];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int      tid[num_thread];
 
    for (int i = 0; i < num_thread; i ++){
        tid[i] = i; 
        pthread_create(&threads[i], &attr, myMulti, (void*)&tid[i]);
    }
    for (int i = 0; i < num_thread; i ++){
        pthread_join(threads[i], NULL);
    }
    return;
}