#pragma once
#include <windows.h>
#include "PreDef.h"

#define PATH_LEN (MAX_PATH)

#pragma pack(1)
enum InputType
{
	Mouse,
	Keyboard,
	Pic,//图片识别
	StopScript,//停止脚本
	Log,//输出日志，方便玩家查看脚本进度
	Jump,//跳转命令，同时支持index和module跳转
	Wait,//等待命令，当时把delay的类型设置为short了，用于延时不够，而且类型也不对，因为delay后总要做点什么
};

enum OpType
{
	Click,
	Press,
	Move,
	Release,
	Wheel,
};

struct InputData 
{
	InputType			type;
	OpType				opType;
	char				vk;
	short				x;
	short				y;
	short				x2;//第二组坐标用于组合矩形使用
	short				y2;
	float				xRate;//坐标的百分比，这样即使变化了画面也能点击到正确的位置
	float				yRate;
	float				xRate2;//第二组坐标的百分比
	float				yRate2;
	short				delay;
	DWORD				startTime;
	bool				bFinishFlag;
	bool				bInitStartTimeFlag;
	bool				bFindPicFlag;
	float				cmpPicRate;
	char				picPath[PATH_LEN];
	int					findPicOvertime;
	bool				bFindPicOvertimeFlag;
	int					findPicSucceedJumpIndex;
	int					findPicOvertimeJumpIndex;
	char				findPicSucceedJumpModule[PATH_LEN];//如果跳转指令索引为0xffff(65535)的话，我们就解析为跳转模块了
	char				findPicOvertimeJumpModule[PATH_LEN];

	//--------------扩展数据（每次扩展记得减少reserve的大小，之前预留的大小为path_len*3）
	bool				bCmpPicCheckFlag;
	char				comment[PATH_LEN];
	short				repeatCount;
	short				alreadyRepeatCount;
	int					cmpParam;
	int					outputParam;
	int					waitTime;//秒

	bool				bEnableThresholdCmpFlag;
	BYTE				thresholdCmpLowColor[3];
	BYTE				thresholdCmpHighColor[3];
	BYTE				thresholdCmpReplaceColor[3];

	//------预留数据
	//.....预留数据扩展，免得每次加入新数据，之前的保存文件都要报废（扩展原大小应该是PATH_LEN * 3，目前已经减去了所有扩展进来的大小，只要不加大块数据，这个预留应该是足够了）
	char				reserve[PATH_LEN * 2 
		- (1 + sizeof(short) * 2 + sizeof(int) * 3 + sizeof(bool) + sizeof(BYTE) * 9)];

	InputData()
		:type(Mouse), opType(Click), vk(0), x(0), y(0)
		, delay(10), startTime(GetTickCount()), cmpPicRate(0.65f)
		, xRate(0.0f), yRate(0.0f), bFinishFlag(false)
		, bInitStartTimeFlag(false), bFindPicFlag(false)
		, findPicOvertime(-1), bFindPicOvertimeFlag(false)
		, bCmpPicCheckFlag(true)
		, findPicSucceedJumpIndex(-1), findPicOvertimeJumpIndex(-1), repeatCount(1)
		, alreadyRepeatCount(0)
		, bEnableThresholdCmpFlag(false)
	{
		ZeroMemory(picPath, PATH_LEN);
		ZeroMemory(findPicSucceedJumpModule, PATH_LEN);
		ZeroMemory(findPicOvertimeJumpModule, PATH_LEN);
		ZeroMemory(comment, PATH_LEN);
		ZeroMemory(reserve, sizeof(reserve));
		ZeroMemory(thresholdCmpLowColor, sizeof(thresholdCmpLowColor));
		ZeroMemory(thresholdCmpHighColor, sizeof(thresholdCmpHighColor));
		ZeroMemory(thresholdCmpReplaceColor, sizeof(thresholdCmpReplaceColor));
	}
};