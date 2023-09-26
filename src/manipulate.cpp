// ImageManipulator.cpp
#include "manipulate.h"
#include "Magick++/Exception.h"
#include "Magick++/Geometry.h"
#include <iostream>

void ImageManipulator::resize(Magick::Image &image, const std::string &size) {
  image.resize(size);
}

void ImageManipulator::crop(Magick::Image &image,
                            const std::string &dimensions) {
  std::cout << "Cropping to: " << dimensions << std::endl;
  try {
    image.crop(dimensions);
    image.repage();
  } catch (Magick::Exception &e) {
    std::cerr << e.what() << std::endl;
  }
}