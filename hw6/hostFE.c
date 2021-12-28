#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void print_error_message(cl_int status, cl_program* program, cl_device_id* device, char* which) {
    if (status != CL_SUCCESS)
    {
        size_t len;
        char buffer[8 * 1024];

        printf("Error: Failed to %s !!!!\n", which);
        clGetProgramBuildInfo(*program, *device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
    }
    else {
        printf("--->after %s\n", which);
    }
}

void hostFE(int filterWidth, float* filter, int imageHeight, int imageWidth,
    float* inputImage, float* outputImage, cl_device_id* device,
    cl_context* context, cl_program* program)
{
    cl_int status;
    int filterSize = filterWidth * filterWidth;

    // Step0: Initialize Device
    cl_command_queue queue = clCreateCommandQueue(*context, *device, 0, &status);
    // print_error_message(status,program,device,"clCreateCommandQueue");


    // Step1: Create Buffers/Images

    // create buffer on device
    cl_mem data_filter = clCreateBuffer(*context, CL_MEM_READ_ONLY , sizeof(float) * filterSize, NULL, &status);
    // print_error_message(status,program,device,"clCreateBuffer on data_filter");
    cl_mem data_input = clCreateBuffer(*context, CL_MEM_READ_ONLY , sizeof(float) * imageWidth * imageHeight, NULL, &status);
    // print_error_message(status,program,device,"clCreateImage2D on data_input");
    cl_mem data_output = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(float) * imageWidth * imageHeight, NULL, &status);
    // print_error_message(status,program,device,"clCreateImage2D on data_output");


    // Transfer input data to the device    
    status = clEnqueueWriteBuffer(queue, data_filter, CL_FALSE, 0, filterSize * sizeof(float), filter, 0, NULL, NULL); // blocking_read=CL_TRUE
    // print_error_message(status,program,device,"clEnqueueWriteBuffer");

    status = clEnqueueWriteBuffer(queue, data_input, CL_FALSE, 0, sizeof(float) * imageWidth * imageHeight, inputImage, 0, NULL, NULL); // blocking_read=CL_TRUE

    // print_error_message(status,program,device,"clEnqueueWriteBuffer");



    // Step2: Build Program, Select Kernel
    cl_kernel kernel = clCreateKernel(*program, "convolution", &status);
    // print_error_message(status,program,device,"clCreateKernel");


    // Step3: Set Arguments, Enqueue Kernel 

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &data_filter);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &data_input);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &data_output);
    clSetKernelArg(kernel, 3, sizeof(int), &filterWidth);
    clSetKernelArg(kernel, 4, sizeof(int), &imageWidth);
    clSetKernelArg(kernel, 5, sizeof(int), &imageHeight);
    // print_error_message(status,program,device,"clSetKernelArg");


    int work_size = imageHeight * imageWidth;
    size_t local_size = 256;
    size_t global_size = ceil(work_size / (float)local_size) * local_size;
    // size_t global_size = imageWidth * imageHeight;
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
    // print_error_message(status,program,device,"clEnqueueNDRangeKernel");

    // Step4: Read Back Result 
    status = clEnqueueReadBuffer(queue, data_output, CL_TRUE, 0, sizeof(float) * imageHeight * imageWidth, outputImage, 0, 0, 0);
    clReleaseCommandQueue(queue);
    clReleaseMemObject(data_input);
    clReleaseMemObject(data_output);
    clReleaseKernel(kernel);
    // print_error_message(status,program,device,"clEnqueueReadBuffer");

}
