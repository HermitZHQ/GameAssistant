#pragma once
#include "QWidget"
#include "QString"
#include "PreDef.h"
#include "Input.h"
#include "QThread"
#include "QMenu"
#include <mutex>

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
	WOLT_60,//薇欧蕾特副本
	MAOMAO_60,
	Evelyn_60_1,
	Evelyn_60_2,
	Evelyn_60_3,
};

enum ZZ_Dev
{
	Normal20,
	Normal10,
	Limit,
};

enum ZZ_Huodong
{
	t_1_5,
	t_2_5,
	t_3_5,
	t_4_5,
};

//参数从1开始，0为默认值，不进行处理
//相同范围内的枚举连续写，这样比较的时候可以写成范围比较
enum ZZ_Map_Param
{
	//------非战斗中情况
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
	Evelyn_Story = 11,//个人剧情
	StartQuestUI,//停止在开始任务的图标上（一般是刚刚从一场战斗退出）
	Battle_Main_1,//只查到第一个匹配
	Dev_ing,//开发中
	Fb_Count_0,//副本次数不足
	Daily_Count_0,//每次打完


	//------战斗中的情况
	Battle_deploy = 100,//部署阶段
	Battle_deployed,//部署完成阶段（敌人开始行动，但是未接触没有真正进入战斗）
	Battle_prepare,//战斗准备阶段（双方接触，但是未点击开始战斗时）
	Battle,//真正在战斗中
	Battle_end,//整场战斗结束的结算界面
	State_failed = 105,//关卡失败界面
	Escort_failed,//货送失败
};

//参数从1开始，0为默认值，不进行处理
enum ZZ_Cmp_Param
{
	//使用16进制的0x1，0x2，0x4，0x8，0x10，0x20，0x40，0x80....循环做标记，这样不会重复
	Map_Reward = 0x1,
	Map_Daily = 0x2,
	Map_Evelyn = 0x4,
	Map_Wolt = 0x8,//薇欧蕾特地图标记
	Map_Maomao = 0x10,//喵喵副本
};

class PlayerUI;
class ThreadMapStatus : public QThread
{
	Q_OBJECT
		void run() override;
public:
	ThreadMapStatus( QObject *parent = nullptr )
		:QThread( parent )
		, m_parent( ( PlayerUI* )parent )
	{}
signals:
	void resultReady( const QString &s );

private:
	PlayerUI										*m_parent;
};

class ThreadMapRecognize : public QThread
{
	Q_OBJECT
		void run() override;
public:
	ThreadMapRecognize( QObject *parent = nullptr )
		:QThread( parent )
		, m_parent( ( PlayerUI* )parent )
	{}
signals:
	void resultReady( const QString &s );

private:
	PlayerUI										*m_parent;
};

class ThreadNextStep : public QThread
{
	Q_OBJECT
		void run() override;
public:
	ThreadNextStep( QObject *parent = nullptr )
		:QThread( parent )
		, m_parent( ( PlayerUI* )parent )
	{}
signals:
	void resultReady( const QString &s );

private:
	PlayerUI										*m_parent;
};

class ThreadMapPosSelect : public QThread
{
	Q_OBJECT
		void run() override;
public:
	ThreadMapPosSelect( QObject *parent = nullptr )
		:QThread( parent )
		, m_parent( ( PlayerUI* )parent )
	{}
signals:
	void resultReady( const QString &s );

private:
	PlayerUI										*m_parent;
};

class MainWindow;
class PlayerUI : public QWidget
{
	Q_OBJECT
	QThread workerThread1;
public:
	PlayerUI(MainWindow *wnd);
	~PlayerUI();

	const inline Ui::PlayerUI* GetUI() { return m_ui; }

	void Init();

	void UpdateAllScript();
	void UpdateMapStatusRecognizeScript();
	void UpdateNextStepScript();
	void UpdateMapPositionSelectScript();
	//根据用户设置，传入对比参数，忽略不需要对比的地图（这样我们就可以把所有的地图对比都放在一个脚本了，之前加的对比参数就有用了）
	void UpdateAllMapRecognizeAndBattleScript();

	inline bool GetRunThreadFlag() {
		return m_bRunThreadFlag;
	}

	inline bool GetPauseMapStatusFlag() {
		return m_bPauseMapStatusFlag;
	}

	inline bool GetPauseMapRecognizeFlag() {
		return m_bPauseMapRecognizeFlag;
	}

	inline bool GetPauseMapPosSelectFlag() {
		return m_bPauseMapPosSelectFlag;
	}

	inline bool GetPauseNextStepFlag() {
		return m_bPauseNextStepFlag;
	}

public slots:
	void handleResults(const QString &);
	void ClearTipInfo();

signals:
	void operate(const QString &);

protected:
	void UpdateMapStatusInputDataVector( int cmpParam );
	void UpdateMapRecognizeAndBattleInputDataVector( int cmpParam );
	void UpdateNormalInputDataVector(int cmpParam, std::vector<InputData> &inputVec);
	void StopScritp();
	void StartScript();

