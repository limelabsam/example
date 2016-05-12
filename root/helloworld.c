#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CL/cl.h>
#define FSLCL_ERROR -1
#define FSLCL_SUCCESS CL_SUCCESS
cl_mem buffer_input = NULL;
cl_mem buffer_output = NULL;
cl_kernel buffer_kernel = NULL;
size_t buffer_size = 0;
int buffer_width = 1024;
int buffer_height = 1024;
cl_platform_id platform_id;
cl_device_id device_id;
cl_context context;
cl_command_queue cq;
cl_program program;
cl_kernel kernel;
struct fsl_kernel_src_str
{
 char *src;
 long size;
};
typedef struct fsl_kernel_src_str fsl_kernel_src;
cl_int FSLCL_LoadKernelSource (char *filename, fsl_kernel_src *kernel)
{
 FILE *fp = NULL;
 fp = fopen (filename, "rb");
 if (fp == NULL)
 {
 printf ("\nFailed to open: %s\n", filename);
 return FSLCL_ERROR;
 }
fseek (fp, 0, SEEK_END);
 kernel->size = ftell (fp);
 rewind (fp);
 kernel->src = (char *) malloc (sizeof (char) * kernel->size);
 if (! kernel->src)
 {
 printf ("\nError Allocating memory to load CL program");
 return FSLCL_ERROR;
 }
fread (kernel->src, 1, kernel->size, fp);
 kernel->src[kernel->size] = '\0';
 fclose (fp);
 return FSLCL_SUCCESS;
}

int main (int argc, char **argv)
{
 int dimension = 2;
 size_t global[2] = {buffer_width, buffer_height};
 size_t local[2] = {4, 16};
 int size_2d = buffer_width * buffer_height;
 cl_int ret;
 cl_uint platforms;
 char **data;
 char **data2;
 int i, j;


 data = (char **) malloc (buffer_width * sizeof (char *));
 data2 = (char **) malloc (buffer_width * sizeof (char *));

 for (i = 0; i < buffer_width; i++)
 {
 data[i] = (char *) malloc (buffer_height * sizeof (char));
 data2[i] = (char *) malloc (buffer_height * sizeof (char));
 }

 for (i = 0; i < buffer_width; i++)
 {
 for (j = 0; j < buffer_height; j++)
 {
 data[i][j] = 0;
 data2[i][j] = 0;
 }
 }
ret = clGetPlatformIDs (1, &platform_id, &platforms );
 assert (ret == CL_SUCCESS);
 cl_uint devices;
 ret = clGetDeviceIDs (platform_id,CL_DEVICE_TYPE_GPU,1,&device_id, &devices);
 assert (ret == CL_SUCCESS);
cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
context = clCreateContext(properties, 1, &device_id, NULL,NULL,&ret);
assert (ret == CL_SUCCESS);
 cq = clCreateCommandQueue(context, device_id, 0, &ret);
assert (ret == CL_SUCCESS);
 fsl_kernel_src app_kernel;
ret = FSLCL_LoadKernelSource ((char *)"helloworld.cl", &app_kernel);
// Submit the source code of the kernel to OpenCL
program = clCreateProgramWithSource (context, 1, (const char **)&app_kernel.src, 0,&ret);
if (ret == CL_SUCCESS)
// and compile it (after this we could extract the compiled version)
ret = clBuildProgram (program, 1, &device_id, NULL, NULL, NULL);
if (ret < 0)
{
printf ("Failed\n");
printf ("\nReturn: %d\n", ret);
clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, app_kernel.size, app_kernel.src, NULL);
printf ("\n%s", app_kernel.src);
}
assert(ret == CL_SUCCESS); 
buffer_input = clCreateBuffer (context, CL_MEM_READ_ONLY, size_2d, NULL, &ret);
 assert (ret == CL_SUCCESS);
 buffer_output = clCreateBuffer (context, CL_MEM_WRITE_ONLY , size_2d, NULL, &ret);
 assert (ret == CL_SUCCESS);
 // get a handle and map parameters for the kernel
 kernel = clCreateKernel(program, "helloworld", &ret);
 assert (ret == CL_SUCCESS);
 clSetKernelArg (kernel, 0, sizeof(cl_mem), &buffer_input);
 clSetKernelArg (kernel, 1, sizeof(cl_mem), &buffer_output);
 clSetKernelArg (kernel, 2, sizeof(int), &buffer_width);
 clSetKernelArg (kernel, 3, sizeof(int), &buffer_height);
 for (i = 0; i < buffer_width; i++)
 {
 for (j = 0; j < buffer_height; j++)
 {
 data[i][j] = rand () % 10;
 }
 }
 ret = clEnqueueWriteBuffer (cq, buffer_input, CL_TRUE, 0, size_2d, data, 0, NULL, NULL);
 assert (ret == CL_SUCCESS);
 ret = clEnqueueNDRangeKernel(cq, kernel, dimension, NULL, &global[0], &local[0], 0, NULL, NULL);
 assert (ret == CL_SUCCESS);
 ret = clEnqueueReadBuffer(cq, buffer_output, CL_TRUE, 0, size_2d, data2, 0, NULL, NULL);
 assert (ret == CL_SUCCESS);
 printf ("\nResult:\n");
 for (i = 0; i < buffer_width; i++)
 {
 for (j = 0; j < buffer_height; j++)
 {
 printf ("\n%d -- %d", data[i][j], data2[i][j]);
 }

 }
 clFlush( cq);
 clFinish(cq);
 clReleaseMemObject (buffer_input);
 clReleaseMemObject (buffer_output);
 clReleaseContext(context);
 clReleaseKernel(kernel);
 clReleaseProgram(program);
 clReleaseCommandQueue(cq);
 return 0;
}
