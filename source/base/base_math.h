/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
#ifndef BASE_MATH_H
#define BASE_MATH_H

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
    struct
    {
        F32 x;
        F32 y;
        F32 z;
    };
    F32 v[3];
};

typedef union Vec3F64 Vec3F64;
union Vec3F64
{
    struct
    {
        F64 x;
        F64 y;
        F64 z;
    };
    F64 v[3];
};

// Scalar Math Ops
#define abs_s64(v)                      (S64)llabs(v)

#define sqrt_f32(v)                     sqrtf(v)
#define cbrt_f32(v)                     cbrtf(v)
#define mod_f32(a, b)                   fmodf((a), (b))
#define pow_f32(b, e)                   powf((b), (e))
#define ceil_f32(v)                     ceilf(v)
#define floor_f32(v)                    floorf(v)
#define round_f32(v)                    roundf(v)
#define abs_f32(v)                      fabsf(v)
#define radians_from_turns_f32(v)       ((v)*(2*3.1415926535897f))
#define turns_from_radians_f32(v)       ((v)/(2*3.1415926535897f))
#define degrees_from_turns_f32(v)       ((v)*360.f)
#define turns_from_degrees_f32(v)       ((v)/360.f)
#define degrees_from_radians_f32(v)     (degrees_from_turns_f32(turns_from_radians_f32(v)))
#define radians_from_degrees_f32(v)     (radians_from_turns_f32(turns_from_degrees_f32(v)))
#define sin_f32(v)                      sinf(radians_from_turns_f32(v))
#define cos_f32(v)                      cosf(radians_from_turns_f32(v))
#define tan_f32(v)                      tanf(radians_from_turns_f32(v))

#define sqrt_f64(v)                     sqrt(v)
#define cbrt_f64(v)                     cbrt(v)
#define mod_f64(a, b)                   fmod((a), (b))
#define pow_f64(b, e)                   pow((b), (e))
#define ceil_f64(v)                     ceil(v)
#define floor_f64(v)                    floor(v)
#define round_f64(v)                    round(v)
#define abs_f64(v)                      fabs(v)
#define radians_from_turns_f64(v)       ((v)*(2*3.1415926535897))
#define turns_from_radians_f64(v)       ((v)/(2*3.1415926535897))
#define degrees_from_turns_f64(v)       ((v)*360.0)
#define turns_from_degrees_f64(v)       ((v)/360.0)
#define degrees_from_radians_f64(v)     (degrees_from_turns_f64(turns_from_radians_f64(v)))
#define radians_from_degrees_f64(v)     (radians_from_turns_f64(turns_from_degrees_f64(v)))
#define sin_f64(v)                      sin(radians_from_turns_f64(v))
#define cos_f64(v)                      cos(radians_from_turns_f64(v))
#define tan_f64(v)                      tan(radians_from_turns_f64(v))

#define v3f32(x, y, z)                  vec_3f32((x), (y), (z))

internal_function Vec3F32    vec_3f32(F32 x, F32 y, F32 z);
internal_function Vec3F32    add_3f32(Vec3F32 a, Vec3F32 b);
internal_function Vec3F32    sub_3f32(Vec3F32 a, Vec3F32 b);
internal_function Vec3F32    mul_3f32(Vec3F32 a, Vec3F32 b);
internal_function Vec3F32    div_3f32(Vec3F32 a, Vec3F32 b);
internal_function Vec3F32    scale_3f32(Vec3F32 v, F32 s);
internal_function F32        dot_3f32(Vec3F32 a, Vec3F32 b);
internal_function F32        length_squared_3f32(Vec3F32 v);
internal_function F32        length_3f32(Vec3F32 v);
internal_function Vec3F32    normalize_3f32(Vec3F32 v);
internal_function Vec3F32    cross_3f32(Vec3F32 a, Vec3F32 b);

#endif // BASE_MATH_H
