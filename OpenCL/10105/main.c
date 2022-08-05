#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
//#define DEBUG
#define UINT uint32_t
#define MAXN 1024
#define MAXK 1024
#define MAXGPU 3
#define MAXPLATFORM 3
#define PLATFORM 0
#define DEVICENUM 2
#define GPUid 0
#define LOGSIZE 4096
#define LOCALSIZE 8
 
cl_context context;
cl_program program;
cl_kernel  kernel_add, kernel_mul;
cl_command_queue Queue[DEVICENUM];
cl_mem     bufferA[DEVICENUM], bufferB[DEVICENUM], bufferC[DEVICENUM];
cl_event   events[DEVICENUM];
 
cl_int status;
int N;
UINT IN[6][MAXN][MAXN], TMP[6][MAXN][MAXN];
int S[6];
#define Check(status) \
    if (status != CL_SUCCESS){ \
        fprintf(stderr, "Error in Line %u\n\n", __LINE__);\
        release(); \
    }
 
 
void release(){
    clReleaseContext(context);    /* context etcmake */
    clReleaseProgram(program);
    clReleaseKernel(kernel_mul);
    clReleaseKernel(kernel_add);
    for (int i = 0; i < DEVICENUM; i ++){
        clReleaseMemObject(bufferA[i]); 
        clReleaseMemObject(bufferB[i]); 
        clReleaseMemObject(bufferC[i]); 
        clReleaseCommandQueue(Queue[i]);
    }
}
int init(char file[]){
    cl_platform_id platform_id[MAXPLATFORM];
    cl_uint platform_id_got;
    cl_device_id GPU[MAXGPU];
    cl_uint GPU_id_got;
    status = clGetPlatformIDs(MAXPLATFORM, platform_id, &platform_id_got);
    Check(status);
    status = clGetDeviceIDs(platform_id[PLATFORM], CL_DEVICE_TYPE_GPU, MAXGPU, GPU, &GPU_id_got);
    Check(status)
    /* getcontext */
    context = 
      clCreateContext(NULL, DEVICENUM, GPU, NULL, NULL, &status);
    Check(status);
    /* commandqueue */
    for (int i = 0; i < DEVICENUM; i ++){
        Queue[i] = clCreateCommandQueueWithProperties(context, GPU[i], NULL, &status);
        Check(status);
    }
 
    /* kernelsource */
    FILE *kernelfp = fopen(file, "r");
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
 
    program = clCreateProgramWithSource(context, 1, &constKernelSource, &kernelLength, &status);
    Check(status);
 
    /* buildprogram */
    status = clBuildProgram(program, DEVICENUM, GPU, NULL, NULL, NULL);
    if (status != CL_SUCCESS){
        char log[LOGSIZE];
        status = clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG            , LOGSIZE, log, NULL);
        printf("%s", log);
 
    }
    /* createkernel */
    kernel_mul = clCreateKernel(program, "matrixmul", &status);
    Check(status);
 
    kernel_add = clCreateKernel(program, "matrixAdd", &status);
    Check(status);
 
    for (int i = 0; i < DEVICENUM; i ++){
        bufferA[i] = clCreateBuffer(context, CL_MEM_READ_WRITE,  MAXN * MAXN * sizeof(cl_uint), NULL, &status);
 
        bufferB[i] = clCreateBuffer(context, CL_MEM_READ_WRITE,  MAXN * MAXN * sizeof(cl_uint), NULL, &status);
 
        bufferC[i] = clCreateBuffer(context, CL_MEM_READ_WRITE,  MAXN * MAXN * sizeof(cl_uint), NULL, &status);
 
        Check(status)
    }
}
void transpose(UINT A[][MAXN]){
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < i; j ++){
            UINT tmp = A[i][j];
            A[i][j] = A[j][i];
            A[j][i] = tmp;
        }
    }
}
 
