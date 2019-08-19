#include "PlayerUI.h"
#include "ui_player.h"
#include "mainwindow.h"
#include "PreDef.h"

PlayerUI::PlayerUI(MainWindow *wnd) :
	m_ui(new Ui::PlayerUI)
	, m_mainWnd(wnd)
	, m_specificLevelScriptMap({
		{Nefud_1E, QString(DEFAULT_PATH).append("zz_map_nefud-1e")},
		{Hecate_1E, QString(DEFAULT_PATH).append("zz_map_hecate-1e")},
		{WOLT_50, QString(DEFAULT_PATH).append("zz_map_fb_wolt50")},
		{WOLT_60, QString(DEFAULT_PATH).append("zz_map_fb_wolt60")},
		{Evelyn_50, QString(DEFAULT_PATH).append("zz_map_fb_evelyn_50")},
		{Evelyn_60, QString(DEFAULT_PATH).append("zz_map_fb_evelyn_60")},
		})

	, m_specificDelegateScriptMap({
		{Slowest, QString(DEFAULT_PATH).append("delegate_slowest")},
		})

	, m_specificDevScriptMap({
		{Limit, QString(DEFAULT_PATH).append("dev_limit")},
		{Normal10, QString(DEFAULT_PATH).append("dev_normal_10")},
		{Normal20, QString(DEFAULT_PATH).append("dev_normal_20")},
		})
	, m_mapStatusCmpParam(-1)
	, m_mapStatusOutputParam(0)
	, m_lastStatusParam(-1)
{
	m_ui->setupUi(this);
}

PlayerUI::~PlayerUI()
{
	delete m_ui;
}

void PlayerUI::Init()
{
	//���Ӷ�Ӧtimer�ĺ���
	m_mapStatusTimer.connect(&m_mapStatusTimer, &QTimer::timeout, this, &PlayerUI::UpdateMapStatusRecognizeScript);
	m_nextStepTimer.connect(&m_nextStepTimer, &QTimer::timeout, this, &PlayerUI::UpdateNextStepScript);
	m_mapPosSelectTimer.connect(&m_mapPosSelectTimer, &QTimer::timeout, this, &PlayerUI::UpdateMapPositionSelectScript);
	m_mapRecognizeTimer.connect(&m_mapRecognizeTimer, &QTimer::timeout, this, &PlayerUI::UpdateAllMapRecognizeAndBattleScript);

	//Ԥ�ȳ�ʼ������仯��inputVec
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("map_status_recognize").c_str(), m_mapStatusInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("next_step").c_str(), m_nextStepInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
}

void PlayerUI::UpdateMapStatusRecognizeScript()
{
	UpdateMapRecognizeInputDataVector(m_mapStatusCmpParam);

	//����״̬��ʼ�͹رն�Ӧ��timerִ�нű������Ч�ʣ����ٴ�����
	if (m_mapStatusOutputParam >= ZZ_Map_Param::Battle_deploy
		&& m_mapStatusOutputParam <= ZZ_Map_Param::Battle_end)
	{
		if (!m_mapRecognizeTimer.isActive())
		{
			//��Ϊս���ű���ɺ����ڶ���ֹͣ��������ת�ص�ͼ���ң���������Ƚϼ򵥣������漰����ת�ص�ͼ���Һ��ֽ���ս���ű������⣬������Ϸ��ߵĽű����¶������Ķ���ֹͣ�ű�����ֹͣtimer�����ǰ�inputVec��գ����Ե�������Ҫ�ٴο�����ͼʶ���ʱ��������Ҫ�ȼ���ʶ���ļ�����Ϊ��ʱ��vec�ǿգ�
			m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
			m_mapRecognizeTimer.start(2);
			m_mainWnd->AddTipInfo("�ѽ���ս������");
		}

		if (m_mapPosSelectTimer.isActive())
		{
			m_mapPosSelectTimer.stop();
		}
	}
	else if (m_mapStatusOutputParam >= ZZ_Map_Param::Lobby
		&& m_mapStatusOutputParam <= ZZ_Map_Param::Battle_A4)
	{
		if (m_mapRecognizeTimer.isActive())
		{
			m_mapRecognizeTimer.stop();
			m_mainWnd->AddTipInfo("�Ѵ�ս�������˳�");
		}

		if (!m_mapPosSelectTimer.isActive())
		{
			m_mapPosSelectTimer.start(2);
		}
	}
}

void PlayerUI::UpdateNextStepScript()
{
	if (m_nextStepInputVec.size())
	{
		UpdateNormalInputDataVector(m_nextStepCmpParam, m_nextStepInputVec);
	}
}

