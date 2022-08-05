#include <stdio.h>
#include <CL/cl.h>

#define MAXB 256
#define MAXPLATFORM 5
#define MAXDEVICE 10

int main()
{
  cl_platform_id platform_id[MAXPLATFORM];
  cl_device_id device_id[MAXDEVICE];
  cl_uint platform_id_got;

  clGetPlatformIDs(MAXPLATFORM, platform_id, &platform_id_got);
  printf("%u platform found\n", platform_id_got);

  for (int i = 0; i < platform_id_got; i++)
  {
    char buffer[MAXB];
    size_t length;
    clGetPlatformInfo(platform_id[i], CL_PLATFORM_NAME,
                      MAXB, buffer, &length);
    printf("Platform Name %s\n", buffer);
    clGetPlatformInfo(platform_id[i], CL_PLATFORM_VENDOR,
                      MAXB, buffer, &length);

    printf("Platform Vendor %s\n", buffer);

    clGetPlatformInfo(platform_id[i], CL_PLATFORM_VERSION,
                      MAXB, buffer, &length);

    printf("Platform Version %s\n", buffer);

    clGetPlatformInfo(platform_id[i], CL_PLATFORM_PROFILE,
                      MAXB, buffer, &length);

    printf("Platform Profile %s\n", buffer);

    cl_device_id devices[MAXDEVICE];
    cl_uint device_id_got;

    clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_ALL,
                   MAXDEVICE, devices, &device_id_got);
    printf("%u Devices\n", device_id_got);

    clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_CPU,
                   MAXDEVICE, devices, &device_id_got);
    printf("%u CPU Devices\n", device_id_got);

    clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_GPU,
                   MAXDEVICE, devices, &device_id_got);
    printf("%u GPU Devices\n", device_id_got);

    for (int j = 0; j < device_id_got; j++)
    {
      clGetDeviceInfo(devices[j], CL_DEVICE_NAME,
                      MAXB, buffer, &length);
      buffer[length] = '\0';
      printf("Device name %s\n", buffer);
      cl_ulong number;
      clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE,
                      sizeof(cl_ulong), &number, NULL);
      printf("Global memory size %lld\n", (long long)number);
      clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE,
                      sizeof(cl_ulong), &number, NULL);
      printf("Local memory size %lld\n", (long long)number);
      clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                      sizeof(cl_ulong), &number, NULL);
      printf("# of compute units %lld\n", (long long)number);
      clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE,
                      sizeof(cl_ulong), &number, NULL);
      printf("max # of work items in a work group %lld\n",
             (long long)number);
    }
    /* end */
  }
  return 0;
}