int GPUmul(UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN], int device){
    //transpose(B);
 
    status = clEnqueueWriteBuffer(Queue[device], bufferA[device], 0, 0, MAXN * MAXN * sizeof(cl_uint),
                A, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(Queue[device], bufferB[device], 0, 0, MAXN * MAXN * sizeof(cl_uint),
                B, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(Queue[device], bufferC[device], 0, 0, MAXN * MAXN * sizeof(cl_uint),
                C, 0, NULL, NULL);
 
 
    size_t globalThreads[] = {(size_t)N, (size_t)N};
    size_t localThreads[] = {1, 1}; 
    status = clSetKernelArg(kernel_mul, 0, sizeof(cl_mem), (void*)&bufferA[device]);
    Check(status);
    status = clSetKernelArg(kernel_mul, 1, sizeof(cl_mem), (void*)&bufferB[device]);
    Check(status);
    status = clSetKernelArg(kernel_mul, 2, sizeof(cl_mem), (void*)&bufferC[device]);
    Check(status);
 
    status = clEnqueueNDRangeKernel(Queue[device], kernel_mul, 2, NULL, globalThreads, localThreads,
                 0, NULL, &(events[device]));
 
    Check(status);
 
    clEnqueueReadBuffer(Queue[device], bufferC[device], CL_TRUE, 0, MAXN * MAXN * sizeof(cl_uint), C,
                0, NULL, NULL);
    return 0;
}
 
int GPUadd(UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN], int device){ 
    status = clEnqueueWriteBuffer(Queue[device], bufferA[device], 0, 0, MAXN * MAXN * sizeof(cl_uint), A, 0, NULL, NULL);
 
    status = clEnqueueWriteBuffer(Queue[device], bufferB[device], 0, 0, MAXN * MAXN * sizeof(cl_uint), B, 0, NULL, NULL);
 
    status = clEnqueueWriteBuffer(Queue[device], bufferC[device], 0, 0, MAXN * MAXN * sizeof(cl_uint), C, 0, NULL, NULL);
    Check(status); 
 
    size_t globalThreads[] = {(size_t)N, (size_t)N};
    size_t localThreads[] = {1, 1};
 
 
    status = clSetKernelArg(kernel_add, 0, sizeof(cl_mem),
                (void*)&bufferA[device]);
    Check(status);
    status = clSetKernelArg(kernel_add, 1, sizeof(cl_mem),
                (void*)&bufferB[device]);
    Check(status);
    status = clSetKernelArg(kernel_add, 2, sizeof(cl_mem),
                (void*)&bufferC[device]);
    Check(status);
 
    status = clEnqueueNDRangeKernel(Queue[device], kernel_add, 2, NULL, globalThreads, localThreads,
                 0, NULL, NULL);
 
    Check(status);
    status = clEnqueueReadBuffer(Queue[device], bufferC[device], CL_TRUE, 0, MAXN * MAXN * sizeof(cl_uint), C,
                0, NULL, NULL);
    Check(status);
    return 0;
}
 
void rand_gen(UINT c, int N, UINT A[][MAXN]) {
    UINT x = 2, n = N*N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x = (x * x + c + i + j)%n;
            A[i][j] = x;
        }
    }
}
void print_matrix(int N, UINT A[][MAXN]) {
    for (int i = 0; i < N; i++) {
        fprintf(stderr, "[");
        for (int j = 0; j < N; j++)
            fprintf(stderr, " %u", A[i][j]);
        fprintf(stderr, " ]\n");
    }
}
UINT signature(int N, UINT A[][MAXN]) {
    UINT h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            h = (h + A[i][j]) * 2654435761LU;
    }
    return h;
}
 
void task(){ 
    for (int i = 0; i < 6; i++) {
        scanf("%d", &S[i]);
        rand_gen(S[i], N, IN[i]);
    }
 
    GPUmul(IN[0], IN[1], TMP[0], 0);
    GPUmul(IN[2], IN[3], TMP[1], 1);
 
    clFinish(Queue[0]);
    clFinish(Queue[1]);
 
    GPUmul(TMP[0], IN[4], TMP[3], 0);
    GPUmul(TMP[1], IN[5], TMP[4], 1);
 
    clFinish(Queue[0]);
    clFinish(Queue[1]);
 
    GPUadd(TMP[0], TMP[1], TMP[2],0); 
    GPUadd(TMP[3], TMP[4], TMP[5], 1);
 
    clFinish(Queue[0]);
    clFinish(Queue[1]);
 
    printf("%u\n", signature(N, TMP[2]));
    printf("%u\n", signature(N, TMP[5]));
}
 
int main() {
    init("matrix-lib.cl");
    while (scanf("%d", &N) != EOF)
        task();
    release();
    return 0;
}