void PlayerUI::UpdateMapPositionSelectScript()
{
	//������Ϸ״̬����̬�����л������ű�

	//�龰�����仯ʱ�����滻һ�νű�
	if (m_lastStatusParam != m_mapStatusOutputParam)
	{
		m_lastStatusParam = m_mapStatusOutputParam;

		//�ͽ�
		if (m_ui->chk_reward->isChecked())
		{
			if (ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam)
			{
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_battle_findIcon_a4_reward").c_str(), m_mapPosSelectInputVec);
			}
			else if (m_mapStatusOutputParam >= ZZ_Map_Param::Lobby
				&& m_mapStatusOutputParam <= ZZ_Map_Param::Battle_A4
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main)
			{
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("to_battle_main").c_str(), m_mapPosSelectInputVec);
			}
			else
			{
				if (0 != m_lastStatusParam)
				{
					m_mainWnd->AddTipInfo(std::string("��ǰ�����޷���תִ���ͽ�����_").append(std::to_string(m_lastStatusParam)).c_str());
				}
			}
		}
	}

	if (m_mapPosSelectInputVec.size())
	{
		UpdateNormalInputDataVector(-1, m_mapPosSelectInputVec);
	}
}

void PlayerUI::UpdateAllMapRecognizeAndBattleScript()
{
	if (m_mapRecognizeInputVec.size())
	{
		//����Ĳ��������û��Ĺ�ѡ�任��Ŀǰ��ʱΪ-1
		UpdateNormalInputDataVector(-1, m_mapRecognizeInputVec);
	}
}

void PlayerUI::UpdateMapRecognizeInputDataVector(int cmpParam)
{
	if (m_mapStatusInputVec.size() == 0)
	{
		m_mainWnd->AddTipInfo("�ű���������Ϊ0���߳��˳�...");
		return;
	}

	//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
	m_mainWnd->UpdateGameWindowSize();
	//ǿ�Ƹ��´������ݣ����㴰����С��
// 	::SetActiveWindow(m_hWnd);
// 	InvalidateRect(m_hWnd, nullptr, TRUE);
// 	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
// 	PostMessage(m_hWnd, WM_ACTIVATE, 0, 0);
// 	PostMessage(m_hWnd, WM_ERASEBKGND, 0, 0);
// 	PostMessage(m_hWnd, WM_PAINT, 0, 0);

	//��Ҫ�Ѹ���������bottom���ԣ��Ų��ᵯ������
// 	::SetWindowPos( m_hParentWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
	bool bAllFinishedFlag = true;
	int index = -1;
	for (auto &input : m_mapStatusInputVec)
	{
		++index;
		//�жϲ����Ѿ����
		if ((input.bFinishFlag && input.type != Pic)
			|| (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag))
			|| (-1 != cmpParam && input.cmpParam != cmpParam))
		{
			continue;
		}

		//��ʼ�����뿪ʼʱ��
		if (!input.bInitStartTimeFlag)
		{
			input.bInitStartTimeFlag = true;
			input.startTime = GetTickCount();
		}

		bAllFinishedFlag = false;

		//�ж��ӳ�
		if (GetTickCount() - input.startTime < (DWORD)input.delay)
			break;

		//��ѯͼƬ�Ƿ�ʱ
		if (input.findPicOvertime != -1 && InputType::Pic == input.type && (GetTickCount() - input.startTime > (DWORD)(input.findPicOvertime + input.delay)))
		{
			input.bFindPicOvertimeFlag = true;
			//�жϳ�ʱָ����ת
			if (-1 != input.findPicOvertimeJumpIndex)
			{
				(0xffff != input.findPicOvertimeJumpIndex) ? m_mainWnd->JumpInput(input.findPicOvertimeJumpIndex, m_mapStatusInputVec) :
					(m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicOvertimeJumpModule, m_mapStatusInputVec));
				break;
			}
			continue;
		}

		switch (input.type)
		{
		case Mouse:
			m_mainWnd->HandleMouseInput(input);
			break;
		case Keyboard:
			m_mainWnd->HandleKeyboardInput(input);
			break;
		case Pic:
			m_mainWnd->HandleGameImgCompare(input);
			break;
		case StopScript:
		{
			m_mainWnd->AddTipInfo("�ű���������ϣ����ֶ�ѡ�������ű�ִ��");
		}
		break;
		case Log:
		{
			m_mainWnd->AddTipInfo(input.comment);
		}
		break;
		default:
			break;
		}

		//���û���ҵ�ͼƬ������������������һ��
		if (InputType::Pic == input.type && !input.bFindPicFlag)
		{
			break;
		}
		//ͼƬ�Աȳɹ�ָ����ת
		else if (InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex)
		{
			m_mapStatusOutputParam = input.outputParam;

			(0xffff != input.findPicSucceedJumpIndex) ? m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, m_mapStatusInputVec) :
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, m_mapStatusInputVec);
			break;
		}
		else if (InputType::Pic == input.type && input.bFindPicFlag)
		{
			m_mapStatusOutputParam = input.outputParam;
		}

		//�������������ظ�����������Ҫ�ظ��Ĵ������ͱ��Ϊ������ϣ�Ŀǰ�ظ���������ͼƬ�Ա�������Ч��
		input.alreadyRepeatCount += 1;
		if (input.alreadyRepeatCount >= input.repeatCount)
		{
			input.bFinishFlag = true;
		}
		else
		{
			//û�дﵽ�ظ�����ʱ�����ÿ�ʼʱ�䣬���ж��Ƿ���ת��break�����ж��ӳ�
			input.startTime = GetTickCount();
			//�����������ͼ�ɹ���ת�����ı�������Ϊ��ͼ���Ƕ�����ʹ�ö�Σ���Ҳ�Ͳ����ٶ�ӽṹ������ˣ�Ŀǰ�Ķ���ظ���������ӳ���
			if (-1 != input.findPicSucceedJumpIndex && 0xffff != input.findPicSucceedJumpIndex)
			{
				m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, m_mapStatusInputVec);
			}
			break;
		}
	}

	if (bAllFinishedFlag)
	{
		m_mainWnd->ResetAllInputFinishFlag(m_mapStatusInputVec);
	}
}

