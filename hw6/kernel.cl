__kernel void convolution(
    __global  float* filter,
    __global  float* input,
    __global float* output,
    int filterWidth,
    int imageWidth,
    int imageHeight) 
{
    int i = get_global_id(0) / imageWidth;
    int j = get_global_id(0) % imageWidth;
    int halffilterSize = filterWidth / 2;
    float sum=0;
    for (int k = -halffilterSize; k <= halffilterSize; k++)
    {
        for (int l = -halffilterSize; l <= halffilterSize; l++)
        {
            if (i + k >= 0 && i + k < imageHeight &&
                j + l >= 0 && j + l < imageWidth)
            {
                sum += input[(i + k) * imageWidth + j + l] *
                        filter[(k + halffilterSize) * filterWidth +
                                l + halffilterSize];
            }
        }
    }
    output[i * imageWidth + j] = sum;
    
}