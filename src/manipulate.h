// ImageManipulator.h
#pragma once
#include <Magick++.h>

class ImageManipulator {
public:
    static void resize(Magick::Image& image, const std::string& size);
    static void crop(Magick::Image& image, const std::string& dimensions);
};