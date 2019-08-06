#include "OpencvMgr.h"

OpencvMgr *OpencvMgr::m_singleton = nullptr;
unique_ptr<OpencvMgr> OpencvMgr::m_autoPtr(&OpencvMgr::Singleton());

using namespace cv;
using namespace std;

OpencvMgr::OpencvMgr()
{
}

OpencvMgr::~OpencvMgr()
{
}

OpencvMgr& OpencvMgr::Singleton()
{
	return (nullptr == m_singleton) ? *(m_singleton = new OpencvMgr) : *(m_singleton);
}

double OpencvMgr::CompareImgs()
{
	const char* strPic1 = "d:/3.png";
	const char* strPic2 = "d:/3-1.png";
	const char* strXml = "d:/H1to3p.xml";

	Mat img1 = imread(strPic1, IMREAD_GRAYSCALE);
	Mat img2 = imread(strPic2, IMREAD_GRAYSCALE);
	resize(img2, img2, Size(img1.cols, img1.rows));

	Mat homography;
	FileStorage fs(strXml, FileStorage::READ);
	fs.getFirstTopLevelNode() >> homography;

	vector<KeyPoint> kpts1, kpts2;
	Mat desc1, desc2;

	Ptr<AKAZE> akaze = AKAZE::create();
	akaze->detectAndCompute(img1, noArray(), kpts1, desc1);
	akaze->detectAndCompute(img2, noArray(), kpts2, desc2);

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

	double minDist = 999999.9;
	vector<DMatch> good_matches;
	vector<KeyPoint> inliers1, inliers2;
	for (size_t i = 0; i < matched1.size(); i++) {
		Mat col = Mat::ones(3, 1, CV_64F);
		col.at<double>(0) = matched1[i].pt.x;
		col.at<double>(1) = matched1[i].pt.y;

		col = homography * col;
		col /= col.at<double>(2);
		double dist = sqrt(pow(col.at<double>(0) - matched2[i].pt.x, 2) +
			pow(col.at<double>(1) - matched2[i].pt.y, 2));

		minDist = dist < minDist ? dist : minDist;
		if (dist < inlier_threshold) {
			int new_i = static_cast<int>(inliers1.size());
			inliers1.push_back(matched1[i]);
			inliers2.push_back(matched2[i]);
			good_matches.push_back(DMatch(new_i, new_i, 0));
		}
	}

// 	Mat res;
// 	drawMatches(img1, inliers1, img2, inliers2, good_matches, res);
// 	imwrite("akaze_result.png", res);

	auto rate = (double)matched1.size() / ((kpts1.size() + kpts2.size()) / 2.0);

// 	double inlier_ratio = inliers1.size() / (double)matched1.size();
// 	cout << "A-KAZE Matching Results" << endl;
// 	cout << "*******************************" << endl;
// 	cout << "# Min Dist:				\t" << minDist << endl;
// 	cout << "# Keypoints 1:                        \t" << kpts1.size() << endl;
// 	cout << "# Keypoints 2:                        \t" << kpts2.size() << endl;
// 	cout << "# Matches:                            \t" << matched1.size() << endl;
// 	cout << "# Average Matches:                    \t" << rate << endl;
// 	cout << "# Inliers:                            \t" << inliers1.size() << endl;
// 	cout << "# Inliers Ratio:                      \t" << inlier_ratio << endl;
// 	cout << endl;

// 	imshow("result", res);
// 	waitKey();

	return rate;
}
