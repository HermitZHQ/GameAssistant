#pragma once
#include <memory>

#include "opencv.hpp"
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "opencv_highgui410d.lib")
#pragma comment(lib, "opencv_imgproc410d.lib")
#pragma comment(lib, "opencv_core410d.lib")
#pragma comment(lib, "opencv_imgcodecs410d.lib")
#pragma comment(lib, "opencv_features2d410d.lib")
#else
#pragma comment(lib, "opencv_highgui410.lib")
#pragma comment(lib, "opencv_imgproc410.lib")
#pragma comment(lib, "opencv_core410.lib")
#pragma comment(lib, "opencv_imgcodecs410.lib")
#pragma comment(lib, "opencv_features2d410.lib")
#endif // DEBUG

const float inlier_threshold = 2.5f; // Distance threshold to identify inliers with homography check
const float nn_match_ratio = 0.8f;   // Nearest neighbor matching ratio

using std::unique_ptr;

class OpencvMgr
{
public:
	OpencvMgr();
	~OpencvMgr();

	static OpencvMgr& Singleton();

	double CompareImgs();

private:
	static OpencvMgr						*m_singleton;
	static unique_ptr<OpencvMgr>			m_autoPtr;
};

