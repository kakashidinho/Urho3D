/*
// Copyright (C) 2018 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "luckey.h"

unsigned LucKey::IntVector2ToHash(IntVector2 vec) { return (MakeHash(vec.x_) & 0xffff) | (MakeHash(vec.y_) << 16); }

Vector3 LucKey::PolarPoint(float radius, float angle)
{
    return Quaternion(angle, Vector3::UP) * Vector3::FORWARD * radius;
}

float LucKey::Delta(float lhs, float rhs, bool angle)
{
    if (!angle)
        return (lhs > rhs) ? Abs(lhs - rhs) : Abs(rhs - lhs);
    else {
        lhs = Cycle(lhs, 0.0f, 360.0f);
        rhs = Cycle(rhs, 0.0f, 360.0f);
        if (Delta(lhs, rhs) > 180.0f)
            return Abs(360.0f - Delta(lhs, rhs));
        else
            return Delta(lhs, rhs);
    }
}
Vector2 LucKey::Rotate(const Vector2 vec2, const float angle){
    float x{vec2.x_};
    float y{vec2.y_};

    float theta{M_DEGTORAD * angle};

    float cs{cos(theta)};
    float sn{sin(theta)};

    return Vector2(x * cs - y * sn, x * sn + y * cs);
}

float LucKey::RandomSign()
{
    return Random(2) * 2 - 1;
}
Color LucKey::RandomHairColor(bool onlyNatural)
{
    bool grey{!Random(23)};
    bool dyed{!Random(23)};
    Color hairColor{};
    if (onlyNatural || (!dyed && !grey)){
        //Natural
        hairColor.FromHSV(Random(0.034f, 0.15f),
                          Random(0.34f, 0.65f),
                          Random(0.05f, 0.9f));
    } else if (!dyed && grey){
        //Grey
        hairColor.FromHSV(Random(0.034f, 0.16f),
                          Random(0.0f, 0.05f),
                          Random(0.23f, 0.86f));
    } else if (dyed && !grey){
        //Bright dye
        hairColor.FromHSV(Random(6)*0.1666f,
                          Random(0.6f, 0.9f),
                          Random(0.5f, 0.71f));
    } else if (dyed && grey){
        //Fake black
        hairColor.FromHSV(0.666f,
                          0.05f,
                          Random(0.01f, 0.05f));
    }
    return hairColor;
}

Color LucKey::RandomSkinColor()
{
    Color skinColor{};
    skinColor.FromHSV(Random(0.05f, 0.13f), Random(0.6f, 0.8f), Random(0.23f, 0.8f));
    return skinColor;
}
Color LucKey::RandomColor()
{
    Color color{};
    color.FromHSV(Random(), Random(), Random());
    return color;
}

float LucKey::Sine(float x)
{
    x = Cycle(x, -M_PI, M_PI);

    float sin{};

    if (x < 0.0f)
        sin = 1.27323954f * x + 0.405284735f * x * x;
    else
        sin = 1.27323954f * x - 0.405284735f * x * x;

    if (sin < 0)
        sin = 0.225f * (sin *-sin - sin) + sin;
    else
        sin = 0.225f * (sin * sin - sin) + sin;

    return sin;
}
float LucKey::Cosine(float x)
{
    return Sine(x + M_PI * 0.5f);
}

int LucKey::Cycle(int x, int min, int max)
{
    int res{x};
    if (min > max){ int temp{min};
        min = max; max = temp;
    }

    int range{max - min + 1};

    if (x < min)
        res += range * abs((max - x) / range);
    else if (x > max)
        res -= range * abs((x - min) / range);

    return res;
}

float LucKey::Cycle(float x, float min, float max)
{
    float res{x};
    if (min > max){ float temp{min};
        min = max; max = temp;
    }

    float range{max - min};

    if (x < min)
        res += range * abs(ceil((min - x) / range));
    else if (x > max)
        res -= range * abs(ceil((x - max) / range));

    return res;
}

