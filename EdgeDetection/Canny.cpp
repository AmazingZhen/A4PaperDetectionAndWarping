#include "stdafx.h"
#include "Canny.h"
#include <iostream>
#include <queue>

using namespace std;

CImg<float> getGaussianBlur(const CImg<float>& src_img) {
	return src_img.get_correlate(getGaussianMask(SIGMA));
}

CImg<float> getOneDimensionalGradient(const CImg<float>& src_img, bool isX) {
	return src_img.get_correlate(getOneDimensionalDerivativeOfGaussianMask(SIGMA, isX));
}

CImg<float> getGradient(const CImg<float>& dx, const CImg<float>& dy) {
	assert(dx.width() == dy.width() && dx.height() == dy.height());

	int dst_width = dx.width();
	int dst_height = dx.height();

	CImg<float> gradient(dst_width, dst_height, 1, 1, 0);

	for (int col = 0; col < dst_width; col++) {
		for (int row = 0; row < dst_height; row++) {
			gradient(col, row) = sqrt(dx(col, row) * dx(col, row) + dy(col, row) * dy(col, row));
		}
	}

	return gradient;
}

CImg<unsigned char> getAngelOfGradient(const CImg<float>& dx, const CImg<float>& dy) {
	assert(dx.width() == dy.width() && dx.height() == dy.height());

	int dst_width = dx.width();
	int dst_height = dx.height();

	CImg<unsigned char> theta(dst_width, dst_height, 1, 1, 0);
	for (int col = 0; col < dst_width; col++) {
		for (int row = 0; row < dst_height; row++) {
			if (dx(col, row) == 0) {
				// tan(theta) = 0
				if (dy(col, row) == 0) {
					theta(col, row) = 0;
				}
				// tan(theta) = infinity
				else {
					theta(col, row) = 90;
				}
			}
			else {
				float tan_theta = dy(col, row) / dx(col, row);

				if (tan_theta > -0.4142 && tan_theta <= 0.4142) {
					theta(col, row) = 0;
				}
				else if (tan_theta > 0.4142 && tan_theta < 2.4142) {
					theta(col, row) = 45;
				}
				else if (abs(tan_theta) >= 2.4142) {
					theta(col, row) = 90;
				}
				else if (tan_theta > -2.4142 && tan_theta <= -0.4142) {
					theta(col, row) = 135;
				}
			}
		}
	}

	return theta;
}

CImg<float> nonMaximumSuppression(const CImg<float>& gradient, const CImg<unsigned char>& theta) {
	assert(gradient.width() == theta.width() && gradient.height() == theta.height());

	int dst_width = gradient.width();
	int dst_height = gradient.height();

	CImg<float> res(dst_width, dst_height, 1, 1, 0);
	for (int col = 1; col < dst_width - 1; col++) {
		for (int row = 1; row < dst_height - 1; row++) {
			if (theta(col, row) == 0) {
				if (gradient(col, row) > gradient(col - 1, row) && gradient(col, row) > gradient(col + 1, row)) {
					res(col, row) = gradient(col, row);
				}
				else {
					res(col, row) = 0;
				}
			}
			else if (theta(col, row) == 45) {
				if (gradient(col, row) > gradient(col - 1, row - 1) && gradient(col, row) > gradient(col + 1, row + 1)) {
					res(col, row) = gradient(col, row);
				}
				else {
					res(col, row) = 0;
				}
			}
			else if (theta(col, row) == 90) {
				if (gradient(col, row) > gradient(col, row - 1) && gradient(col, row) > gradient(col, row + 1)) {
					res(col, row) = gradient(col, row);
				}
				else {
					res(col, row) = 0;
				}
			}
			else if (theta(col, row) == 135) {
				if (gradient(col, row) > gradient(col - 1, row + 1) && gradient(col, row) > gradient(col + 1, row - 1)) {
					res(col, row) = gradient(col, row);
				}
				else {
					res(col, row) = 0;
				}
			}
		}
	}

	return res.normalize(0, 255);
}

void findPixelConnectToEdge(int col, int row, const CImg<float>& suppression, CImg<unsigned char>& res, float threshold_low, float threshold_high) {
	if (col < 0 || col >= suppression.width())
		return;
	if (row < 0 || row >= suppression.height())
		return;

	assert(suppression.width() == res.width() && suppression.height() == res.height());

	if (res(col, row) == 0 && suppression(col, row) >= threshold_low && suppression(col, row) < threshold_high) {
		res(col, row) = 255;
		// 8-adjacency
		findPixelConnectToEdge(col - 1, row - 1, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col, row - 1, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col + 1, row - 1, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col - 1, row, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col + 1, row, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col - 1, row + 1, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col, row + 1, suppression, res, threshold_low, threshold_high);
		findPixelConnectToEdge(col + 1, row + 1, suppression, res, threshold_low, threshold_high);
	}
}

