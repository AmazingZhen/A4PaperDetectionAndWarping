#ifndef _CANNY_H_
#define _CANNY_H_

#include "Gaussian.h"

#define SIGMA 1
#define HYSTERESIS_THRESHOLD_LOW 25
#define HYSTERESIS_THRESHOLD_HIGH 55

CImg<float> getGaussianBlur(const CImg<float>& src_img);
CImg<float> getOneDimensionalGradient(const CImg<float>& src_img, bool isX);
CImg<float> getGradient(const CImg<float>& dx, const CImg<float>& dy);
CImg<unsigned char> getAngelOfGradient(const CImg<float>& dx, const CImg<float>& dy);
CImg<float> nonMaximumSuppression(const CImg<float>& gradient, const CImg<unsigned char>& theta);
CImg<unsigned char> hysteresisThresholding(const CImg<float>& suppression, float threshold_low, float threshold_high);

CImg<unsigned char> cannyEdgeDection(const CImg<float>& src_img);

CImg<unsigned char> getRectangleEdgeFromCanny(const CImg<unsigned char>& canny_edge);

#endif