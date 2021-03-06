
#include "ZZPicCompareStrategy.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <math.h>
#include "QTime"
#include "QTimer"
#include "mainwindow.h"

#define SAFE_DEL_ARR(p) if (nullptr != p){delete[] p; p = nullptr;}

using namespace std;
using namespace cv;

ZZPicCompareStrategy::ZZPicCompareStrategy(MainWindow *mainWnd)
	:PicCompareStrategy(mainWnd)
	, m_buf(nullptr), m_dc(nullptr)
	, m_compatibleDc(nullptr), m_compatibleBmp(nullptr)
{
}

ZZPicCompareStrategy::~ZZPicCompareStrategy()
{
	SAFE_DEL_ARR(m_buf);
	DeleteObject(m_compatibleBmp);
// 	ReleaseDC(m, m_dc);
	DeleteDC(m_compatibleDc);
}

double ZZPicCompareStrategy::HandlePicCompare(InputData &input, HWND gameWnd, const CSize &s)
{
	if (nullptr == gameWnd)
		return 0.0;

	DWORD dwTime = GetTickCount();

	//--------------------------get window bitmap
	m_gameWnd = gameWnd;
	m_dc = (nullptr == m_dc) ? GetDC(m_gameWnd) : m_dc;
	m_compatibleDc = (nullptr == m_compatibleDc) ? CreateCompatibleDC(m_dc) : m_compatibleDc;
	RECT rect = { 0 };
	GetWindowRect(m_gameWnd, &rect);
	int x = rect.left, y = rect.top, w = (rect.right - rect.left), h = (rect.bottom - rect.top);
	m_compatibleBmp = (nullptr == m_compatibleBmp) ? CreateCompatibleBitmap(m_dc, w, h) : m_compatibleBmp;
	SelectObject(m_compatibleDc, m_compatibleBmp);

	//This is the best stretch mode
	BOOL bRes = BitBlt(m_compatibleDc, 0, 0, w, h, m_dc, 0, 0, SRCCOPY);
	if (FALSE == bRes)
	{
		return 0.0;
	}

	// Get the BITMAP from the HBITMAP
	BITMAP bmpScreen;
	GetObject(m_compatibleBmp, sizeof(BITMAP), &bmpScreen);

// 	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	m_buf = (nullptr == m_buf) ? new char[w * h * 4] : m_buf;
	int iRes = GetDIBits(m_compatibleDc, m_compatibleBmp, 0, h, m_buf, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
	if (0 == iRes)
	{
		return 0.0;
	}

	//-----------------------Mat img
	cv::Size cvSize;
	cvSize.width = w;
	cvSize.height = h;

	cv::Mat img(cvSize, CV_8UC3);
	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			img.at<cv::Vec3b>(i, j)[0] = m_buf[(j + i * w) * 4];
			img.at<cv::Vec3b>(i, j)[1] = m_buf[(j + i * w) * 4 + 1];
			img.at<cv::Vec3b>(i, j)[2] = m_buf[(j + i * w) * 4 + 2];
		}
	}
	cv::flip(img, img, 0);

	// 	cv::imwrite("C:\\Users\\cdzha\\Desktop\\test.bmp", img);
	// 	cv::imwrite("C:\\Users\\cdzha\\Desktop\\test2.bmp", partialImg);


	//-----------------------compare func
	float fBaseScaleFactor = 2.0f;
	double rOffsetRate = 0, gOffsetRate = 0, bOffsetRate = 0;
	size_t kpSize1 = 0, kpSize2 = 0, matchedSize = 0;

	auto compareFunc = [&](Mat srcImg, Mat cmpImg)->double {
		Mat img1_rgb = imread(input.picPath);
		Mat img2_rgb = cmpImg.clone();

// 		Mat img1_rgb = imread("d:/3.png");
// 		Mat img2_rgb = imread("d:/6.png");

		//计算直方图（统计rgb像素总值）
		int r = 0, g = 0, b = 0, r2 = 0, g2 = 0, b2 = 0;
		int tmpR, tmpG, tmpB;
		for (int i = 0; i < img1_rgb.rows; ++i)
		{
			for (int j = 0; j < img1_rgb.cols; ++j)
			{
				tmpR = img1_rgb.at<Vec3b>(i, j)[2];
				tmpG = img1_rgb.at<Vec3b>(i, j)[1];
				tmpB = img1_rgb.at<Vec3b>(i, j)[0];
				// handle the threshold compare
				if (input.bEnableThresholdCmpFlag
					&& (!(tmpR >= input.thresholdCmpLowColor[0] && tmpR <= input.thresholdCmpHighColor[0])
					|| !(tmpG >= input.thresholdCmpLowColor[1] && tmpG <= input.thresholdCmpHighColor[1])
					|| !(tmpB >= input.thresholdCmpLowColor[2] && tmpB <= input.thresholdCmpHighColor[2]))) {
					img1_rgb.at<Vec3b>(i, j)[2] = input.thresholdCmpReplaceColor[0];
					img1_rgb.at<Vec3b>(i, j)[1] = input.thresholdCmpReplaceColor[1];
					img1_rgb.at<Vec3b>(i, j)[0] = input.thresholdCmpReplaceColor[2];
				}

				r += img1_rgb.at<Vec3b>(i, j)[2];
				g += img1_rgb.at<Vec3b>(i, j)[1];
				b += img1_rgb.at<Vec3b>(i, j)[0];
			}
		}

// 		imwrite("d:/6_replace.png", img1_rgb);
		for (int i = 0; i < img2_rgb.rows; ++i)
		{
			for (int j = 0; j < img2_rgb.cols; ++j)
			{
				tmpR = img2_rgb.at<Vec3b>(i, j)[2];
				tmpG = img2_rgb.at<Vec3b>(i, j)[1];
				tmpB = img2_rgb.at<Vec3b>(i, j)[0];
				// handle the threshold compare
				if (input.bEnableThresholdCmpFlag
					&& (!(tmpR >= input.thresholdCmpLowColor[0] && tmpR <= input.thresholdCmpHighColor[0])
						|| !(tmpG >= input.thresholdCmpLowColor[1] && tmpG <= input.thresholdCmpHighColor[1])
						|| !(tmpB >= input.thresholdCmpLowColor[2] && tmpB <= input.thresholdCmpHighColor[2]))) {
					img2_rgb.at<Vec3b>(i, j)[2] = input.thresholdCmpReplaceColor[0];
					img2_rgb.at<Vec3b>(i, j)[1] = input.thresholdCmpReplaceColor[1];
					img2_rgb.at<Vec3b>(i, j)[0] = input.thresholdCmpReplaceColor[2];
				}

				r2 += img2_rgb.at<Vec3b>(i, j)[2];
				g2 += img2_rgb.at<Vec3b>(i, j)[1];
				b2 += img2_rgb.at<Vec3b>(i, j)[0];
			}
		}
		double rRate = (double)r2 / (double)r;
		double gRate = (double)g2 / (double)g;
		double bRate = (double)b2 / (double)b;
		rOffsetRate = std::abs((1.0 - rRate));
		gOffsetRate = std::abs((1.0 - gRate));
		bOffsetRate = std::abs((1.0 - bRate));

		Mat img1;
		cv::cvtColor(img1_rgb, img1, COLOR_BGR2GRAY);
		Mat img2;
		cv::cvtColor(img2_rgb, img2, COLOR_BGR2GRAY);
		//测试获取图片是否正常，之前我获取的rgb图片中的rgb值就填写反了，而且我获取特征点的img2没有使用灰度图....
		if (GetDebugFlg())
		{
			imwrite("d:/img2_rgb.png", img2_rgb);
			imwrite("d:/img2.png", img2);
			imwrite("d:/img1_rgb.png", img1_rgb);
			imwrite("d:/img1.png", img1);
		}

		const double cmpOffsetRate = 0.2;
		if (rOffsetRate > cmpOffsetRate || gOffsetRate > cmpOffsetRate || bOffsetRate > cmpOffsetRate)
		{
// 			m_mainWnd->AddTipInfo(std::string("rgb over offset:").append(input.picPath).c_str());
#ifdef DEV_VER
			char cTmp[MAX_PATH] = { 0 };
			sprintf_s(cTmp, MAX_PATH, "rgb over offset:[%f|%f|%f]_%s\n", rOffsetRate, gOffsetRate, bOffsetRate, input.picPath);
			OutputDebugStringA(cTmp);
#endif // _DEBUG
			return 0.0;
		}

		vector<KeyPoint> kpts1, kpts2;
		Mat desc1, desc2;

		Ptr<AKAZE> akaze = AKAZE::create();

		akaze->detectAndCompute(img1, noArray(), kpts1, desc1);
		akaze->detectAndCompute(img2, noArray(), kpts2, desc2);

		//为了能够读取出图片的特征点，我们需要等比例的放大图片，直到特征点个数大于30（测试性质）
		float fScaleStep = 1.0f;//每次加大基数
		const unsigned int cKPNum = 25;
		int originalWidth1 = img1.cols;
		int originalHeight1 = img1.rows;
		int originalWidth2 = img2.cols;
		int originalHeight2 = img2.rows;
		int iTestCount = 0;
		// 经过我测试，比较小的图，其实放大倍数够的话也可以检测到特征点，所以可以一次放到够大，反而节省了多次放大的时间
		if (originalWidth1 < 50) {
			fBaseScaleFactor = 150.0f / (float)originalWidth1;
		}
		while (iTestCount < 2 && (kpts1.size() < cKPNum && kpts2.size() < cKPNum))
		{
			cv::resize(img1, img1, cv::Size(originalWidth1 * fBaseScaleFactor, originalHeight1 * fBaseScaleFactor));
			cv::resize(img2, img2, cv::Size(originalWidth2 * fBaseScaleFactor, originalHeight2 * fBaseScaleFactor));

			akaze->detectAndCompute(img1, noArray(), kpts1, desc1);
			akaze->detectAndCompute(img2, noArray(), kpts2, desc2);

			fBaseScaleFactor += fScaleStep;
			++iTestCount;
		}

		if (kpts1.size() < 2 || kpts2.size() < 2)
		{
			std::string strPicName = input.picPath;
			auto pos = strPicName.find_last_of( "/" );
			if ( pos != std::string::npos )
			{
				strPicName = strPicName.substr( pos );
			}

#ifdef DEV_VER
			m_mainWnd->AddTipInfo(std::string("key point too low[").append(std::to_string(kpts1.size())).append("|").append( std::to_string( kpts2.size() ) ).append("]-").append( strPicName ).c_str());
#endif // _DEBUG

			auto validRate = 1.0 - input.cmpPicRate;
			if (r == r2 && g == g2 && b == b2 && r != 0 && g != 0) {
				return 1.0;
			}
			else if ((rOffsetRate + gOffsetRate + bOffsetRate) / 3.0 < validRate) {
				return (1.0 - (rOffsetRate + gOffsetRate + bOffsetRate) / 3.0);
			}

			return 0.0;
		}

		//比较关键点的个数差异，同一张图的差异不可能过大
		kpSize1 = kpts1.size();
		kpSize2 = kpts2.size();
		double sizeRate = 1;
		if (kpSize1 != kpSize2)
		{
			if (kpSize1 > kpSize2)
			{
				sizeRate = (double)kpSize1 / (double)kpSize2;
			}
			else
			{
				sizeRate = (double)kpSize2 / (double)kpSize1;
			}

			//特征点比例异常，退出
			if (sizeRate > 2.5)
			{
// 				m_mainWnd->AddTipInfo(std::string("特征点比例异常 ").append("s1:").append(std::to_string(kpts1.size())).append(" s2:").append(std::to_string(kpts2.size())).c_str());
				return 0.0;
			}
		}

		BFMatcher matcher(NORM_HAMMING);
		vector< vector<DMatch> > nn_matches;
		matcher.knnMatch(desc1, desc2, nn_matches, 2);

		vector<KeyPoint> matched1, matched2;
		for (size_t i = 0; i < nn_matches.size(); i++) {
			DMatch first = nn_matches[i][0];
			float dist1 = nn_matches[i][0].distance;
			float dist2 = nn_matches[i][1].distance;

			if (dist1 < nn_match_ratio * dist2) {
				matched1.push_back(kpts1[first.queryIdx]);
				matched2.push_back(kpts2[first.trainIdx]);
			}
		}

		vector<DMatch> good_matches;
		vector<KeyPoint> inliers1, inliers2;

		double rate = (double)matched1.size() / ((kpts1.size() + kpts2.size()) / 2.0);
		matchedSize = matched1.size();
#ifdef DEV_VER
		if (rate > 1.0)
		{
			m_mainWnd->AddTipInfo(std::string("s1:").append(std::to_string(kpts1.size())).append(" s2:").append(std::to_string(kpts2.size())).append(" m1:").append(std::to_string(matched1.size())).append(" m2:").append(std::to_string(matched2.size())).c_str());
		}
#endif // _DEBUG

		kpSize1 = (int)kpts1.size();
		kpSize2 = (int)kpts2.size();
		return rate;
	};

	//segmentation
	cv::Mat partialImg;
	x = input.xRate * s.x;
	y = input.yRate * s.y;
	int x2 = input.xRate2 * s.x;
	int y2 = input.yRate2 * s.y;
	cv::Rect rt2(x, y, (x2 - x), (y2 - y));
	img(rt2).copyTo(partialImg);

	double rate = compareFunc(img, partialImg);
	#ifdef DEV_VER
