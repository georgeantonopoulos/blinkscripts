# The Blink Scripting Bible: A Comprehensive Reference for Nuke

## 1. Introduction to Blink Scripting in Nuke

Foundry's Blink framework presents a powerful avenue for artists and technical directors within Nuke to perform rapid image processing. Conceived as a language with syntax reminiscent of C++, Blink allows users to author code that, through an on-the-fly translation process utilizing the LLVM compiler, can execute across a variety of supported devices—including both the central processing unit (CPU) and the graphics processing unit (GPU). This capability to write code once and deploy it across different hardware platforms underscores one of Blink's fundamental strengths. From an artist's perspective, Blink serves as a critical bridge, enabling direct manipulation of image pixels through a programmatic interface.

Blink occupies a unique and valuable position within Nuke's compositing environment. While Python scripting is predominantly used for navigating and manipulating the node graph, automating tasks, and managing file paths, it typically does not involve direct pixel-level operations. Conversely, Nuke's expression node excels at immediate changes to pixel values but lacks the capacity for complex programmatic constructs such as looping or the creation of numerous interconnected variables. Blink steps into this gap, offering a "C++-like" language that allows for intricate pixel manipulations with the added benefit of performance optimization through GPU acceleration. At its core, Blink operates by executing a "Blink kernel" for every pixel in the image intended as the output.

The ability of Blink to leverage either the CPU or GPU for parallel processing on image data provides a significant advantage in terms of speed, particularly for operations that require intensive pixel-level calculations. This performance benefit allows artists to create complex effects and custom tools that might be impractical or too slow using traditional Nuke nodes or Python scripting alone. The "C++-like" nature of the language suggests that individuals with prior experience in C++ will find some familiarity in Blink's syntax and structure. However, it is important to note that Blink is not a direct implementation of C++ and includes specific keywords, a unique kernel structure, and specialized functions tailored for image processing within Nuke. Therefore, a dedicated understanding of Blink's specific features and syntax is essential for effective utilization.

## 2. Setting Up Your Blink Environment

To begin working with Blink scripts in Nuke, the primary entry point is the BlinkScript node. Upon creation, the default BlinkScript node loads with an example kernel that demonstrates a saturation adjustment. This provides a functional starting point for users to examine and understand the basic structure of a Blink script.

The BlinkScript node interface is organized into several key tabs that provide control over various aspects of the Blink kernel:

- **BlinkScript Tab**:  
  - Contains the Kernel File section for loading and saving Blink kernels (using the `.rpp` extension).  
  - Offers buttons for Load, Save, Clear, and Recompile (to trigger kernel code compilation).  
  - Features the Kernel Source text editor for writing, editing, and viewing the Blink kernel code.  
  - Includes the Bake button to cache Library Files for shareability and the Make Live button to re-enable editing.  
  - The Protect button encrypts the node, hiding kernel editing options.

- **Kernel Parameters Tab**:  
  - Exposes parameters defined in the Blink kernel as interactive knobs in the Nuke interface.  
  - Displays the Local GPU being used when the "Use GPU if available" option is enabled, or shows "Not available" based on system configuration.  
  - Contains options such as Vectorize on CPU, Safety Rails, Input Group, Output Channels, and Format settings.

- **Library Files Tab**:  
  - Provides a mechanism for including external code into the Blink kernel.  
  - Allows users to specify file paths to `.blib` files, which can contain helper functions and macros for reusability.

Understanding these components is the foundational first step in harnessing the power of Blink within Nuke.

## 3. Core Concepts of Blink Scripting

A Blink kernel—the fundamental unit of code in the Blink framework—shares structural similarities with a class definition in C++. A typical Blink kernel definition begins with the `kernel` keyword, followed by the kernel's name and an inheritance specification from the `ImageComputationKernel` class. This inheritance requires a template parameter that defines the kernel granularity, indicating whether the kernel will operate on a per-pixel basis or a per-component basis. For example, a kernel intended to process each pixel as a whole would be defined as:

```cpp
kernel MyKernel : ImageComputationKernel<ePixelWise> {
  // ...
};
```

