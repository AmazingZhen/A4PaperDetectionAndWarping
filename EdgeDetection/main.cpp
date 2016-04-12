// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CImg.h"
#include "Canny.h"
#include "draw2d.h"
#include "Hough.h"
#include "Segmentation.h"
#include "PaperDetection.h"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cimg_library;

string getString(const int n){
	std::stringstream newstr;
	newstr << n;
	return newstr.str();
}

template <class T>
CImg<T> getGray(const CImg<T>& src_img) {
	assert(src_img.spectrum() == 3);
	int dst_width = src_img.width();
	int dst_height = src_img.height();

	CImg<T> res(dst_width, dst_height, 1, 1, 0);

	for (int col = 1; col < dst_width - 1; col++) {
		for (int row = 1; row < dst_height - 1; row++) {
			res(col, row) = 0.299 * src_img(col, row, 0) + 0.587 * src_img(col, row, 1) + 0.114 * src_img(col, row, 2);
		}
	}

	return res;
}

int main(int argc, char* argv[])
{
	int t;

	for (int i = 1; i < 17; i++) {
		string s1("dataset/");
		string s2(".jpg");

		CImg<float> image((s1 + getString(i) + s2).c_str());
		CImg<float> gray = getGaussianBlur(getGray(image));

		CImg<float> res = warp(image, gray);
		string t1("res/final_");
		string t2(".jpg");
		res.save((t1 + getString(i) + t2).c_str());

		cout << i << endl;
	}

	cin >> t;

	return 0;
}

