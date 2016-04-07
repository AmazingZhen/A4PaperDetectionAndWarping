#include "stdafx.h"
#include "Segmentation.h"
#include "Canny.h"
#include <iostream>
#include <queue>
#include <cassert>

using namespace std;

int getNumOfPixelsConnectToPixel(int col, int row, const CImg<unsigned char>& canny_edge, CImg<bool>& has_visited) {
	if (col < 0 || col >= canny_edge.width())
		return 0;
	if (row < 0 || row >= canny_edge.height())
		return 0;

	int connectivity_count = 0;

	queue<int> col_pos_queue;
	queue<int> row_pos_queue;

	col_pos_queue.push(col);
	row_pos_queue.push(row);
	has_visited(col, row) = true;

	while (!col_pos_queue.empty()) {
		int cur_col = col_pos_queue.front();
		col_pos_queue.pop();
		int cur_row = row_pos_queue.front();
		row_pos_queue.pop();

		connectivity_count++;

		for (int i = cur_col - 1; i <= cur_col + 1; i++) {
			for (int j = cur_row - 1; j <= cur_row + 1; j++) {
				if (i >= 0 && i < canny_edge.width() && j >= 0 && j < canny_edge.height()) {
					if (canny_edge(i, j) != 0 && has_visited(i, j) == false) {
						has_visited(i, j) = true;
						col_pos_queue.push(i);
						row_pos_queue.push(j);
					}
				}
			}
		}
	}

	return connectivity_count;
}

CImg<unsigned char> drawPixelsConnectToPixel(int col, int row, const CImg<unsigned char>& canny_edge, CImg<bool>& has_drawed) {
	assert(col >= 0 && col < canny_edge.width());
	assert(row >= 0 && row < canny_edge.height());

	int src_width = canny_edge.width();
	int src_height = canny_edge.height();

	CImg<unsigned char> res(src_width, src_height, 1, 1, 0);

	queue<int> col_pos_queue;
	queue<int> row_pos_queue;

	col_pos_queue.push(col);
	row_pos_queue.push(row);
	has_drawed(col, row) = true;

	while (!col_pos_queue.empty()) {
		int cur_col = col_pos_queue.front();
		col_pos_queue.pop();
		int cur_row = row_pos_queue.front();
		row_pos_queue.pop();

		res(cur_col, cur_row) = 255;

		for (int i = cur_col - 1; i <= cur_col + 1; i++) {
			for (int j = cur_row - 1; j <= cur_row + 1; j++) {
				if (i >= 0 && i < canny_edge.width() && j >= 0 && j < canny_edge.height()) {
					if (canny_edge(i, j) != 0 && has_drawed(i, j) == false) {
						has_drawed(i, j) = true;
						col_pos_queue.push(i);
						row_pos_queue.push(j);
					}
				}
			}
		}
	}

	return res;
}

CImg<unsigned char> getForeground(const CImg<float>& gray, float threshold) {
	CImg<unsigned char> res;
	res = gray.get_threshold(threshold).get_normalize(0, 255);

	int src_width = gray.width();
	int src_height = gray.height();

	CImg<bool> has_visited(src_width, src_height, 1, 1, 0);

	int max_connectivity_count = 0;
	int max_col_pos = 0;
	int max_row_pos = 0;

	for (int col = 0; col < src_width; col++) {
		for (int row = 0; row < src_height; row++) {
			if (res(col, row) != 0 && !has_visited(col, row)) {
				int connectivity_count = getNumOfPixelsConnectToPixel(col, row, res, has_visited);

				if (connectivity_count > max_connectivity_count) {
					max_connectivity_count = connectivity_count;
					max_col_pos = col;
					max_row_pos = row;
				}
			}
		}
	}

	CImg<bool> has_drawed(src_width, src_height, 1, 1, 0);
	res = drawPixelsConnectToPixel(max_col_pos, max_row_pos, res, has_drawed);

	res.dilate(11);
	res.erode(11);

	return res;
}

CImg<unsigned char> getEdgeFromForeground(const CImg<unsigned char> &foreground) {
	int src_width = foreground.width();
	int src_height = foreground.height();

	CImg<float> dx = getOneDimensionalGradient(foreground, true);
	CImg<float> dy = getOneDimensionalGradient(foreground, false);

	CImg<float> gradient = getGradient(dx, dy);
	CImg<unsigned char> theta = getAngelOfGradient(dx, dy);

	CImg<float> sup_res = nonMaximumSuppression(gradient, theta);

	CImg<unsigned char> res(src_width, src_height, 1, 1, 0);
	for (int col = 0; col < src_width; col++) {
		for (int row = 0; row < src_height; row++) {
			if (sup_res(col, row) > 0) {
				res(col, row) = 255;
			}
		}
	}

	return res;
}

CImg<unsigned char> getEdgeBySegmentation(const CImg<float>& gray) {
	return getEdgeFromForeground(getForeground(gray, SEGMENTATION_THRESHOLD));
}