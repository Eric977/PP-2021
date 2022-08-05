#include <stdio.h>
#include <CL/cl.h>
#include <assert.h>

#define MAXK 1024
#define MAXGPU 10
#define MAXPLATFORM 5
int main(int argc, char *argv[])
{
char file[100];
  scanf("%s", file);
  cl_int status;
  cl_platform_id platform_id[MAXPLATFORM];
  cl_uint platform_id_got;
  status = clGetPlatformIDs(MAXPLATFORM, platform_id,
                            &platform_id_got);
  cl_device_id GPU[MAXGPU];
  cl_uint GPU_id_got;
  status = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU,
                          MAXGPU, GPU, &GPU_id_got);
  /* getcontext */
  cl_context context =
      clCreateContext(NULL, GPU_id_got, GPU, NULL, NULL,
                      &status);
  /* commandqueue */
  cl_command_queue commandQueue =
      clCreateCommandQueueWithProperties(context, GPU[0], NULL, &status);
  /* kernelsource */
  FILE *kernelfp = fopen(file, "r");
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength =
      fread(kernelBuffer, 1, MAXK, kernelfp);
  cl_program program =
      clCreateProgramWithSource(context, 1, &constKernelSource,
                                &kernelLength, &status);
  /* buildprogram */
  status =
      clBuildProgram(program, GPU_id_got, GPU, NULL, NULL,
                     NULL);
  if (status != CL_SUCCESS)
  {
    char log[32768];
    status = clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG,
                                   32768, log, NULL);
    printf("%s", log);
  }
}