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
	void UpdateAutoScript();
	void UpdateRewardMapRecognizeScript();

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
	QTimer											m_mapRecognizeTimer;
	std::vector<InputData>							m_mapRecognizeInputVec;
	int												m_mapRecognizeCmpParam;
	int												m_mapRecognizeOutputParam;

	int												m_lastOutputParam;

	//此inputVec专门用于开启挂机后来点击下一步，开始战斗，以及战斗结算用，就不用一直在其他脚本中重复写了
	QTimer											m_nextStepTimer;
	std::vector<InputData>							m_nextStepInputVec;
	int												m_nextStepCmpParam;
	int												m_nextStepOutputParam;

	//此inputVec专门用于根据挂机设置和获取到的游戏状态来自动切换于各个脚本之间
	QTimer											m_autoTimer;
	std::vector<InputData>							m_autoInputVec;

	//此inputVec专门用于侦测所有赏金地图
	QTimer											m_rewardMapRecognizeTimer;
	std::vector<InputData>							m_rewardMapRecognizeInputVec;
};

