#pragma once
#include "QWidget"
#include "QString"
#include "PreDef.h"
#include "Input.h"

namespace Ui {
	class PlayerUI;
}

enum ZZ_Reward
{
	A4
};

enum ZZ_Delegate
{
	Slowest
};

enum ZZ_Specific
{
	Nefud_1E,
	Hecate_1E,
	WOLT_50,//薇欧蕾特副本
	WOLT_60,//薇欧蕾特副本
	Evelyn_50,//伊芙琳50级副本
	Evelyn_60,//伊芙琳60级副本
};

enum ZZ_Dev
{
	Limit,
	Normal10,
	Normal20,
};

//参数从1开始，0为默认值，不进行处理
//相同范围内的枚举连续写，这样比较的时候可以写成范围比较
enum ZZ_Map_Param
{
	Lobby = 1,//大厅
	Dev,//开发
	Team,//机库
	Recruit,//招募
	Member,//机师
	Battle_Main,//战斗主界面
	Battle_A1 = 7,//战斗A1界面
	Battle_A2,
	Battle_A3,
	Battle_A4,
	Battle_deploy = 11,//部署阶段
	Battle_deployed = 12,//部署完成阶段（敌人开始行动，但是未接触没有真正进入战斗）
	Battle_prepare,//战斗准备阶段（双方接触，但是未点击开始战斗时）
	Battle,//真正在战斗中
	Battle_end = 15,//整场战斗结束的结算界面
	State_failed,//关卡失败界面
};

//参数从1开始，0为默认值，不进行处理
enum ZZ_Cmp_Param
{
	Map_Reward_A1 = 1,
	Map_Reward_A2,
	Map_Reward_A3,
	Map_Reward_A4,
	Map_Daily,
};

class MainWindow;
class PlayerUI : public QWidget
{
	Q_OBJECT
public:
	PlayerUI(MainWindow *wnd);
	~PlayerUI();

	const inline Ui::PlayerUI* GetUI() { return m_ui; }

	void Init();

	void UpdateMapStatusRecognizeScript();
	void UpdateNextStepScript();
	void UpdateMapPositionSelectScript();
	//根据用户设置，传入对比参数，忽略不需要对比的地图（这样我们就可以把所有的地图对比都放在一个脚本了，之前加的对比参数就有用了）
	void UpdateAllMapRecognizeAndBattleScript();

protected:
	void UpdateMapRecognizeInputDataVector(int cmpParam);
	void UpdateNormalInputDataVector(int cmpParam, std::vector<InputData> &inputVec);
	void StopScritp();
	void StartScript();

private slots:
	void OnBtnStop();
	void OnBtnA4Reward();
	void OnBtnDaily();
	void OnBtnNextStep();
	void OnBtnDelegate();
	void OnBtnDev();
	void OnBtnRecruit();
	void OnBtnSpecific();
	void OnBtnStartAuto();

private:
	Ui::PlayerUI									*m_ui;
	MainWindow										*m_mainWnd;
	std::unordered_map<ZZ_Specific, QString>		m_specificLevelScriptMap;
	std::unordered_map<ZZ_Delegate, QString>		m_specificDelegateScriptMap;
	std::unordered_map<ZZ_Dev, QString>				m_specificDevScriptMap;

	//此inputVec专门用于识别目前的游戏状态，比如在大厅，在机库，在准备战斗，正在战斗中等
	QTimer											m_mapStatusTimer;
	std::vector<InputData>							m_mapStatusInputVec;
	int												m_mapStatusCmpParam;
	int												m_mapStatusOutputParam;

	int												m_lastStatusParam;

	//此inputVec专门用于开启挂机后来点击下一步，开始战斗，以及战斗结算还有跳过对话，跳过错误用，就不用一直在其他脚本中重复写了
	QTimer											m_nextStepTimer;
	std::vector<InputData>							m_nextStepInputVec;
	int												m_nextStepCmpParam;
	int												m_nextStepOutputParam;

	//此inputVec专门用于根据挂机设置和获取到的游戏状态来自动切换于各个脚本之间（比如切换找赏金图片，找每日图片，找特定fb图标）
	QTimer											m_mapPosSelectTimer;
	std::vector<InputData>							m_mapPosSelectInputVec;

	//此inputVec专门用于侦测所有地图（加对比参数忽略特定地图）
	QTimer											m_mapRecognizeTimer;
	std::vector<InputData>							m_mapRecognizeInputVec;
};

