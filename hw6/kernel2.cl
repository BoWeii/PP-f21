__kernel void convolution(
    __constant  float* filter,
    __read_only image2d_t input,
    __read_write image2d_t output,
      int filterWidth,
      int imageWidth,
      int imageHeight,
     sampler_t sampler) 
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    int z=0;
    int halffilterSize = filterWidth / 2;
    float4 sum={0,0,0,0};
    int2 coord;
    // Apply the filter to the neighborhood
    for (int k = -halffilterSize; k <= halffilterSize; k++)
    {
        coord.y= j + k;
        for (int l = -halffilterSize; l <= halffilterSize; l++)
        {
            coord.x = i + l;
            float4 temp = read_imagef(input,sampler,coord);
            sum.x += temp.x * filter[z++];
        }
    } 
    if(j<imageHeight && i<imageWidth){
    coord.x=i;
    coord.y=j;
    write_imagef(output, coord, sum);

    }


}