void PlayerUI::UpdateNormalInputDataVector(int cmpParam, std::vector<InputData> &inputVec)
{
	if (inputVec.size() == 0)
	{
		m_mainWnd->AddTipInfo("�ű���������Ϊ0���߳��˳�...");
		return;
	}

	//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
	m_mainWnd->UpdateGameWindowSize();
	//ǿ�Ƹ��´������ݣ����㴰����С��
// 	::SetActiveWindow(m_hWnd);
// 	InvalidateRect(m_hWnd, nullptr, TRUE);
// 	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
// 	PostMessage(m_hWnd, WM_ACTIVATE, 0, 0);
// 	PostMessage(m_hWnd, WM_ERASEBKGND, 0, 0);
// 	PostMessage(m_hWnd, WM_PAINT, 0, 0);

	//��Ҫ�Ѹ���������bottom���ԣ��Ų��ᵯ������
// 	::SetWindowPos( m_hParentWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
	bool bAllFinishedFlag = true;
	int index = -1;
	for (auto &input : inputVec)
	{
		++index;
		//�жϲ����Ѿ����
		if ((input.bFinishFlag && input.type != Pic)
			|| (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag))
			|| (-1 != cmpParam && input.cmpParam != cmpParam))
			continue;

		//��ʼ�����뿪ʼʱ��
		if (!input.bInitStartTimeFlag)
		{
			input.bInitStartTimeFlag = true;
			input.startTime = GetTickCount();
		}

		bAllFinishedFlag = false;

		//�ж��ӳ�
		if (GetTickCount() - input.startTime < (DWORD)input.delay)
			break;

		//��ѯͼƬ�Ƿ�ʱ
		if (input.findPicOvertime != -1 && InputType::Pic == input.type && (GetTickCount() - input.startTime > (DWORD)(input.findPicOvertime + input.delay)))
		{
			input.bFindPicOvertimeFlag = true;
			//�жϳ�ʱָ����ת
			if (-1 != input.findPicOvertimeJumpIndex)
			{
				(0xffff != input.findPicOvertimeJumpIndex) ? m_mainWnd->JumpInput(input.findPicOvertimeJumpIndex, inputVec) :
					(m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicOvertimeJumpModule, inputVec));
				break;
			}
			continue;
		}

		switch (input.type)
		{
		case Mouse:
			m_mainWnd->HandleMouseInput(input);
			break;
		case Keyboard:
			m_mainWnd->HandleKeyboardInput(input);
			break;
		case Pic:
			m_mainWnd->HandleGameImgCompare(input);
			break;
		case StopScript:
		{
			inputVec.clear();
			m_mainWnd->AddTipInfo("�ű���������ϣ����ֶ�ѡ�������ű�ִ��");
		}
		break;
		case Log:
		{
			m_mainWnd->AddTipInfo(input.comment);
		}
		break;
		case Jump:
		{
			if (0xffff == input.findPicSucceedJumpIndex)
			{
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, inputVec);
				break;
			}
			else
			{
				if (-1 != input.findPicSucceedJumpIndex)
				{
					m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, inputVec);
				}
			}
		}
		break;
		default:
			break;
		}

		//���û���ҵ�ͼƬ������������������һ��
		if (InputType::Pic == input.type && !input.bFindPicFlag)
		{
			break;
		}
		//ͼƬ�Աȳɹ�ָ����ת
		else if (InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex)
		{
			(0xffff != input.findPicSucceedJumpIndex) ? m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, inputVec) :
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, inputVec);
			break;
		}

		//�������������ظ�����������Ҫ�ظ��Ĵ������ͱ��Ϊ������ϣ�Ŀǰ�ظ���������ͼƬ�Ա�������Ч��
		input.alreadyRepeatCount += 1;
		if (input.alreadyRepeatCount >= input.repeatCount)
		{
			input.bFinishFlag = true;
		}
		else
		{
			//û�дﵽ�ظ�����ʱ�����ÿ�ʼʱ�䣬���ж��Ƿ���ת��break�����ж��ӳ�
			input.startTime = GetTickCount();
			//�����������ͼ�ɹ���ת�����ı�������Ϊ��ͼ���Ƕ�����ʹ�ö�Σ���Ҳ�Ͳ����ٶ�ӽṹ������ˣ�Ŀǰ�Ķ���ظ���������ӳ���
			if (-1 != input.findPicSucceedJumpIndex && 0xffff != input.findPicSucceedJumpIndex)
			{
				m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, inputVec);
			}
			break;
		}
	}

	if (bAllFinishedFlag)
	{
		m_mainWnd->ResetAllInputFinishFlag(inputVec);
	}
}