	//pos select
	void ResetPosSelectFlags();
	void GotoBattleMain();
	void ForceGotoBattleMain();
	void ForceQuitBattle();
	void GotoRewardFirstIcon();
	void GotoDailyFirstIcon();
	void GotoEmergency();
	void GotoFb();

	void GotoDev();

	inline bool NotInBattleFlag() {
		return ( m_mapStatusOutputParam >= ZZ_Map_Param::Lobby
			&& m_mapStatusOutputParam <= ZZ_Map_Param::Daily_Count_0 );
	}

private slots:
	void OnBtnStop();
	void OnBtnStartAuto();
	void OnBtnToggleBattleOptions();
	void OnBtnToggleNoneBattleOptions();
	void UpdateMainThreadTimerReset();

	//----活动
	void OnBtnHuodong1();

private:
	bool											m_bInitFlag;
	Ui::PlayerUI									*m_ui;
	MainWindow										*m_mainWnd;
	QMenu											m_menu;

	std::unordered_map<ZZ_Specific, QString>		m_specificLevelScriptMap;
	std::unordered_map<ZZ_Delegate, QString>		m_specificDelegateScriptMap;
	std::unordered_map<ZZ_Dev, QString>				m_specificDevScriptMap;
	std::unordered_map<ZZ_Huodong, QString>			m_specificHuodongScriptMap;

	struct TimerInfo 
	{
		QTimer										timer;
		int											interval;
		bool										bResetFlag;

		TimerInfo()
			:bResetFlag(false)
		{}

		void SetResetFlag()
		{
			bResetFlag = true;
		}

		void CheckReset()
		{
			if ( bResetFlag )
			{
				bResetFlag = false;

				timer.start( interval );
			}
		}
	};

	//具体游戏相关挂机设置
	//间隔都以分钟计数
	struct SettingInfo
	{
	public:
		int						interval;
		bool					bShouldExecFlag;
		bool					bResetFlag;
		QTimer					timer;

		SettingInfo()
			:bResetFlag(false)
			, bShouldExecFlag(false)
		{
			timer.connect( &timer, &QTimer::timeout, [&]() {

				bShouldExecFlag = true;
			} );
		}

		void StartAutoHandle() 
		{
			bShouldExecFlag = false;
			bResetFlag = false;

			timer.start( interval * ( 60 * 1000 ) );
		}

		void SetResetFlag()
		{
			bResetFlag = true;
		}

		void CheckReset()
		{
			if ( bResetFlag )
			{
				bShouldExecFlag = false;
				bResetFlag = false;

				timer.start( interval * ( 60 * 1000 ) );
			}
		}

		void Stop()
		{
			bShouldExecFlag = false;
			bResetFlag = false;
			timer.stop();
		}
	};
	SettingInfo										m_emergencySetting;
	SettingInfo										m_devSetting;
	SettingInfo										m_delegateSetting;
	SettingInfo										m_recruitSetting;

	TimerInfo										m_checkNoneBattleBlockTimer;
	TimerInfo										m_checkBattleBlockTimer;

	QTimer											m_mainThreadResetTimer;
	std::mutex										m_loadScriptMutex;

	bool											m_bInBattleFlag;
	bool											m_bRunThreadFlag;
	bool											m_bPauseMapStatusFlag;
	bool											m_bPauseMapRecognizeFlag;
	bool											m_bPauseMapPosSelectFlag;
	bool											m_bPauseNextStepFlag;

	//流程控制标记
	bool											m_bToBattleMainFlag;//是否已跳转到战斗主界面的标记（任务列表收起状态）
	bool											m_bToBattleRewardFlag;//进入赏金任务标记（直到进入战斗中才算完全进入）
	bool											m_bToBattleDailyFlag;//进入日常标记
	bool											m_bToBattleEmergencyFlag;
	bool											m_bToBattleFbFlag;

	bool											m_bFbFinishedFlag;//检查fb次数用尽
	bool											m_bDailyFinishedFlag;//每日已经打完

	bool											m_bToDev;

	//此inputVec专门用于识别目前的游戏状态，比如在大厅，在机库，在准备战斗，正在战斗中等
	ThreadMapStatus									m_threadMapStatus;
	std::vector<InputData>							m_mapStatusInputVec;
	int												m_mapStatusCmpParam;
	int												m_mapStatusOutputParam;

	int												m_lastStatusParam;

	//此inputVec专门用于开启挂机后来点击下一步，开始战斗，以及战斗结算还有跳过对话，跳过错误用，就不用一直在其他脚本中重复写了
	ThreadNextStep									m_threadNextStep;
	std::vector<InputData>							m_nextStepInputVec;

	//此inputVec专门用于根据挂机设置和获取到的游戏状态来自动切换于各个脚本之间（比如切换找赏金图片，找每日图片，找特定fb图标）
	ThreadMapPosSelect								m_threadMapPosSelect;
	std::vector<InputData>							m_mapPosSelectInputVec;

	//此inputVec专门用于侦测所有地图（加对比参数忽略特定地图）
	ThreadMapRecognize								m_threadMapRecognize;
	std::vector<InputData>							m_mapRecognizeInputVec;
};