Within the body of the kernel definition, at least one image specification is required. This is done using the `Image` keyword with template arguments that define the read specification (e.g., `eRead` or `eWrite`), the access pattern (e.g., `eAccessPoint`, `eAccessRanged1D`, `eAccessRanged2D`, or `eAccessRandom`), and the edge method (e.g., `eEdgeClamped`, `eEdgeConstant`, or `eEdgeNone`). For example:

```cpp
Image<eRead, eAccessPoint, eEdgeClamped> src;
Image<eWrite> dst;
```

The core logic of the Blink kernel resides within a special method called `process()`. This method is executed for each point in the iteration space (typically every pixel of the output image). The method signature varies depending on the kernel granularity:

- For a **componentwise kernel**, the signature might be:  
  ```cpp
  void process()
  ```
- For a **pixelwise kernel**, a common signature is:  
  ```cpp
  void process(int2 pos)
  ```  
  which provides the x and y coordinates of the current pixel being processed. Alternatively, pixel coordinates may be implicitly managed.

The choice between pixelwise (`ePixelWise`) and componentwise (`eComponentWise`) granularity determines how the `process()` method is invoked and how pixel data is accessed.

Image access within a Blink kernel is controlled by the template arguments provided to the `Image` specification. The read specification is either `eRead` or `eWrite`, and the access pattern determines how pixels are accessed. The edge method controls behavior for out-of-bound pixel accesses.

## 4. Blink Syntax and Language Elements

Blink scripting supports a variety of variable types, control flow structures, and operators.

### Variable Types

- **Scalar Types**:  
  - `int` for integers  
  - `float` for floating-point numbers (requires an `f` suffix, e.g., `1.0f`)  
  - `bool` for Boolean values

- **Vector Types**:  
  - Integer vectors: `int1`, `int2`, `int3`, `int4`  
  - Floating-point vectors: `float1`, `float2`, `float3`, `float4`  
  - Vectors can be accessed via swizzling (e.g., `.x`, `.y`, `.z`, `.w`) or subscripting.

- **Matrix Types**:  
  - `float3x3` for 3×3 matrices  
  - `float4x4` for 4×4 matrices

- **Array Types**:  
  - Fixed-length, zero-indexed arrays of variables of the same type

- **Rectangle Types**:  
  - `recti` for integer-coordinate rectangles  
  - `rectf` for floating-point-coordinate rectangles

### Operators and Control Flow

