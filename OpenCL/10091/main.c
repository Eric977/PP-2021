#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
//#define DEBUG
#define UINT uint32_t
#define MAXN 1024
#define MAXK 1024
#define MAXGPU 10
#define MAXPLATFORM 5
#define PLATFORM 0
#define GPUid 0
#define LOGSIZE 4096
#define LOCALSIZE 8
 
cl_context context;
cl_program program;
cl_kernel  kernel;
cl_command_queue Queue;
cl_mem     bufferA, bufferB, bufferC;
cl_int status;
UINT A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
int N;
 
#define Check(status) \
    if (status != CL_SUCCESS){ \
        fprintf(stderr, "Error in Line %u\n\n", __LINE__);\
        release(); \
    }
 
 
void release(){
    clReleaseContext(context);    /* context etcmake */
    clReleaseCommandQueue(Queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(bufferC); 
}
int init(char file[]){
    cl_platform_id platform_id[MAXPLATFORM];
    cl_uint platform_id_got;
    cl_device_id GPU[MAXGPU];
    cl_uint GPU_id_got;
    status = clGetPlatformIDs(MAXPLATFORM, platform_id, 
                &platform_id_got);
    Check(status);
    status = clGetDeviceIDs(platform_id[PLATFORM], CL_DEVICE_TYPE_GPU, 
                MAXGPU, GPU, &GPU_id_got);
    Check(status)
    /* getcontext */
    context = 
      clCreateContext(NULL, 1, &GPU[GPUid], NULL, NULL, 
              &status);
    Check(status);
    /* commandqueue */
    Queue =
      clCreateCommandQueueWithProperties(context, GPU[GPUid], NULL, &status);
 
    Check(status);
 
    /* kernelsource */
    FILE *kernelfp = fopen(file, "r");
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = 
      fread(kernelBuffer, 1, MAXK, kernelfp);
 
    program =
      clCreateProgramWithSource(context, 1, &constKernelSource, 
                    &kernelLength, &status);
 
    Check(status);
 
    /* buildprogram */
    status = 
      clBuildProgram(program, 1, &GPU[GPUid], NULL, NULL, 
             NULL);
    if (status != CL_SUCCESS){
        char log[LOGSIZE];
        status = clGetProgramBuildInfo(program, GPU[GPUid], CL_PROGRAM_BUILD_LOG            , LOGSIZE, log, NULL);
        printf("%s", log);
 
    }
    /* createkernel */
    kernel = clCreateKernel(program, "matrixmul", &status);
    Check(status);
 
    bufferA =
      clCreateBuffer(context,
             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
             MAXN * MAXN * sizeof(cl_uint), A, &status);
    bufferB =
      clCreateBuffer(context,
             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
             MAXN * MAXN * sizeof(cl_uint), B, &status);
 
    bufferC = 
        clCreateBuffer(context, 
             CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
             MAXN * MAXN * sizeof(cl_uint), C, &status);  
}
 
int run(){
    size_t globalThreads[] = {(size_t)N, (size_t)N};
    size_t localThreads[] = {1, 1};
 
 
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem),
                (void*)&bufferA);
    Check(status);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem),
                (void*)&bufferB);
    Check(status);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem),
                (void*)&bufferC);
    Check(status);
 
    status =
      clEnqueueNDRangeKernel(Queue, kernel, 2, NULL,
                 globalThreads, localThreads,
                 0, NULL, NULL);
 
    Check(status);
    clEnqueueReadBuffer(Queue, bufferC, CL_TRUE,
                0, MAXN * MAXN * sizeof(cl_uint), C,
                0, NULL, NULL);
    Check(status);
    return 0;
}
 
void multiply(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            UINT sum = 0;    // overflow, let it go.
            for (int k = 0; k < N; k++)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
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
int main() {
    uint32_t S1, S2;
    scanf("%d %u %u", &N, &S1, &S2);
    rand_gen(S1, N, A);
    rand_gen(S2, N, B);
    //multiply(N, A, B, C);
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < i; j ++){
            UINT tmp = B[i][j];
            B[i][j] = B[j][i];
            B[j][i] = tmp;
        }
    }
    init("matrixmul.cl");
    run();
#ifdef DEBUG
    print_matrix(N, A);
    print_matrix(N, B);
    print_matrix(N, C);
#endif
    printf("%u\n", signature(N, C));
 
 
    clReleaseContext(context);    /* context etcmake */
    clReleaseCommandQueue(Queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(bufferA);    /* buffers */
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
 
   return 0;
}