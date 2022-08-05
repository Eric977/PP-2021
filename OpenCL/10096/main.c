#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
//#define DEBUG
#define UINT uint32_t
#define MAXN 2064
#define MAXK 1024
#define MAXGPU 8
#define MAXPLATFORM 3
#define PLATFORM 0
#define GPUid 0
#define LOGSIZE 4096
#define LOCALSIZE 16
 
cl_context context;
cl_program program;
cl_kernel  kernel;
cl_command_queue Queue;
cl_mem     buffer[2];
cl_int status;
char A[2][MAXN][MAXN] = {0};
int N, M, n;
 
#define Check(status) \
    if (status != CL_SUCCESS){ \
        fprintf(stderr, "Error in Line %u\n\n", __LINE__);\
        release(); \
    }
 
 
void print(char A[][MAXN][MAXN], int cur){
    for (int i = 0; i < N; i ++){
        for (int j = 0; j < N; j ++){
            printf("%c", A[cur][i][j] + '0');
        }
        printf("\n");
    }
    return;
 
}
void release(){
    clReleaseContext(context);    /* context etcmake */
    clReleaseCommandQueue(Queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(buffer[0]); 
    clReleaseMemObject(buffer[1]); 
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
    kernel = clCreateKernel(program, "simulate", &status);
    Check(status);
 
    buffer[0] =
      clCreateBuffer(context,
             CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
             MAXN * MAXN * sizeof(char), A[0], &status);
    buffer[1] =
      clCreateBuffer(context,
             CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
             MAXN * MAXN * sizeof(char), A[1], &status);
}
 
int run(){
    size_t globalThreads[] = {(size_t)n, (size_t)n};
    size_t localThreads[] = {LOCALSIZE, LOCALSIZE};
    int flag = 0;
    for (int i = 0; i < M; i ++){
        status = clSetKernelArg(kernel, 0, sizeof(cl_mem),
                    (void*)&buffer[flag]);
        Check(status);
        status = clSetKernelArg(kernel, 1, sizeof(cl_mem),
                    (void*)&buffer[!flag]);
        Check(status);
        status = clSetKernelArg(kernel, 2, sizeof(int),
                    (void*)&N);
        Check(status);
 
        status =
          clEnqueueNDRangeKernel(Queue, kernel, 2, NULL,
                     globalThreads, localThreads,
                     0, NULL, NULL);
        flag = !flag;
    }
 
    clEnqueueReadBuffer(Queue, buffer[flag], CL_TRUE,
                0, MAXN * MAXN * sizeof(char), A[flag],
                0, NULL, NULL);
    Check(status);
    print(A, flag);
    return 0;
}
 
 
int main() {
    scanf("%d%d", &N, &M);
    n = (1 + N / LOCALSIZE) * LOCALSIZE;
    for (int i = 0; i < N; i ++){
        scanf("%s", A[0][i]);
        int v = 0;
        for (int j = 0; j < N; j ++){
            A[0][i][j] -= '0';   
        }
    }
 
    init("game-of-life.cl");
    run();
    release();
   return 0;
}