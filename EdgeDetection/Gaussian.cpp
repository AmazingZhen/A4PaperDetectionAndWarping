#include "stdafx.h"
#include "Gaussian.h"

float getGaussianDistribution(int x, int y, float sigma) {
	return exp(-(x * x + y * y) / (2 * sigma * sigma));
}

CImg<float> getGaussianMask(float sigma) {
	int windowSize = 6 * sigma + 1;
	if (windowSize % 2 != 0) {
		windowSize += 1;
	}

	CImg<float> mask(windowSize, windowSize, 1, 1, 0);
	int center = (windowSize - 1) / 2;

	float sum = 0;
	for (int col = 0; col < windowSize; col++) {
		for (int row = 0; row < windowSize; row++) {
			mask(col, row) = getGaussianDistribution(col - center, row - center, sigma);
			sum += mask(col, row);
		}
	}

	// normalization
	for (int col = 0; col < windowSize; col++) {
		for (int row = 0; row < windowSize; row++) {
			mask(col, row) /= sum;
		}
	}

	return mask;
}

CImg<float> getDerivativeOfGaussianMask(float sigma) {
	int windowSize = 6 * sigma + 1;
	if (windowSize % 2 != 0) {
		windowSize += 1;
	}

	CImg<float> mask(windowSize, windowSize, 1, 1, 0);
	int center = (windowSize - 1) / 2;

	float sum = 0;
	for (int col = 0; col < windowSize; col++) {
		for (int row = 0; row < windowSize; row++) {
			mask(col, row) = getGaussianDistribution(col - center, row - center, sigma);
			sum += mask(col, row);
		}
	}

	// normalization
	for (int col = 0; col < windowSize; col++) {
		for (int row = 0; row < windowSize; row++) {
			mask(col, row) /= sum;
		}
	}

	return mask;
}

CImg<float> getOneDimensionalGaussianMask(float sigma, bool isX) {
	int windowSize = 6 * sigma + 1;
	if (windowSize % 2 != 0) {
		windowSize += 1;
	}

	int center = (windowSize - 1) / 2;

	if (isX) {
		CImg<float> mask(windowSize, 1, 1, 1, 0);

		float sum = 0;
		for (int col = 0; col < windowSize; col++) {
			mask(col, 0) = getGaussianDistribution(col - center, 0, sigma);

			sum += mask(col, 0);
		}

		// normalization
		for (int col = 0; col < windowSize; col++) {
			mask(col, 0) /= sum;
		}

		return mask;
	}
	else {
		CImg<float> mask(1, windowSize, 1, 1, 0);

		float sum = 0;
		for (int row = 0; row < windowSize; row++) {
			mask(0, row) = getGaussianDistribution(0, row - center, sigma);

			sum += mask(0, row);
		}

		// normalization
		for (int row = 0; row < windowSize; row++) {
			mask(0, row) /= sum;
		}

		return mask;
	}
}

CImg<float> getOneDimensionalDerivativeOfGaussianMask(float sigma, bool isX) {
	int windowSize = 6 * sigma + 1;
	if (windowSize % 2 != 0) {
		windowSize += 1;
	}

	int center = (windowSize - 1) / 2;

	if (isX) {
		CImg<float> mask(windowSize, 1, 1, 1, 0);
		float sum = 0;

		for (int col = 0; col < windowSize; col++) {
			mask(col, 0) = -(col - center) / (sigma * sigma) * getGaussianDistribution(col - center, 0, sigma);

			sum += mask(col, 0);
		}

		// normalization
		for (int col = 0; col < windowSize; col++) {
			mask(col, 0) /= sum;
		}

		return mask;
	}
	else {
		CImg<float> mask(1, windowSize, 1, 1, 0);
		float sum = 0;

		for (int row = 0; row < windowSize; row++) {
			mask(0, row) = -(row - center) / (sigma * sigma) * getGaussianDistribution(0, row - center, sigma);

			sum += mask(0, row);
		}

		// normalization
		for (int row = 0; row < windowSize; row++) {
			mask(0, row) /= sum;
		}

		return mask;
	}
}