- **Operators**:  
  Standard arithmetic operators (`+`, `-`, `*`, `/`), assignment operators (`=`, `+=`, etc.), comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`), and logical operators (`&&`, `||`, `!`).

- **Control Flow**:  
  `if`/`else` statements for conditional execution and `for` loops for iterating over sequences or image pixels.

## 5. Key Differences Between Blink and C++

While Blink scripting has a "C++-like" syntax, there are several key differences:

- **Kernel Structure**:  
  Blink enforces a specific organization with Kernel inheritance, Image specifications, and a dedicated `process()` method for image processing logic.

- **Granularity**:  
  The distinction between pixelwise (`ePixelWise`) and componentwise (`eComponentWise`) execution is unique to Blink and is tailored for image processing workflows.

- **Image Access**:  
  Blink uses specialized keywords (like `Image`, `eRead`, `eWrite`) and functions (e.g., `bilinear()`, `sample()`) for image data manipulation, which are not part of standard C++.

- **Built-in Functions**:  
  Blink includes an optimized library of functions for vector and matrix operations, image sampling, and common mathematical operations.

- **Runtime Compilation**:  
  Blink code is compiled at runtime using the LLVM compiler, enabling hardware-specific optimizations. In contrast, standard C++ code is compiled ahead of time.

- **Memory Management**:  
  Memory for image data is managed implicitly in Blink, while in C++ it is the programmer's responsibility.

## 6. Comprehensive Blink Kernel Function Reference

Blink provides a rich set of built-in functions that can be used within a kernel to perform various operations. These functions are broadly categorized as follows:

### Vector Functions
- **dot(vec a, vec b)**: Calculates the dot product of two vectors.  
- **cross(vec3 a, vec3 b)**: Computes the cross product of two 3-component vectors.  
- **length(vec a)**: Returns the magnitude (Euclidean length) of a vector.  
- **normalize(vec a)**: Returns a unit vector in the same direction as the input vector.

### Math Functions
- **Trigonometric Functions**: `sin(a)`, `cos(a)`, `tan(a)`, `asin(a)`, `acos(a)`, `atan(a)`, `atan2(a, b)`  
- **Logarithmic and Exponential Functions**: `exp(a)`, `log(a)`, `log2(a)`, `log10(a)`  
- **Rounding Functions**: `floor(a)`, `ceil(a)`, `round(a)`  
- **Power Functions**: `pow(a, b)`, `sqrt(a)`, `rsqrt(a)`  
- **Absolute Functions**: `fabs(a)`, `abs(a)`  
- **Modulus and Fraction Functions**: `fmod(a, b)`, `modf(a, *b)`  
- **Sign, Min, and Max Functions**: `sign(a)`, `min(a, b)`, `max(a, b)`, `clamp(a, minVal, maxVal)`  
- **Reciprocal Function**: `rcp(a)`

### Atomic Functions (Integer Types Only)
- **atomicAdd(*ptr, val)**: Atomically adds a value to an integer.  
- **atomicInc(*ptr)**: Atomically increments an integer by 1.

### Statistical Function
- **median(data, size)**: Calculates and returns the median value from an array of scalar data.

### Rectangle Functions
- **Constructors**: `rect()`, `rect(x1, y1, x2, y2)`  
- **Operations**: `grow(x, y)`, `inside(x, y)`, `width()`, `height()`

### Image Sampling Functions
- **sample(image, u, v)**: Samples the pixel value from an image at normalized coordinates (u, v).  
- **bilinear(image, u, v)**: Performs bilinear interpolation to sample the pixel value from an image.

## 7. Complete Blink Script Examples

Below are several complete Blink script examples illustrating various tasks:

### 7.1 Basic Pass-Through Kernel

This kernel demonstrates the fundamental structure of a Blink script by simply passing the input image to the output.

```cpp
kernel PassThrough : ImageComputationKernel<ePixelWise> {
  Image<eRead> src;
  Image<eWrite> dst;

  void process() {
    dst() = src();
  }
};
```

### 7.2 Solid Color Kernel

This example generates a solid color output. It includes a parameter to control the color from the Nuke interface.

```cpp
kernel SolidColor : ImageComputationKernel<ePixelWise> {
  Image<eWrite> dst;
  param: float4 color;

  void define() {
    defineParam(color, "Color", float4(1.0f, 0.0f, 0.0f, 1.0f)); // Default to red
  }

  void process() {
    dst() = color;
  }
};
```

### 7.3 Brightness Adjustment Kernel

This kernel adjusts the brightness of an input image by multiplying each pixel's color components by a brightness factor.

```cpp
kernel Brightness : ImageComputationKernel<ePixelWise> {
  Image<eRead> src;
  Image<eWrite> dst;
  param: float brightness;

  void define() {
    defineParam(brightness, "Brightness", 1.0f);
  }

  void process() {
    dst() = src() * brightness;
  }
};
```

### 7.4 Center of Mass Calculation

This advanced example calculates the center of mass of a binary image (where pixels are either black or white) and outputs a white pixel at that center.

```cpp
kernel CenterOfMass : ImageComputationKernel<ePixelWise> {
  Image<eRead, eAccessRandom> src;
  Image<eWrite, eAccessRandom> dst;
  local: float sumX;
  local: float sumY;
  local: int count;

  void init() {
    sumX = 0.0f;
    sumY = 0.0f;
    count = 0;
  }

  void process(int2 pos) {
    if (pos.x == 0 && pos.y == 0) {
      sumX = 0.0f;
      sumY = 0.0f;
      count = 0;
      for (int j = src.bounds.y1; j <= src.bounds.y2; ++j) {
        for (int i = src.bounds.x1; i <= src.bounds.x2; ++i) {
          if (src(i, j).x > 0.0f) {
            sumX += i;
            sumY += j;
            count++;
          }
        }
      }
    }

    if (count > 0 && pos.x == round(sumX / count) && pos.y == round(sumY / count)) {
      dst(pos.x, pos.y) = float4(1.0f);
    } else {
      dst(pos.x, pos.y) = float4(0.0f);
    }
  }
};
```

### 7.5 Drawing a 2D Line

This kernel demonstrates how to draw a simple 2D line by iterating through a number of points and setting the pixel value at specific coordinates.

```cpp
kernel PointLighning : ImageComputationKernel<ePixelWise> {
  Image<eRead, eAccessRandom> format;
  Image<eWrite, eAccessRandom> dst;

  void process(int2 pos) {
    if (pos.x || pos.y) {
      return;
    }
    int number_of_points = 50;
    for (int y = 0; y < number_of_points; y++) {
      dst(0, y) = float4(1.0f);
    }
  }
};
```

## 8. Debugging Blink Scripts

Debugging Blink scripts in Nuke can sometimes be challenging because the error messages provided by Nuke when a kernel fails to compile can be generic (e.g., "Error compiling kernel"). Here are some strategies:

- **Safety Rails**: Enable the Safety Rails feature in the Kernel Parameters tab to generate additional code that checks for out-of-bounds memory accesses.
- **Conceptual printf-style Debugging**: Write intermediate values to specific pixels (for example, the red channel) to visualize data and identify issues.
- **Type Casting**: Use explicit type casting to avoid implicit type conversion errors.
- **Testing Across Versions**: Sometimes testing the script in different Nuke versions can yield more specific error messages.
- **Third-Party Tools**: Consider using tools like KnobScripter, which can provide an enhanced environment with syntax highlighting and better error feedback.
- **Simplify and Isolate**: Comment out sections of code systematically (e.g., using `/* ... */`) to isolate the problematic lines. Start with complex calculations or loops.
- **Check Function Calls**: Ensure built-in functions like `abs`/`fabs`, `round`, `min`/`max`, `pow`, `dot`, `length`, etc., are used with the correct argument types (int vs. float).
- **Check Image Access**: Remember that `eAccessPoint` uses `src()`, while `eAccessRandom` requires `src(x, y)`.
- **Potential `clamp()` Issue**: If encountering generic compile errors after adding complex logic, try temporarily removing the `clamp()` function from the final `dst()` assignment (e.g., `dst() = finalColor;` instead of `dst() = clamp(finalColor, 0.0f, 1.0f);`). If it then compiles, the issue might be a subtle type interaction with `clamp()` in that specific context.

## Appendix: Blink Script Command and Function Summary

### Table 1: Blink Kernel Function Reference

| Function Name | Category      | Description                                                        | Syntax Example                                      |
|---------------|---------------|--------------------------------------------------------------------|-----------------------------------------------------|
| dot           | Vector        | Returns the dot product of two vectors.                          | `float result = dot(v1, v2);`                         |
| cross         | Vector        | Returns the cross product of two 3-component vectors.              | `float3 result = cross(v1, v2);`                      |
| length        | Vector        | Returns the magnitude of a vector.                                 | `float len = length(v);`                              |
| normalize     | Vector        | Returns a unit vector in the same direction as the input vector.   | `float3 unitVec = normalize(v);`                    |
| sin           | Math          | Returns the sine of an angle (in radians).                         | `float s = sin(angle);`                               |
| cos           | Math          | Returns the cosine of an angle (in radians).                       | `float c = cos(angle);`                               |
| tan           | Math          | Returns the tangent of an angle (in radians).                      | `float t = tan(angle);`                               |
| asin          | Math          | Returns the arcsine of a value (in radians).                       | `float a = asin(value);`                              |
| acos          | Math          | Returns the arccosine of a value (in radians).                     | `float a = acos(value);`                              |
| atan          | Math          | Returns the arctangent of a value (in radians).                    | `float a = atan(value);`                              |
| atan2         | Math          | Returns the arctangent of y/x with quadrant determination.         | `float a = atan2(y, x);`                              |
| exp           | Math          | Returns the exponential of a value (e^a).                          | `float e = exp(value);`                               |
| log           | Math          | Returns the natural logarithm of a value.                          | `float l = log(value);`                               |
| log2          | Math          | Returns the base-2 logarithm of a value.                           | `float l = log2(value);`                              |
| log10         | Math          | Returns the base-10 logarithm of a value.                          | `float l = log10(value);`                             |
| floor         | Math          | Returns the largest integer not greater than the value.            | `int f = floor(value);`                               |
| ceil          | Math          | Returns the smallest integer not less than the value.              | `int c = ceil(value);`                                |
| round         | Math          | Rounds the value to the nearest integer.                           | `int r = round(value);`                               |
| pow           | Math          | Raises a base to the power of an exponent.                          | `float p = pow(base, exponent);`                      |
| sqrt          | Math          | Returns the square root of a value.                                | `float s = sqrt(value);`                              |
| rsqrt         | Math          | Returns the reciprocal of the square root (1/√a).                    | `float rs = rsqrt(value);`                            |
| fabs          | Math          | Returns the absolute value for floating-point numbers.             | `float absVal = fabs(value);`                         |
| abs           | Math          | Returns the absolute value for integers.                           | `int absVal = abs(value);`                            |
| fmod          | Math          | Returns the floating-point remainder of a division.                | `float rem = fmod(a, b);`                             |
| modf          | Math          | Splits a floating-point number into integer and fractional parts.  | `float frac = modf(value, &intPart);`                 |
| sign          | Math          | Returns the sign of a number (-1, 0, or 1).                          | `float s = sign(value);`                              |
| min           | Math          | Returns the smaller of two values.                                 | `float minValue = min(a, b);`                         |
| max           | Math          | Returns the larger of two values.                                  | `float maxValue = max(a, b);`                         |
| clamp         | Math          | Clamps a value within a specified range.                           | `float clampedValue = clamp(value, minVal, maxVal);`   |
| rcp           | Math          | Returns the reciprocal of a value (1/x).                           | `float recValue = rcp(value);`                        |
| atomicAdd     | Atomic (Int)  | Atomically adds a value to an integer at a given memory location.   | `atomicAdd(&intVar, value);`                          |
| atomicInc     | Atomic (Int)  | Atomically increments an integer by 1.                             | `atomicInc(&intVar);`                                 |
| median        | Statistical   | Returns the median value of an array.                              | `float med = median(dataArray, size);`                |

### Table 2: Blink Data Types

| Data Type | Description                                              | Example                                       |
|-----------|----------------------------------------------------------|-----------------------------------------------|
| int       | Integer value.                                           | `int count = 10;`                             |
| float     | Floating-point value (requires 'f' suffix).              | `float value = 3.14159f;`                       |
| bool      | Boolean value (`true` or `false`).                       | `bool isReady = true;`                         |
| int1      | 1-component integer vector.                              | `int1 scalarInt = 5;`                          |
| int2      | 2-component integer vector (e.g., UV coordinates).       | `int2 uv = int2(256, 512);`                     |
| int3      | 3-component integer vector.                              | `int3 vectorInt = int3(1, 2, 3);`               |
| int4      | 4-component integer vector.                              | `int4 rgbaInt = int4(255, 0, 0, 255);`          |
| float1    | 1-component floating-point vector.                       | `float1 scalarFloat = 2.718f;`                  |
| float2    | 2-component floating-point vector.                       | `float2 point = float2(10.0f, 20.0f);`          |
| float3    | 3-component floating-point vector (e.g., RGB color).       | `float3 colorRGB = float3(1.0f, 0.5f, 0.2f);`   |
| float4    | 4-component floating-point vector (e.g., RGBA color).      | `float4 colorRGBA = float4(1.0f, 0.5f, 0.2f, 1.0f);` |
| float3x3  | 3×3 floating-point matrix (e.g., for color transformations).| `float3x3 matrix3 = {1, 0, 0, 0, 1, 0, 0, 0, 1};` |
| float4x4  | 4×4 floating-point matrix (e.g., for 3D transformations).  | `float4x4 matrix4;`                           |
| recti     | Rectangle with integer coordinates.                      | `recti intRect = recti(0, 0, 640, 480);`        |
| rectf     | Rectangle with floating-point coordinates.               | `rectf floatRect = rectf(0.0f, 0.0f, 1.0f, 1.0f);` |
| scalar    | Placeholder for the scalar type corresponding to a vector. | `scalar val = length(float3(1, 2, 3));`         |
| vec       | Placeholder for any of the vector types (int1–4, float1–4).  | `vec v = float2(1.0f, 2.0f);`                   |
| int_type  | Placeholder for any integer scalar or vector type.       | `int_type count = 5;`                           |

## Conclusion

Blink scripting offers a powerful and versatile toolset for VFX artists and compositors working in Nuke. Its "C++-like" syntax, combined with its ability to leverage both CPU and GPU processing, allows for the creation of highly optimized custom image processing effects that extend beyond the capabilities of standard Nuke nodes and Python scripting. By understanding the core concepts of kernel structure, granularity, and image access—as well as the available data types and built-in functions—users can harness Blink to develop innovative solutions for a wide range of creative and technical challenges.

## Works Cited

- [Create Tools With BlinkScript | 01. Introduction to BlinkScript - YouTube](https://www.youtube.com/watch?v=xJZ_9EkdyAI)
- [Blink Reference Guide — Guide to Writing Blink Kernels](https://learn.foundry.com/nuke/developers/90/BlinkKernels/Blink.html)
- [BlinkScript - Foundry Learn](https://learn.foundry.com/nuke/content/reference_guide/other_nodes/blinkscript.html)
- [An Artist's Guide to Blinkscript - YouTube](https://www.youtube.com/watch?v=MZ3BYRqx-B0)
- [Nuke 15.1 | Unleashed Creativity with Blinkscript - YouTube](https://www.youtube.com/watch?v=N-kzKcuV_C4)
- [Nuke 16.0 Adds Multishot Compositing, Updates BlinkScript and ...](https://www.digitalmediaworld.tv/vfx/nuke-16-0-adds-multishot-compositing-updates-blinkscript-and-timelines)
- [Blink 101 - Guillem Ramisa de Soto](http://www.guillemramisadesoto.com/blink-101)
- [Making 3D Lightning in Nuke using Blinkscript - Erwan Leroy](https://erwanleroy.com/making-3d-lightning-in-nuke-using-blinkscript/)
- [The Power Of BlinkScript (Nuke) - YouTube](https://www.youtube.com/watch?v=Wru4IDfV4EU)
- [Variable Types | Guide to using Blink - Foundry Learn](https://learn.foundry.com/nuke/developers/15.1/BlinkUserGuide/BlinkKernelAPIReference/Types.html)
- [Intro to writing Nuke C++ Plugins (a.k.a. the NDK) – Part 1 – Intro & Compiling - Erwan Leroy](https://erwanleroy.com/intro-to-writing-nuke-c-plugins-a-k-a-the-ndk-part-1-intro-compiling/)
- [Blinkscript debugging - Foundry Community](https://community.foundry.com/discuss/topic/148062/blinkscript-debugging)
- [Debugging python code IN nuke with Eclipse (step through like a pro!) - Nukepedia](https://www.nukepedia.com/written-tutorials/debugging-python-code-in-nuke-with-eclipse-step-through-like-a-pro)
- [KnobScripter v3.1 User Guide - Adrian Pueyo](https://adrianpueyo.com/KnobScripter_v3.1.pdf)
- [Nuke not printing full Blink script compile error : r/NukeVFX - Reddit](https://www.reddit.com/r/NukeVFX/comments/943e0w/nuke_not_printing_full_blink_script_compile_error/)

---

Feel free to adjust the headings or formatting as needed for your workflow.