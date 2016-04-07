#ifndef _GAUSSIAN_H_
#define _GAUSSIAN_H_

#include "CImg.h"
#include <cmath>
#include <cassert>

using namespace cimg_library;

float getGaussianDistribution(int x, int y, float sigma);

CImg<float> getGaussianMask(float sigma);
CImg<float> getOneDimensionalGaussianMask(float sigma, bool isX);
CImg<float> getOneDimensionalDerivativeOfGaussianMask(float sigma, bool isX);


#endif