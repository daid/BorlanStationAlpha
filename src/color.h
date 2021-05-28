#pragma once

#include <cmath>

class Color;

class HsvColor
{
public:
    HsvColor()
    : hue(0), saturation(0), value(0)
    {
    }

    HsvColor(const double hue, const double saturation, const double value)
    : hue(hue), saturation(saturation), value(value)
    {
    }
    
    // hue: 0-360
    // saturation: 0-100
    // value: 0-100
    double hue;
    double saturation;
    double value;
};

class Color
{
public:
    uint8_t r, g, b;

    Color()
    : r(0), g(0), b(0)
    {
    }

    Color(uint8_t red, uint8_t green, uint8_t blue)
    : r(red), g(green), b(blue)
    {
    }

    Color(const HsvColor& hsv)
    {
        double c = hsv.value / 100.0 * hsv.saturation / 100.0;
        double x = c * (1.0 - std::abs(std::fmod((hsv.hue / 60.0), 2.0) - 1.0));
        double m = hsv.value / 100.0 - c;

        if (hsv.hue < 60)
        {
            r = (c + m) * 255;
            g = (x + m) * 255;
            b = m * 255;
        }
        else if (hsv.hue < 120)
        {
            r = (x + m) * 255;
            g = (c + m) * 255;
            b = m * 255;
        }
        else if (hsv.hue < 180)
        {
            r = m * 255;
            g = (c + m) * 255;
            b = (x + m) * 255;
        }
        else if (hsv.hue < 240)
        {
            r = m * 255;
            g = (x + m) * 255;
            b = (c + m) * 255;
        }
        else if (hsv.hue < 300)
        {
            r = (x + m) * 255;
            g = m * 255;
            b = (c + m) * 255;
        }
        else
        {
            r = (c + m) * 255;
            g = m * 255;
            b = (x + m) * 255;
        }
    }
};

