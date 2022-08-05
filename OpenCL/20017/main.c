#include <stdio.h>
#include <CL/cl.h>
#include <omp.h>
#include "utils.h"
#include <inttypes.h>
#define MAXK 4096
#define MAXGPU 3
#define MAXPLATFORM 3
#define PLATFORM 0
#define GPUID 0
#define LOGSIZE 4096
#define MAXN 67108864
#define LOCALSIZE 256
 
int N;
int n;
uint32_t key1, key2;
uint32_t C[MAXN / LOCALSIZE];
 
cl_context context;
cl_program program;
cl_kernel  kernel;
cl_command_queue Queue;
cl_mem     bufferC;
cl_int status;
 
 
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
      clCreateContext(NULL, 1, &GPU[GPUID], NULL, NULL, 
              &status);
    Check(status);
    /* commandqueue */
    Queue =
      clCreateCommandQueueWithProperties(context, GPU[GPUID], NULL, &status);
 
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
      clBuildProgram(program, 1, &GPU[GPUID], NULL, NULL, 
             NULL);
    if (status != CL_SUCCESS){
        char log[LOGSIZE];
        status = clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG            , LOGSIZE, log, NULL);
        printf("%s", log);
 
    }
    /* createkernel */
    kernel = clCreateKernel(program, "vecdot", &status);
    Check(status);
 
    bufferC = 
        clCreateBuffer(context, 
                       CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                       MAXN / LOCALSIZE * sizeof(cl_uint), C, &status);  
}
int run(){
 
    n = (N - 1) / LOCALSIZE + 1;
    n = n - n % LOCALSIZE + LOCALSIZE;
    size_t globalThreads[] = {(size_t)n};
    size_t localThreads[] = {LOCALSIZE};
 
    int group = n / LOCALSIZE;
 
    status = clSetKernelArg(kernel, 0, sizeof(cl_uint), 
                (void*)&key1);
    status = clSetKernelArg(kernel, 1, sizeof(cl_uint), 
                (void*)&key2);
    status = clSetKernelArg(kernel, 2, sizeof(cl_uint), 
                (void*)&N);
    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), 
                (void*)&bufferC);
 
 
    status = 
      clEnqueueNDRangeKernel(Queue, kernel, 1, NULL, 
                 globalThreads, localThreads, 
                 0, NULL, NULL);
 
    clEnqueueReadBuffer(Queue, bufferC, CL_TRUE, 
                0,  group * sizeof(cl_uint), C, 
                0, NULL, NULL);
    uint32_t sum = 0;
    //printf("group = %d\n", group);
    for (int i = 0; i < group; i ++){
        sum += C[i];
    }
    printf("%" PRIu32 "\n", sum);
 
    return 0;
 
}
 
int main(){
    init("vecdot.cl");
    while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
        run();
    }
    return 0;
}