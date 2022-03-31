#pragma once

#include <assert.h>

#include <stdexcept>
#include <string>
#include <vec3.hpp>

class Image
{
public:
    Image(int w, int h)
    {
        width  = w;
        height = h;
        data   = new glm::dvec3[width * height];

        SetAllPixels(glm::dvec3(0.0f));
    }

    ~Image()
    {
        delete[] data;
    }

    int Width() const
    {
        return width;
    }

    int Height() const
    {
        return height;
    }

    const glm::dvec3 &GetPixel(int x, int y) const
    {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }

    void SetAllPixels(const glm::dvec3 &color)
    {
        for (int i = 0; i < width * height; i++) {
            data[i] = color;
        }
    }

    void SetPixel(int x, int y, const glm::dvec3 &color)
    {
        if (!(x >= 0 && x < width)) {
            throw std::runtime_error("x out image: " + std::to_string(x));
        }
        if (!(y >= 0 && y < height)) {
            throw std::runtime_error("y out image: " + std::to_string(y));
        }
        data[y * width + x] = color;
    }

    static Image *LoadPPM(const char *filename);
    void          SavePPM(const char *filename) const;
    static Image *LoadTGA(const char *filename);
    void          SaveTGA(const char *filename) const;

    // Extension for image comparison.
    static Image *Compare(Image *img1, Image *img2);

private:
    int         width;
    int         height;
    glm::dvec3 *data;
};