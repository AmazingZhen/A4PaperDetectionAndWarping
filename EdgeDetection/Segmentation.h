#ifndef _SEGMENTATION_H_
#define _SEGMENTATION_H_

#include "CImg.h"

using namespace cimg_library;

#define SEGMENTATION_THRESHOLD 100

// Find the maximum connected domain by breadth first search, relatively slow.
CImg<unsigned char> getForeground(const CImg<float>& gray, float threshold);
CImg<unsigned char> getEdgeFromForeground(const CImg<unsigned char> &foreground);

CImg<unsigned char> getEdgeBySegmentation(const CImg<float>& gray);


#endif