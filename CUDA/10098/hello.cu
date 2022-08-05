#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
 
 
int main(){
    int nDevices;
    cudaGetDeviceCount(&nDevices);
    printf("%d devices found supporting CUDA\n", nDevices);
 
    cudaDeviceProp devInfo;
    for (int i = 0; i < nDevices; i ++){
      cudaDeviceProp devInfo;
      cudaGetDeviceProperties(&devInfo, i);
      printf("----------------------------------\n");
      printf("Device %s\n", devInfo.name);
      printf("----------------------------------\n");
      printf(" Device memory: \t%zu\n", devInfo.totalGlobalMem);
      printf(" Memory per-block: \t%zu\n", devInfo.sharedMemPerBlock);
      printf(" Register per-block: \t%d\n", devInfo.regsPerBlock);
      printf(" Warp size: \t\t%d\n", devInfo.warpSize);
      printf(" Memory pitch: \t\t%zu\n", devInfo.memPitch);
      printf(" Constant Memory: \t%zu\n", devInfo.totalConstMem);
      printf(" Max thread per-block: \t%d\n", devInfo.maxThreadsPerBlock);
      printf(" Max thread dim: \t%d / %d / %d\n", 
          devInfo.maxThreadsDim[0], devInfo.maxThreadsDim[1], devInfo.maxThreadsDim[2]);
      printf(" Max grid size: \t%d / %d / %d\n", 
          devInfo.maxGridSize[0], devInfo.maxGridSize[1], devInfo.maxGridSize[2]);
      printf(" Ver: \t\t\t%d.%d\n", devInfo.major, devInfo.minor);
      printf(" Clock: \t\t%d\n", devInfo.clockRate);
      printf(" Texture Alignment: \t%zu\n", devInfo.textureAlignment);
 
 
    }
 
}