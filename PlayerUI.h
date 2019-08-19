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
	WOLT_50,//ޱŷ���ظ���
	WOLT_60,//ޱŷ���ظ���
	Evelyn_50,//��ܽ��50������
	Evelyn_60,//��ܽ��60������
};

enum ZZ_Dev
{
	Limit,
	Normal10,
	Normal20,
};

//������1��ʼ��0ΪĬ��ֵ�������д���
//��ͬ��Χ�ڵ�ö������д�������Ƚϵ�ʱ�����д�ɷ�Χ�Ƚ�
enum ZZ_Map_Param
{
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
	Battle_deploy = 11,//����׶�
	Battle_deployed = 12,//������ɽ׶Σ����˿�ʼ�ж�������δ�Ӵ�û����������ս����
	Battle_prepare,//ս��׼���׶Σ�˫���Ӵ�������δ�����ʼս��ʱ��
	Battle,//������ս����
	Battle_end = 15,//����ս�������Ľ������
	State_failed,//�ؿ�ʧ�ܽ���
};

//������1��ʼ��0ΪĬ��ֵ�������д���
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
	//�����û����ã�����ԱȲ��������Բ���Ҫ�Աȵĵ�ͼ���������ǾͿ��԰����еĵ�ͼ�Աȶ�����һ���ű��ˣ�֮ǰ�ӵĶԱȲ����������ˣ�
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

	//��inputVecר������ʶ��Ŀǰ����Ϸ״̬�������ڴ������ڻ��⣬��׼��ս��������ս���е�
	QTimer											m_mapStatusTimer;
	std::vector<InputData>							m_mapStatusInputVec;
	int												m_mapStatusCmpParam;
	int												m_mapStatusOutputParam;

	int												m_lastStatusParam;

	//��inputVecר�����ڿ����һ����������һ������ʼս�����Լ�ս�����㻹�������Ի������������ã��Ͳ���һֱ�������ű����ظ�д��
	QTimer											m_nextStepTimer;
	std::vector<InputData>							m_nextStepInputVec;
	int												m_nextStepCmpParam;
	int												m_nextStepOutputParam;

	//��inputVecר�����ڸ��ݹһ����úͻ�ȡ������Ϸ״̬���Զ��л��ڸ����ű�֮�䣨�����л����ͽ�ͼƬ����ÿ��ͼƬ�����ض�fbͼ�꣩
	QTimer											m_mapPosSelectTimer;
	std::vector<InputData>							m_mapPosSelectInputVec;

	//��inputVecר������������е�ͼ���ӶԱȲ��������ض���ͼ��
	QTimer											m_mapRecognizeTimer;
	std::vector<InputData>							m_mapRecognizeInputVec;
};

