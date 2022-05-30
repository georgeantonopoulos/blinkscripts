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

    double sat = srcPixel.y;
    double  part, minOverMax= 1.0 - sat ;

    // Calculate Hue & Saturation
    float red;
    float green;
    float blue;

    if (minOverMax > 0.)
    {
      if (srcPixel.x < 1. / 6.)
      { //  R>G>B
        srcPixel.x = 6. * (srcPixel.x - 0. / 6.);
        part = 1. + srcPixel.x * (1. / minOverMax - 1.);
        blue = srcPixel.z / sqrt(p.x / minOverMax / minOverMax + p.y * part * part + p.z);
        red = (blue) / minOverMax;
        green = (blue) + srcPixel.x * ((red) - (blue));
      }
      else if (srcPixel.x < 2. / 6.)
      { //  G>R>B
        srcPixel.x = 6. * (-srcPixel.x + 2. / 6.);
        part = 1. + srcPixel.x * (1. / minOverMax - 1.);
        blue = srcPixel.z / sqrt(p.y / minOverMax / minOverMax + p.x * part * part + p.z);
        green = (blue) / minOverMax;
        red = (blue) + srcPixel.x * ((green) - (blue));
      }
      else if (srcPixel.x < 3. / 6.)
      { //  G>B>R
        srcPixel.x = 6. * (srcPixel.x - 2. / 6.);
        part = 1. + srcPixel.x * (1. / minOverMax - 1.);
        red = srcPixel.z / sqrt(p.y / minOverMax / minOverMax + p.z * part * part + p.x);
        green = (red) / minOverMax;
        blue = (red) + srcPixel.x * ((green) - (red));
      }
      else if (srcPixel.x < 4. / 6.)
      { //  B>G>R
        srcPixel.x = 6. * (-srcPixel.x + 4. / 6.);
        part = 1. + srcPixel.x * (1. / minOverMax - 1.);
        red = srcPixel.z / sqrt(p.z / minOverMax / minOverMax + p.y * part * part + p.x);
        blue = (red) / minOverMax;
        green = (red) + srcPixel.x * ((blue) - (red));
      }
      else if (srcPixel.x < 5. / 6.)
      { //  B>R>G
        srcPixel.x = 6. * (srcPixel.x - 4. / 6.);
        part = 1. + srcPixel.x * (1. / minOverMax - 1.);
        green = srcPixel.z / sqrt(p.z / minOverMax / minOverMax + p.x * part * part + p.y);
        blue = (green) / minOverMax;
        red = (green) + srcPixel.x * ((blue) - (green));
      }
      else
      { //  R>B>G
        srcPixel.x = 6. * (-srcPixel.x + 6. / 6.);
        part = 1. + srcPixel.x * (1. / minOverMax - 1.);
        green = srcPixel.z / sqrt(p.x / minOverMax / minOverMax + p.z * part * part + p.y);
        red = (green) / minOverMax;
        blue = (green) + srcPixel.x * ((red) - (green));
      }
    }
    else
    {
      if (srcPixel.x < 1. / 6.)
      { //  R>G>B
        srcPixel.x = 6. * (srcPixel.x - 0. / 6.);
        red = sqrt(srcPixel.z * srcPixel.z / (p.x + p.y * srcPixel.x * srcPixel.x));
        green = (red) * srcPixel.x;
        blue = 0.;
      }
      else if (srcPixel.x < 2. / 6.)
      { //  G>R>B
        srcPixel.x = 6. * (-srcPixel.x + 2. / 6.);
        green = sqrt(srcPixel.z * srcPixel.z / (p.y + p.x * srcPixel.x * srcPixel.x));
        red = (green) * srcPixel.x;
        blue = 0.;
      }
      else if (srcPixel.x < 3. / 6.)
      { //  G>B>R
        srcPixel.x = 6. * (srcPixel.x - 2. / 6.);
        green = sqrt(srcPixel.z * srcPixel.z / (p.y + p.z * srcPixel.x * srcPixel.x));
        blue = (green) * srcPixel.x;
        red = 0.;
      }
      else if (srcPixel.x < 4. / 6.)
      { //  B>G>R
        srcPixel.x = 6. * (-srcPixel.x + 4. / 6.);
        blue = sqrt(srcPixel.z * srcPixel.z / (p.z + p.y * srcPixel.x * srcPixel.x));
        green = (blue) * srcPixel.x;
        red = 0.;
      }
      else if (srcPixel.x < 5. / 6.)
      { //  B>R>G
        srcPixel.x = 6. * (srcPixel.x - 4. / 6.);
        blue = sqrt(srcPixel.z * srcPixel.z / (p.z + p.x * srcPixel.x * srcPixel.x));
        red = (blue) * srcPixel.x;
        green = 0.;
      }
      else
      { //  R>B>G
        srcPixel.x = 6. * (-srcPixel.x + 6. / 6.);
        red = sqrt(srcPixel.z * srcPixel.z / (p.x + p.z * srcPixel.x * srcPixel.x));
        blue = (red) * srcPixel.x;
        green = 0.;
      }
    }
  

    // Calculate srcPixel.z
    float luma = sqrt(srcPixel.x * srcPixel.x * p.x + srcPixel.y * srcPixel.y * p.y + srcPixel.z * srcPixel.z * p.z);

    // Convert to HSP space
    //float3 saturatedPixel = (srcPixel - luma) * saturation + luma;

    // Write the result to the output image
    dst() = float4(red, green, blue, input.w);
  }
};