CImg<unsigned char> hysteresisThresholding(const CImg<float>& suppression, float threshold_low, float threshold_high) {
	int dst_width = suppression.width();
	int dst_height = suppression.height();

	CImg<unsigned char> res(dst_width, dst_height, 1, 1, 0);

	for (int col = 2; col < dst_width - 2; col++) {
		for (int row = 2; row < dst_height - 2; row++) {
			if (suppression(col, row) >= threshold_high) {
				res(col, row) = 255;
			}
			else if (suppression(col, row) < threshold_low) {
				res(col, row) = 0;
			}
		}
	}

	// 8-adjacency
	for (int col = 2; col < dst_width - 2; col++) {
		for (int row = 2; row < dst_height - 2; row++) {
			if (res(col, row) != 0) {
				findPixelConnectToEdge(col - 1, row - 1, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col, row - 1, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col + 1, row - 1, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col - 1, row, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col + 1, row, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col - 1, row + 1, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col, row + 1, suppression, res, threshold_low, threshold_high);
				findPixelConnectToEdge(col + 1, row + 1, suppression, res, threshold_low, threshold_high);
			}
		}
	}
	return res;
}

CImg<unsigned char> cannyEdgeDection(const CImg<float>& src_img) {
	CImg<float> dx = getOneDimensionalGradient(src_img, true);
	CImg<float> dy = getOneDimensionalGradient(src_img, false);

	CImg<float> gradient = getGradient(dx, dy);
	CImg<unsigned char> theta = getAngelOfGradient(dx, dy);

	CImg<float> sup_res = nonMaximumSuppression(gradient, theta);

	CImg<unsigned char> res = hysteresisThresholding(sup_res, HYSTERESIS_THRESHOLD_LOW, HYSTERESIS_THRESHOLD_HIGH);

	return res;
}
//
//int getNumOfPixelsConnectToPixel(int col, int row, const CImg<unsigned char>& canny_edge, CImg<bool>& has_visited) {
//	if (col < 0 || col >= canny_edge.width())
//		return 0;
//	if (row < 0 || row >= canny_edge.height())
//		return 0;
//
//	int connectivity_count = 0;
//
//	queue<int> col_pos_queue;
//	queue<int> row_pos_queue;
//
//	col_pos_queue.push(col);
//	row_pos_queue.push(row);
//	has_visited(col, row) = true;
//
//	while (!col_pos_queue.empty()) {
//		int cur_col = col_pos_queue.front();
//		col_pos_queue.pop();
//		int cur_row = row_pos_queue.front();
//		row_pos_queue.pop();
//
//		connectivity_count++;
//
//		for (int i = cur_col - 1; i <= cur_col + 1; i++) {
//			for (int j = cur_row - 1; j <= cur_row + 1; j++) {
//				if (i >= 0 && i < canny_edge.width() && j >= 0 && j < canny_edge.height()) {
//					if (canny_edge(i, j) != 0 && has_visited(i, j) == false) {
//						has_visited(i, j) = true;
//						col_pos_queue.push(i);
//						row_pos_queue.push(j);
//					}
//				}
//			}
//		}
//	}
//
//	return connectivity_count;
//}
//
//CImg<unsigned char> drawPixelConnectToEdge(int col, int row, const CImg<unsigned char>& canny_edge, CImg<bool>& has_drawed) {
//	assert(col >= 0 && col < canny_edge.width());
//	assert(row >= 0 && row < canny_edge.height());
//
//	int src_width = canny_edge.width();
//	int src_height = canny_edge.height();
//
//	CImg<unsigned char> res(src_width, src_height, 1, 1, 0);
//
//	queue<int> col_pos_queue;
//	queue<int> row_pos_queue;
//
//	col_pos_queue.push(col);
//	row_pos_queue.push(row);
//	has_drawed(col, row) = true;
//
//	while (!col_pos_queue.empty()) {
//		int cur_col = col_pos_queue.front();
//		col_pos_queue.pop();
//		int cur_row = row_pos_queue.front();
//		row_pos_queue.pop();
//
//		res(cur_col, cur_row) = 255;
//
//		for (int i = cur_col - 1; i <= cur_col + 1; i++) {
//			for (int j = cur_row - 1; j <= cur_row + 1; j++) {
//				if (i >= 0 && i < canny_edge.width() && j >= 0 && j < canny_edge.height()) {
//					if (canny_edge(i, j) != 0 && has_drawed(i, j) == false) {
//						has_drawed(i, j) = true;
//						col_pos_queue.push(i);
//						row_pos_queue.push(j);
//					}
//				}
//			}
//		}
//	}
//
//	return res;
//}
//
//CImg<unsigned char> getRectangleEdgeFromCanny(const CImg<unsigned char>& canny_edge) {
//	int src_width = canny_edge.width();
//	int src_height = canny_edge.height();
//
//	CImg<bool> has_visited(src_width, src_height, 1, 1, 0);
//
//	int max_connectivity_count = 0;
//	int col_pos = 0;
//	int row_pos = 0;
//
//	for (int col = 0; col < src_width; col++) {
//		for (int row = 0; row < src_height; row++) {
//			if (canny_edge(col, row) != 0 && !has_visited(col, row)) {
//				int connectivity_count = getNumOfPixelsConnectToPixel(col, row, canny_edge, has_visited);
//
//				if (connectivity_count > max_connectivity_count) {
//					max_connectivity_count = connectivity_count;
//					col_pos = col;
//					row_pos = row;
//				}
//			}
//		}
//	}
//
//	CImg<bool> has_drawed(src_width, src_height, 1, 1, 0);
//
//	return drawPixelConnectToEdge(col_pos, row_pos, canny_edge, has_drawed);
//}