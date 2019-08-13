#pragma once

#include "windows.h"
#include <unordered_map>
#include <string>
#include "QString"

#ifdef _DEBUG
#define DEV_VER
#endif

#define Q8(str) (QString::fromLocal8Bit(str))
#define TT_BEGIN char cTmp[MAX_PATH] = { 0 };\
				DWORD dwTime2 = GetTickCount();

#define TT_END(title) dwTime2 = GetTickCount() - dwTime2;\
sprintf_s(cTmp, MAX_PATH, "[%s] cost:%d\n", title, dwTime2);\
OutputDebugStringA(cTmp);\

#define DEFAULT_PATH ("./QtSave/")
#define DEFAULT_MODULE_PATH ("./QtSave/Module/")

struct CSize
{
	int			x;
	int			y;

	CSize()
		:x(0), y(0)
	{}
};

static std::string Left2Precision(double d)
{
	int iTmp = (int)(d * 100);
	int part1 = iTmp / 100;
	int part2 = iTmp % 100;
	int part3 = iTmp / 10;
	return std::string(std::to_string(part1)).append(".").append((0 == part3 ? "0" : "")).append(std::to_string(part2));
}
