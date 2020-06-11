#pragma once
#include <windows.h>
#include "PreDef.h"

#define PATH_LEN (MAX_PATH)

#pragma pack(1)
enum InputType
{
	Mouse,
	Keyboard,
	Pic,//ͼƬʶ��
	StopScript,//ֹͣ�ű�
	Log,//�����־��������Ҳ鿴�ű�����
	Jump,//��ת���ͬʱ֧��index��module��ת
	Wait,//�ȴ������ʱ��delay����������Ϊshort�ˣ�������ʱ��������������Ҳ���ԣ���Ϊdelay����Ҫ����ʲô
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
	short				x2;//�ڶ�������������Ͼ���ʹ��
	short				y2;
	float				xRate;//����İٷֱȣ�������ʹ�仯�˻���Ҳ�ܵ������ȷ��λ��
	float				yRate;
	float				xRate2;//�ڶ�������İٷֱ�
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
	char				findPicSucceedJumpModule[PATH_LEN];//�����תָ������Ϊ0xffff(65535)�Ļ������Ǿͽ���Ϊ��תģ����
	char				findPicOvertimeJumpModule[PATH_LEN];

	//--------------��չ���ݣ�ÿ����չ�ǵü���reserve�Ĵ�С��֮ǰԤ���Ĵ�СΪpath_len*3��
	bool				bCmpPicCheckFlag;
	char				comment[PATH_LEN];
	short				repeatCount;
	short				alreadyRepeatCount;
	int					cmpParam;
	int					outputParam;
	int					waitTime;//��

	bool				bEnableThresholdCmpFlag;
	BYTE				thresholdCmpLowColor[3];
	BYTE				thresholdCmpHighColor[3];
	BYTE				thresholdCmpReplaceColor[3];

	//------Ԥ������
	//.....Ԥ��������չ�����ÿ�μ��������ݣ�֮ǰ�ı����ļ���Ҫ���ϣ���չԭ��СӦ����PATH_LEN * 3��Ŀǰ�Ѿ���ȥ��������չ�����Ĵ�С��ֻҪ���Ӵ�����ݣ����Ԥ��Ӧ�����㹻�ˣ�
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