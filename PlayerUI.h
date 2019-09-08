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
	WOLT_60,//ޱŷ���ظ���
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

//������1��ʼ��0ΪĬ��ֵ�������д���
//��ͬ��Χ�ڵ�ö������д�������Ƚϵ�ʱ�����д�ɷ�Χ�Ƚ�
enum ZZ_Map_Param
{
	//------��ս�������
	Lobby = 1,//����
	Dev,//����
	Team,//����
	Recruit,//��ļ
	Member,//��ʦ
	Battle_Main,//ս��������
	Battle_A1 = 7,//ս��A1����
	Battle_A2,
	Battle_A3,
	Battle_A4,
	Evelyn_Story = 11,//���˾���
	StartQuestUI,//ֹͣ�ڿ�ʼ�����ͼ���ϣ�һ���Ǹոմ�һ��ս���˳���
	Battle_Main_1,//ֻ�鵽��һ��ƥ��
	Dev_ing,//������
	Fb_Count_0,//������������
	Daily_Count_0,//ÿ�δ���


	//------ս���е����
	Battle_deploy = 100,//����׶�
	Battle_deployed,//������ɽ׶Σ����˿�ʼ�ж�������δ�Ӵ�û����������ս����
	Battle_prepare,//ս��׼���׶Σ�˫���Ӵ�������δ�����ʼս��ʱ��
	Battle,//������ս����
	Battle_end,//����ս�������Ľ������
	State_failed = 105,//�ؿ�ʧ�ܽ���
	Escort_failed,//����ʧ��
};

//������1��ʼ��0ΪĬ��ֵ�������д���
enum ZZ_Cmp_Param
{
	//ʹ��16���Ƶ�0x1��0x2��0x4��0x8��0x10��0x20��0x40��0x80....ѭ������ǣ����������ظ�
	Map_Reward = 0x1,
	Map_Daily = 0x2,
	Map_Evelyn = 0x4,
	Map_Wolt = 0x8,//ޱŷ���ص�ͼ���
	Map_Maomao = 0x10,//��������
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
	//�����û����ã�����ԱȲ��������Բ���Ҫ�Աȵĵ�ͼ���������ǾͿ��԰����еĵ�ͼ�Աȶ�����һ���ű��ˣ�֮ǰ�ӵĶԱȲ����������ˣ�
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

	//----�
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

	//������Ϸ��عһ�����
	//������Է��Ӽ���
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

	//���̿��Ʊ��
	bool											m_bToBattleMainFlag;//�Ƿ�����ת��ս��������ı�ǣ������б�����״̬��
	bool											m_bToBattleRewardFlag;//�����ͽ������ǣ�ֱ������ս���в�����ȫ���룩
	bool											m_bToBattleDailyFlag;//�����ճ����
	bool											m_bToBattleEmergencyFlag;
	bool											m_bToBattleFbFlag;

	bool											m_bFbFinishedFlag;//���fb�����þ�
	bool											m_bDailyFinishedFlag;//ÿ���Ѿ�����

	bool											m_bToDev;

	//��inputVecר������ʶ��Ŀǰ����Ϸ״̬�������ڴ������ڻ��⣬��׼��ս��������ս���е�
	ThreadMapStatus									m_threadMapStatus;
	std::vector<InputData>							m_mapStatusInputVec;
	int												m_mapStatusCmpParam;
	int												m_mapStatusOutputParam;

	int												m_lastStatusParam;

	//��inputVecר�����ڿ����һ����������һ������ʼս�����Լ�ս�����㻹�������Ի������������ã��Ͳ���һֱ�������ű����ظ�д��
	ThreadNextStep									m_threadNextStep;
	std::vector<InputData>							m_nextStepInputVec;

	//��inputVecר�����ڸ��ݹһ����úͻ�ȡ������Ϸ״̬���Զ��л��ڸ����ű�֮�䣨�����л����ͽ�ͼƬ����ÿ��ͼƬ�����ض�fbͼ�꣩
	ThreadMapPosSelect								m_threadMapPosSelect;
	std::vector<InputData>							m_mapPosSelectInputVec;

	//��inputVecר������������е�ͼ���ӶԱȲ��������ض���ͼ��
	ThreadMapRecognize								m_threadMapRecognize;
	std::vector<InputData>							m_mapRecognizeInputVec;
};