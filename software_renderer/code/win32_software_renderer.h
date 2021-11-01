/* date = January 11th 2021 11:31 am */

#ifndef WIN32_SOFTWARE_RENDERER_H
#define WIN32_SOFTWARE_RENDERER_H

#define internal static;
#define local_persist static;
#define global_variable static;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int32_t bool32;

typedef float float32;
typedef double float64;

#define DEBUG_RENDERING 1

#define ASSERT(expression) if(!(expression)) { *(int *)0 = 0; }
#define ABS(x) if(*x < 0) { *x = -(*x); }
#define SWAP_BIT(var, pos, bit) (((var) & (~(1<<pos))) | ((bit)&(1<<pos)))
// TODO(Ajeej): Use different technique in order to not have
// to specify a type
#define SWAP(a, b, type) \
type *temp = a; \
*a = *b; \
*b = *temp; \

#define SIZEOF_ARRAY(array) (sizeof(array) / sizeof((array)[0]))
#define ArrayCount(x) (sizeof(x) / sizeof((x)[0]))

typedef struct {
    uint32 x, y;
} uvector2_t;


typedef struct {
    int32 x, y;
} vector2_t;


typedef struct {
    float32 x, y;
} fvector2_t;

typedef struct {
    uint32 x, y, z;
} uvector3_t;


typedef struct {
    int32 x, y, z;
} vector3_t;


typedef struct {
    float32 x, y, z;
} fvector3_t;

typedef struct {
    // TODO(Ajeej): Implement array versions of these in a union
    uint32 x, y, z, w;
} uvector4_t;


typedef struct {
    int32 x, y, z, w;
} vector4_t;


typedef struct {
    float32 x, y, z, w;
} fvector4_t;

typedef struct {
    // TODO(Ajeej): Fix the int32 issues by switching to uint32
    uint32 width, height;
    uint32 color_buffer_size;
    uint8 *color_buffer;
} framebuffer_t;

typedef struct {
    fvector2_t pos;
    bool32 attached;
    uint32 radius;
} point_t;

inline float32 
inner_product(fvector2_t v1, fvector2_t v2)
{
    float32 result;
    result = v1.x * v2.x + v1.y * v2.y;
    return result;
}

inline float32 
inner_product(v2_t v1, v2_t v2)
{
    float32 result;
    result = v1.x * v2.x + v1.y * v2.y;
    return result;
}

fvector2_t operator-(fvector2_t v1, fvector2_t v2)
{
    fvector2_t result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

fvector2_t operator+(fvector2_t v1, fvector2_t v2)
{
    fvector2_t result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

fvector2_t operator*(fvector2_t v, float32 x)
{
    fvector2_t result;
    result.x = v.x * x;
    result.y = v.y * x;
    return result;
}

bool32 operator==(fvector2_t v1, fvector2_t v2)
{
    return (v1.x == v2.y && v1.y == v2.y);
}

inline fvector2_t 
fv2(vector2_t v)
{
    fvector2_t result;
    result.x = (float32)v.x;
    result.y = (float32)v.y;
    return result;
}

#endif //WIN32_SOFTWARE_RENDERER_H
