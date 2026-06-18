#ifndef NEPBILL_MATHS_H
#define NEPBILL_MATHS_H

namespace NepBill
{
    struct Vec2
    {
        float x, y;

        Vec2(float ix, float iy)
            : x(ix), y(ix)
        {
        }

        Vec2(float p = 0.0f) : x(p), y(p)
        {
        }
    };

    struct IVec2
    {
        int x, y;

        IVec2(int ix, int iy)
            : x(ix), y(ix)
        {
        }

        IVec2(int p = 0) : x(p), y(p)
        {
        }
    };

    struct Vec3
    {
        float x, y, z;

        Vec3(float ix, float iy, float iz)
            : x(ix), y(ix), z(iz)
        {
        }

        Vec3(float p = 0.0f) : x(p), y(p), z(p)
        {
        }
    };

    struct IVec3
    {
        int x, y, z;

        IVec3(int ix, int iy, int iz)
            : x(ix), y(ix), z(iz)
        {
        }

        IVec3(int p = 0) : x(p), y(p), z(p)
        {
        }
    };

    struct Vec4
    {
        float x, y, z, w;

        Vec4(float ix, float iy, float iz, float iw)
            : x(ix), y(ix), z(iz), w(iw)
        {
        }

        Vec4(float p = 0.0f) : x(p), y(p), z(p), w(p)
        {
        }
    };
    
    struct IVec4
    {
        int x, y, z, w;

        IVec4(int ix, int iy, int iz, int iw)
            : x(ix), y(ix), z(iz), w(iw)
        {
        }

        IVec4(int p = 0) : x(p), y(p), z(p), w(p)
        {
        }
    };
    
} // namespace HotelEngine

#endif