void PlayerUI::StopScritp()
{
	m_mapStatusTimer.stop();
	m_nextStepTimer.stop();
	m_mapPosSelectTimer.stop();
	m_mapStatusOutputParam = 0;
	m_lastStatusParam = -1;

	//����ָ����ģ��������
	int index = m_ui->cmb_sim->currentIndex();
	m_mainWnd->SetSimWndType((SimWndType)index);
	m_mainWnd->AddTipInfo("�ű���ֹͣ����");
}

void PlayerUI::StartScript()
{
	m_mapStatusTimer.start(5);
	m_nextStepTimer.start(10);
	m_mapPosSelectTimer.start(3);
} 

void PlayerUI::OnBtnStop()
{
	StopScritp();
}

void PlayerUI::OnBtnA4Reward()
{
	StopScritp();

	int index = m_ui->cmb_delegate->currentIndex();
	switch (index)
	{
	case A4:
	{
		m_mainWnd->LoadScriptModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_a4_reward").c_str());
	}
		break;
	default:
		break;
	}

	StartScript();
}

void PlayerUI::OnBtnDaily()
{
	StopScritp();
	m_mainWnd->LoadScriptModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_daily").c_str());
	StartScript();
}

void PlayerUI::OnBtnNextStep()
{
	StopScritp();
	m_mainWnd->LoadScriptModuleFile(std::string(DEFAULT_PATH).append("g").c_str());
	StartScript();
}

void PlayerUI::OnBtnDelegate()
{
	StopScritp();

	int index = m_ui->cmb_delegate->currentIndex();
	auto it = m_specificDelegateScriptMap.find((ZZ_Delegate)index);
	if (it != m_specificDelegateScriptMap.end())
	{
		m_mainWnd->LoadScriptModuleFile(it->second.toStdString().c_str());
		StartScript();
	}
}

void PlayerUI::OnBtnDev()
{
	StopScritp();

	int index = m_ui->cmb_dev->currentIndex();
	auto it = m_specificDevScriptMap.find((ZZ_Dev)index);
	if (it != m_specificDevScriptMap.end())
	{
		m_mainWnd->LoadScriptModuleFile(it->second.toStdString().c_str());
		StartScript();
	}
}

void PlayerUI::OnBtnRecruit()
{
	StopScritp();
	m_mainWnd->LoadScriptModuleFile(std::string(DEFAULT_PATH).append("recruit").c_str());
	StartScript();
}

void PlayerUI::OnBtnSpecific()
{
	StopScritp();

	int index = m_ui->cmb_specific->currentIndex();
	auto it = m_specificLevelScriptMap.find((ZZ_Specific)index);
	if (it != m_specificLevelScriptMap.end())
	{
		m_mainWnd->LoadScriptModuleFile(it->second.toStdString().c_str());
		StartScript();
	}
}

void PlayerUI::OnBtnStartAuto()
{
	StopScritp();
	StartScript();
}
