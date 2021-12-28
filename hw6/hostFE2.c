#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void print_error_message(cl_int status, cl_program* program, cl_device_id* device, char *which ) {
    if (status != CL_SUCCESS)
    {
        size_t len;
        char buffer[8 * 1024];

        printf("Error: Failed to %s !!!!\n", which);
        clGetProgramBuildInfo(*program, *device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
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
    cl_image_format _format;
    _format.image_channel_order = CL_R;
    _format.image_channel_data_type = CL_FLOAT;


    // create buffer on device
    cl_mem data_filter = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(float) * filterSize, NULL, &status);
    // print_error_message(status,program,device,"clCreateBuffer on data_filter");
    cl_mem data_input = clCreateImage2D(*context, CL_MEM_READ_ONLY, &_format, imageWidth, imageHeight, 0, NULL, &status);
    // print_error_message(status,program,device,"clCreateImage2D on data_input");
    cl_mem data_output = clCreateImage2D(*context, CL_MEM_READ_WRITE, &_format, imageWidth, imageHeight, 0, NULL, &status);
    // print_error_message(status,program,device,"clCreateImage2D on data_output");


    // Transfer input data to the device    
    status = clEnqueueWriteBuffer(queue, data_filter, CL_FALSE, 0, filterSize * sizeof(float), filter, 0, NULL, NULL); // blocking_read=CL_TRUE
    // print_error_message(status,program,device,"clEnqueueWriteBuffer");
    
    size_t origin[3] = { 0,0,0 };
    size_t region[3] = { imageWidth,imageHeight,1 };
    status = clEnqueueWriteImage(queue, data_input, CL_FALSE, origin, region, 0, 0, inputImage, 0, NULL, NULL);
    // print_error_message(status,program,device,"clEnqueueWriteImage");



    // Step2: Build Program, Select Kernel
    cl_kernel kernel = clCreateKernel(*program, "convolution", &status);
    // print_error_message(status,program,device,"clCreateKernel");


    // Step3: Set Arguments, Enqueue Kernel 
    cl_sampler sampler = clCreateSampler(*context, CL_FALSE, CL_ADDRESS_CLAMP, CL_FILTER_NEAREST, &status);
    // print_error_message(status,program,device,"clCreateSampler");

    //     status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &data_input);
    // status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &data_output);
    // status |= clSetKernelArg(kernel, 2, sizeof(int), &imageHeight);
    // status |= clSetKernelArg(kernel, 3, sizeof(int), &imageWidth);
    // status |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &data_filter);
    // status |= clSetKernelArg(kernel, 5, sizeof(int), &filterWidth);
    // status |= clSetKernelArg(kernel, 6, sizeof(cl_sampler), &sampler);

    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &data_filter);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &data_input);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &data_output);
    status |= clSetKernelArg(kernel, 3, sizeof(int), &filterWidth);
    status |= clSetKernelArg(kernel, 4, sizeof(int), &imageWidth);
    status |= clSetKernelArg(kernel, 5, sizeof(int), &imageHeight);
    status |= clSetKernelArg(kernel, 6, sizeof(cl_sampler), &sampler);
    // print_error_message(status,program,device,"clSetKernelArg");

    size_t globalws[2] = { imageWidth, imageHeight };//
    status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalws, NULL, 0, NULL, NULL);
    // print_error_message(status,program,device,"clEnqueueNDRangeKernel");

    // Step4: Read Back Result (clEnqueueReadImage)
    status = status = clEnqueueReadImage(queue, data_output, CL_TRUE, origin, region, 0, 0, outputImage, 0, NULL, NULL);
    // print_error_message(status,program,device,"clEnqueueReadImage");

}


