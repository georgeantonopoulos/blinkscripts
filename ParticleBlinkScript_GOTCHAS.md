# ParticleBlinkScript Development Gotchas & Best Practices

## 🚨 Critical Issues to Avoid

### 1. **NO .xyz Swizzling Support**
**❌ WRONG:**
```cpp
float3 pos = p_position().xyz;  // Compilation error!
```

**✅ CORRECT:**
```cpp
float4 pos4 = p_position();
float3 pos = float3(pos4.x, pos4.y, pos4.z);
// OR better yet - use individual components:
float pos_x = pos4.x;
float pos_y = pos4.y; 
float pos_z = pos4.z;
```

### 2. **Strict Type Conversion Between float3 ↔ float4**
**❌ WRONG:**
```cpp
float3 velocity = some_calculation();
p_velocity() = velocity;  // Type mismatch error!
```

**✅ CORRECT:**
```cpp
float vel_x = some_calculation_x();
float vel_y = some_calculation_y();
float vel_z = some_calculation_z();
p_velocity() = float4(vel_x, vel_y, vel_z, 0.0f);
```

### 3. **Particle Attributes Are Always float4 Images**
All particle attributes (`p_position`, `p_velocity`, `p_color`, etc.) are stored as `Image<eReadWrite>` with float4 values, even if you only need 1-3 components.

### 4. **No Direct Vector Operations on Mixed Types**
**❌ WRONG:**
```cpp
float3 forces = float3(0.0f);
forces += turbulence;  // If turbulence is computed differently
```

**✅ CORRECT:**
```cpp
float force_x = 0.0f;
float force_y = 0.0f; 
float force_z = 0.0f;
force_x += turb_x;
force_y += turb_y;
force_z += turb_z;
```

## 📝 Required Kernel Structure

### Correct Template:
```cpp
kernel YourKernelName : ImageComputationKernel<ePixelWise> {
    // Particle attributes as Images with p_ prefix
    Image<eReadWrite> p_position;
    Image<eReadWrite> p_velocity;
    Image<eReadWrite> p_color;
    Image<eReadWrite> p_size;
    Image<eReadWrite> p_age;
    
    param:
        float your_parameter;
        float _dt;  // Always include delta time
        
    void define() {
        defineParam(your_parameter, "Your Parameter", 1.0f);
        defineParam(_dt, "_dt", 1.0f);
    }
    
    void process() {
        // Your particle logic here
    }
};
```

## 🔧 Best Practices

### 1. **Individual Component Access**
Always work with individual float components rather than vector types:
```cpp
// Read particle data
float4 pos4 = p_position();
float pos_x = pos4.x, pos_y = pos4.y, pos_z = pos4.z;

// Process individually 
pos_x += velocity_x * dt;
pos_y += velocity_y * dt;
pos_z += velocity_z * dt;

// Write back
p_position() = float4(pos_x, pos_y, pos_z, 1.0f);
```

### 2. **Noise Function Arguments**
When calling noise functions, always provide complete float3 constructors:
```cpp
// Instead of incomplete vectors
float noise = noise3D(pos * scale + float3(time));

// Use complete vectors
float noise = noise3D(float3(pos_x, pos_y, pos_z) * scale + float3(time, time, time));
```

### 3. **Parameter Naming**
- Particle attributes: Use `p_` prefix (`p_position`, `p_velocity`)
- Always include `_dt` parameter for time-based calculations
- Use descriptive parameter names in `define()`

### 4. **Memory Access Patterns**
- Particle attributes are accessed per-pixel, not per-particle
- Each "pixel" represents one particle's data
- No random access to other particles' data in basic kernels

## ⚡ Performance Tips

1. **Minimize float3 Construction:**
   Work with individual components when possible

2. **Noise Function Calls:**
   Cache noise results rather than calling multiple times with similar inputs

3. **Mathematical Operations:**
   Use built-in functions (`pow()`, `sin()`, `sqrt()`) rather than manual implementations

## 🐛 Common Compilation Errors

| Error Message | Cause | Solution |
|---------------|-------|----------|
| `no member named 'xyz'` | Swizzling not supported | Use `.x`, `.y`, `.z` individually |
| `no viable conversion from Vec<float,3> to Vec<float,4>` | Type mismatch | Explicit component-wise conversion |
| `No images declared in kernel` | Wrong kernel type | Use `ImageComputationKernel<ePixelWise>` |
| `call to 'abs' is ambiguous`   | Ambiguous overload between integer and float abs | Use `fabs()` for floats |
| `no viable overloaded '='`     | Assignment mismatch due to incorrect vector size | Ensure image writes use the correct vector size (e.g. position/velocity need `float3`, color needs `float4`, size often a single `float`) |

## 📚 Working Example Structure

See `