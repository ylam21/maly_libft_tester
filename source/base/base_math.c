/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
internal Vec3F32 vec_3f32(F32 x, F32 y, F32 z)
{
    Vec3F32 v = { {x, y, z } };
    return v;
}

internal Vec3F32 add_3f32(Vec3F32 a, Vec3F32 b)
{
    Vec3F32 c = { {a.x+b.x, a.y+b.y, a.z+b.z} };
    return c;
}

internal Vec3F32 sub_3f32(Vec3F32 a, Vec3F32 b)
{
    Vec3F32 c = { {a.x-b.x, a.y-b.y, a.z-b.z} };
    return c;
}

internal Vec3F32 mul_3f32(Vec3F32 a, Vec3F32 b)
{
    Vec3F32 c = { {a.x*b.x, a.y*b.y, a.z*b.z} };
    return c;
}

internal Vec3F32 div_3f32(Vec3F32 a, Vec3F32 b)
{
    Vec3F32 c = { {a.x/b.x, a.y/b.y, a.z/b.z} };
    return c;
}

internal Vec3F32 scale_3f32(Vec3F32 v, F32 s)
{
    Vec3F32 c = { {v.x*s, v.y*s, v.z*s} };
    return c;
}

internal F32 dot_3f32(Vec3F32 a, Vec3F32 b)
{
    F32 c = a.x*b.x + a.y*b.y + a.z*b.z;
    return c;
}

internal F32 length_squared_3f32(Vec3F32 v)
{
    F32 c = v.x*v.x + v.y*v.y + v.z*v.z;
    return c;
}

internal F32 length_3f32(Vec3F32 v)
{
    F32 c = sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z);
    return c;
}

internal Vec3F32 normalize_3f32(Vec3F32 v)
{
    v = scale_3f32(v, 1.f/length_3f32(v));
    return v;
}

internal Vec3F32 cross_3f32(Vec3F32 a, Vec3F32 b)
{
    Vec3F32 c = { {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x} };
    return c;
}
