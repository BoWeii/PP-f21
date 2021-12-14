#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

__device__ int mandel(float c_re, float c_im, int count)
{
    float z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i)
    {
        if (z_re * z_re + z_im * z_im > 4.f)
            break;

        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }
    return i;
}

__global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int resX, int resY, int *img, int maxIterations, int pitch)
{
    // To avoid error caused by the floating number, use the following pseudo code
    //
    int thisX = blockIdx.x * blockDim.x + threadIdx.x; // for i
    int thisY = blockIdx.y * blockDim.y + threadIdx.y; // for j

    if (thisX >= resX) //unuse position by pitching
        return;

    float x = lowerX + thisX * stepX; //  float x = x0 + i * dx;
    float y = lowerY + thisY * stepY; //  float y = y0 + j * dy;

    int index = thisY * pitch + thisX;
    img[index] = mandel(x, y, maxIterations);
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE(float upperX, float upperY, float lowerX, float lowerY, int *img, int resX, int resY, int maxIterations)
{
    //__host__ for cpu call hostFE(x1, y1, x0, y0, output, width, height, maxIterations);
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    size_t pitch_in_byte;
    int *host_res;
    int *kernel_res;

    cudaMallocPitch((void **)&kernel_res, &pitch_in_byte, sizeof(int) * resX, resY);
    cudaHostAlloc(&host_res, resY * pitch_in_byte, cudaHostAllocMapped);

    int pitch = pitch_in_byte / sizeof(int);

    dim3 threadPerBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 numblock(resX / threadPerBlock.x, resY / threadPerBlock.y);
    // cudaHostGetDevicePointer( &kernel_res, host_res, 0 );

    mandelKernel<<<numblock, threadPerBlock>>>(lowerX, lowerY, stepX, stepY, resX, resY, kernel_res, maxIterations, pitch);
    
    cudaMemcpy(host_res, kernel_res, resY * pitch_in_byte, cudaMemcpyDeviceToHost);
    for (int j = 0; j < resY; j++)
    {
        for (int i = 0; i < resX; i++)
        {
            img[j * resX + i] = host_res[j * pitch + i];
        }
    }
    cudaFreeHost(host_res);
    cudaFree(kernel_res);
}
