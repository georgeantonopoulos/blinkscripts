kernel SaturationKernel : ImageComputationKernel<ePixelWise>
{
  Image<eRead, eAccessPoint, eEdgeClamped> src; // the input image
  Image<eWrite> dst; // the output image

  param:
    //float saturation; // This parameter is made available to the user.

  local:
    float3 p;  // This local variable is not exposed to the user.

  // In define(), parameters can be given labels and default values.
  void define() {
  }

  // The init() function is run before any calls to process().
  // Local variables can be initialized here.
  void init() {
    // Initialise coefficients according to rec. 709 standard.
    p.x = 0.299f;
    p.y = 0.587f;
    p.z = 0.114f;
  }

  void process() {
    // Read the input image
    SampleType(src) input = src();

    // Isolate the RGB components
    float3 srcPixel(input.x, input.y, input.z);

    // Calculate Hue & Saturation
    float hue;
    float sat;
    
    if (srcPixel.x==srcPixel.y && srcPixel.x==srcPixel.z) {
        hue=0.0;
        sat=0.0;
        return; 
        }
    
    if (srcPixel.x>=srcPixel.y && srcPixel.x>=srcPixel.z) {   //  srcPixel.x is largest
        if (srcPixel.z>=srcPixel.y) {
            hue=6.0/6.0-1.0/6.0*(srcPixel.z-srcPixel.y)/(srcPixel.x-srcPixel.y);
            sat=1.0-srcPixel.y/srcPixel.x; }
        else {
            hue=0.0/6.0+1.0/6.0*(srcPixel.y-srcPixel.z)/(srcPixel.x-srcPixel.z);
            sat=1.0-srcPixel.z/srcPixel.x; }
            }
    else if (srcPixel.y>=srcPixel.x && srcPixel.y>=srcPixel.z) {   //  srcPixel.y is largest
        if (srcPixel.x>=srcPixel.z) {
            hue=2.0/6.0-1.0/6.0*(srcPixel.x-srcPixel.z)/(srcPixel.y-srcPixel.z);
            sat=1.0-srcPixel.z/srcPixel.y; }
        else {
            hue=2.0/6.0+1.0/6.0*(srcPixel.z-srcPixel.x)/(srcPixel.y-srcPixel.x);
            sat=1.0-srcPixel.x/srcPixel.y; }
        }
    else {   //  srcPixel.z is largest
        if (srcPixel.y>=srcPixel.x) {
            hue=4.0/6.0-1.0/6.0*(srcPixel.y-srcPixel.x)/(srcPixel.z-srcPixel.x);
            sat=1.0-srcPixel.x/srcPixel.z; }
        else {
            hue=4.0/6.0+1.0/6.0*(srcPixel.x-srcPixel.y)/(srcPixel.z-srcPixel.y);
            sat=1.0-srcPixel.y/srcPixel.z; }
        }
         
    // Calculate P
    float luma = sqrt(srcPixel.x * srcPixel.x * p.x + srcPixel.y * srcPixel.y * p.y + srcPixel.z * srcPixel.z * p.z);

    // Convert to HSP space
    //float3 saturatedPixel = (srcPixel - luma) * saturation + luma;

    // Write the result to the output image
    dst() = float4(hue, sat, luma, input.w);
  }
};
