#pragma once
#include "OpencvMgr.h"
#include "PreDef.h"
#include "PicCompareStrategy.h"

//重装战姬图片对比策略，使用策略模式，方便以后扩展其他游戏的挂机
class ZZPicCompareStrategy : public PicCompareStrategy
{
	struct PicCache 
	{
		cv::Mat			img_rgb;
		cv::Mat			img_gray;
	};
public:
	ZZPicCompareStrategy();
	virtual ~ZZPicCompareStrategy();

	virtual double HandlePicCompare(InputData &input, HWND gameWnd, const CSize &s) override;

private:
	HWND												m_gameWnd;
	HDC													m_dc;
	HDC													m_compatibleDc;
	HBITMAP												m_compatibleBmp;
	char												*m_buf;
};