m_ui->edt_rate->setText(Left2Precision(rate).c_str());
#endif

	if (rate > input.cmpPicRate)
	{
// 		std::string strTmp = std::string("C:\\Users\\cdzha\\Desktop\\res_[").append(std::to_string(x)).append("_").append(std::to_string(y)).append("]_").append(std::to_string(rate)).append(".png");
// 		imwrite(strTmp.c_str(), partialImg);

		input.bFindPicFlag = true;
		//对比成功后点击图片的中心点
		if (input.bCmpPicCheckFlag)
		{
// 			Sleep(50); // 这里的延迟是为了应对，有时候刚刚切换到其他画面时，瞬间点击上面的按钮无效的情况（还是关闭掉，自己加延迟吧，否则像食物语关闭自动战斗，需要很快的时候又会有bug，所以延迟还是自己加）
			MouseClick(gameWnd, x + (x2 - x) / 2.0, y + (y2 - y) / 2.0);
		}

#ifdef DEV_VER
		m_mainWnd->AddTipInfo(std::string("cmp[").append(Left2Precision(rate)).append("|").append(std::to_string(GetTickCount() - dwTime)).append("|").append( std::to_string( kpSize1 ) ).append(":").append( std::to_string( kpSize2 ) ).append("-").append(std::to_string(matchedSize)).append("]-").append(input.picPath).append("_").append(Left2Precision(rOffsetRate)).append("_").append(Left2Precision(gOffsetRate)).append("_").append(Left2Precision(bOffsetRate)).c_str());
#endif
		m_mainWnd->AddTipInfo(std::string(input.comment).append("-成功").c_str());
	}

	dwTime = GetTickCount() - dwTime;
#ifdef DEV_VER
	m_ui->edt_costTime->setText(std::to_string(dwTime).append("|").append(Left2Precision(fBaseScaleFactor)).append("|").append(std::to_string(kpSize1)).c_str());
#endif
	return rate;
}
