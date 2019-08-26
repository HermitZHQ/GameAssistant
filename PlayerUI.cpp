#include "PlayerUI.h"
#include "ui_player.h"
#include "mainwindow.h"
#include "PreDef.h"

PlayerUI::PlayerUI(MainWindow *wnd) :
	m_ui(new Ui::PlayerUI)
	, m_mainWnd(wnd)
	, m_threadMapStatus(this)
	, m_threadMapRecognize(this)
	, m_threadMapPosSelect(this)
	, m_threadNextStep(this)
	, m_bRunThreadFlag(true)
	, m_bPauseMapStatusFlag(true)
	, m_bPauseMapRecognizeFlag(true)
	, m_bPauseMapPosSelectFlag(true)
	, m_bPauseNextStepFlag(true)

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
	, m_bInBattleFlag(false)
	, m_bInitFlag(false)
	, m_bToBattleMainFlag(false)
	, m_bToBattleRewardFlag( false )
	, m_bToBattleDailyFlag(false)
	, m_bToDev20(false)
	, m_bToBattleEmergencyFlag(false)
{
	m_ui->setupUi(this);
}

PlayerUI::~PlayerUI()
{
	delete m_ui;

	m_bRunThreadFlag = false;
	m_bPauseMapStatusFlag = true;
	m_threadMapStatus.quit();
	m_threadMapStatus.wait();

	m_threadMapRecognize.quit();
	m_threadMapRecognize.wait();

	m_threadMapPosSelect.quit();
	m_threadMapPosSelect.wait();

	m_threadNextStep.quit();
	m_threadNextStep.wait();
}

void PlayerUI::Init()
{
	if ( m_bInitFlag )
	{
		return;
	}

	connect( &m_threadMapStatus, &ThreadMapStatus::resultReady, this, &PlayerUI::handleResults );
	connect( &m_threadMapStatus, &ThreadMapStatus::finished, &m_threadMapStatus, &QObject::deleteLater );
	m_threadMapStatus.start();

	connect( &m_threadMapRecognize, &ThreadMapRecognize::resultReady, this, &PlayerUI::handleResults );
	connect( &m_threadMapRecognize, &ThreadMapRecognize::finished, &m_threadMapRecognize, &QObject::deleteLater );
	m_threadMapRecognize.start();

	connect( &m_threadMapPosSelect, &ThreadMapPosSelect::resultReady, this, &PlayerUI::handleResults );
	connect( &m_threadMapPosSelect, &ThreadMapPosSelect::finished, &m_threadMapPosSelect, &QObject::deleteLater );
	m_threadMapPosSelect.start();

	connect( &m_threadNextStep, &ThreadNextStep::resultReady, this, &PlayerUI::handleResults );
	connect( &m_threadNextStep, &ThreadNextStep::finished, &m_threadNextStep, &QObject::deleteLater );
	m_threadNextStep.start();

	m_bInitFlag = true;
}

void PlayerUI::UpdateAllScript()
{
// 	DWORD time = GetTickCount();
	UpdateMapStatusRecognizeScript();
	UpdateMapPositionSelectScript();
	UpdateAllMapRecognizeAndBattleScript();
	UpdateNextStepScript();
// 	m_mainWnd->AddTipInfo(std::string("�������º�ʱ��").append(std::to_string(GetTickCount() - time)).c_str());
}

void PlayerUI::UpdateMapStatusRecognizeScript()
{
	UpdateMapStatusInputDataVector(m_mapStatusCmpParam);

	//����״̬��ʼ�͹رն�Ӧ��timerִ�нű������Ч�ʣ����ٴ�����
	if ( !NotInBattleFlag() )
	{
		if (!m_bInBattleFlag)
		{
			m_bInBattleFlag = true;

			//��Ϊս���ű���ɺ����ڶ���ֹͣ��������ת�ص�ͼ���ң���������Ƚϼ򵥣������漰����ת�ص�ͼ���Һ��ֽ���ս���ű������⣬������Ϸ��ߵĽű����¶������Ķ���ֹͣ�ű�����ֹͣtimer�����ǰ�inputVec��գ����Ե�������Ҫ�ٴο�����ͼʶ���ʱ��������Ҫ�ȼ���ʶ���ļ�����Ϊ��ʱ��vec�ǿգ�
			m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
			m_mainWnd->AddTipInfo( "�ѽ���ս������--------" );

			//ֻҪ������ս����������������ս����ĵ�ͼѡ����
			ResetPosSelectFlags();

			//ս�������У���ͣ��ת��ͼ������ʶ���ͼ������cpu����
			m_bPauseMapPosSelectFlag = true;
			m_bPauseMapRecognizeFlag = false;
		}

	}
	else if (NotInBattleFlag())
	{
		if (m_bInBattleFlag)
		{
			m_bInBattleFlag = false;

			m_mainWnd->AddTipInfo("�Ѵ�ս�������˳�--------");

			//ս�������У�������ת��ͼ����ͣʶ���ͼ������cpu����
			m_bPauseMapPosSelectFlag = false;
			m_bPauseMapRecognizeFlag = true;
		}
	}
}

void PlayerUI::UpdateNextStepScript()
{
	if (m_nextStepInputVec.size())
	{
		UpdateNormalInputDataVector(-1, m_nextStepInputVec);
	}
}

void PlayerUI::UpdateMapPositionSelectScript()
{
	//������Ϸ״̬����̬�����л������ű�

	//�龰�����仯ʱ�����滻һ�νű�
	if (m_lastStatusParam != m_mapStatusOutputParam)
	{
		m_lastStatusParam = m_mapStatusOutputParam;

		//if���ϵ������ȼ����εݼ����ͽ���ͣ�Ҳ��������������ִ����Ϻ���ִ���ͽ�--------------------------

		if ( m_ui->chk_dev->isChecked() 
			&& m_devSetting.bShouldExecFlag
			&& !m_bInBattleFlag )//����----
		{
			if ( ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam )
			{
				GotoDev20();
			}
			else if ( NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main
				&& !m_bToDev20)
			{
				GotoBattleMain();
			}
			else if ( ZZ_Map_Param::Dev_ing == m_mapStatusOutputParam )
			{
				m_devSetting.Reset();
				ResetPosSelectFlags();
			}
		}
		else if ( m_ui->chk_emergency->isChecked()//��������----
			&& m_emergencySetting.bShouldExecFlag
			&& !m_bInBattleFlag )
		{
			if ( ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam )
			{
				GotoEmergency();
				m_emergencySetting.Reset();
			}
			else if ( NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main
				&& !m_bToBattleEmergencyFlag )
			{
				GotoBattleMain();
			}
		}
		else if ( m_ui->chk_daily->isChecked() )//ÿ������----
		{
			if ( ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam )
			{
				GotoDailyFirstIcon();
			}
			else if ( NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main
				&& !m_bToBattleDailyFlag)
			{
				GotoBattleMain();
			}
		}
		//�ͽ����ȼ��ͣ��Ƚ��п�����ļ������
		else if (m_ui->chk_reward->isChecked())//�ͽ�����----
		{
			if (ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam
				&& !m_bToBattleRewardFlag)
			{
				GotoRewardFirstIcon();
			}
			else if (NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main )
			{
				GotoBattleMain();
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

void PlayerUI::handleResults(const QString &)
{

}

void PlayerUI::UpdateMapStatusInputDataVector(int cmpParam)
{
	if (m_mapStatusInputVec.size() == 0)
	{
		m_mainWnd->AddTipInfo("�ű���������Ϊ0���߳��˳�...");
		return;
	}

	//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
	m_mainWnd->UpdateGameWindowSize();

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
		DWORD dwTime = GetTickCount() - input.startTime;
		if (GetTickCount() - input.startTime < (DWORD)input.delay)
			break;

		//�ȴ����͵Ļ���������ж��Ƿ��ѵ�ʱ��
		if (input.type == Wait && GetTickCount() - input.startTime < (DWORD)(input.waitTime * 1000))
			break;

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

		//��ѯͼƬ�Ƿ�ʱ---����ִ��֮��������֤������ִ��һ�ζԱȲŻᳬʱ
		dwTime = GetTickCount() - input.startTime;
		if ( input.findPicOvertime != -1
			&& InputType::Pic == input.type
			&& ( GetTickCount() - input.startTime > ( DWORD )( input.findPicOvertime + input.delay ) )
			&& !input.bFindPicFlag )
		{
// 			m_mainWnd->AddTipInfo(std::string("cost time:").append(std::to_string(dwTime)).c_str());
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

		//�ȴ����͵Ļ���������ж��Ƿ��ѵ�ʱ��
		if (input.type == Wait && GetTickCount() - input.startTime < (DWORD)(input.waitTime * 1000))
			break;

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
			return;
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

		//��ѯͼƬ�Ƿ�ʱ---����ִ��֮��������֤������ִ��һ�ζԱȲŻᳬʱ
		if ( input.findPicOvertime != -1
			&& InputType::Pic == input.type
			&& ( GetTickCount() - input.startTime > ( DWORD )( input.findPicOvertime + input.delay ) )
			&& !input.bFindPicFlag )
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
	m_bPauseMapStatusFlag = true;
	m_bPauseMapRecognizeFlag = true;
	m_bPauseMapPosSelectFlag = true;
	m_bPauseNextStepFlag = true;

	m_mapStatusOutputParam = 0;
	m_lastStatusParam = -1;

	m_emergencySetting.Stop();
	m_devSetting.Stop();
	m_delegateSetting.Stop();
	m_recruitSetting.Stop();

	//����ָ����ģ��������
	int index = m_ui->cmb_sim->currentIndex();
	m_mainWnd->SetSimWndType((SimWndType)index);
	m_mainWnd->AddTipInfo("�ű���ֹͣ����");
}

void PlayerUI::StartScript()
{
	Init();

	//startʱ���»�ȡһ�ιһ����õ��ӳ�
	m_emergencySetting.interval = m_ui->edt_emergencyInterval->text().toInt();
	m_devSetting.interval = m_ui->edt_devInterval->text().toInt();
	m_delegateSetting.interval = m_ui->edt_delegateInterval->text().toInt();
	m_recruitSetting.interval = m_ui->edt_recruitInterval->text().toInt();

	m_emergencySetting.StartAutoHandle();
	m_devSetting.StartAutoHandle();
	m_delegateSetting.StartAutoHandle();
	m_recruitSetting.StartAutoHandle();

	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("map_status_recognize").c_str(), m_mapStatusInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("next_step").c_str(), m_nextStepInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);

	m_bPauseMapStatusFlag = false;
	m_bPauseMapRecognizeFlag = false;
	m_bPauseMapPosSelectFlag = false;
	m_bPauseNextStepFlag = false;

	ResetPosSelectFlags();
} 

void PlayerUI::ResetPosSelectFlags()
{
	m_bToBattleMainFlag = false;
	m_bToBattleRewardFlag = false;
	m_bToBattleDailyFlag = false;
	m_bToBattleEmergencyFlag = false;
	m_bToDev20 = false;
}

void PlayerUI::GotoBattleMain()
{
	if ( m_bToBattleMainFlag
		|| m_bToBattleRewardFlag
		|| m_bToBattleDailyFlag )
	{
		return;
	}

	m_bToBattleMainFlag = true;
	m_bToBattleRewardFlag = false;
	m_bToBattleDailyFlag = false;
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_battle_main" ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::GotoRewardFirstIcon()
{
	if ( m_bToBattleRewardFlag )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToBattleRewardFlag = true;

	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_battle_main_reward_firstIcon" ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::GotoDailyFirstIcon()
{
	if ( m_bToBattleDailyFlag )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToBattleDailyFlag = true;

	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_battle_main_daily_firstIcon" ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::GotoEmergency()
{
	if ( m_bToBattleEmergencyFlag )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToBattleEmergencyFlag = true;

	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_battle_main_emergency" ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::GotoDev20()
{
	if ( m_bToDev20 )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToDev20 = true;

	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_dev20" ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::OnBtnStop()
{
	StopScritp();
}

void PlayerUI::OnBtnStartAuto()
{
	StopScritp();
	StartScript();
}


void PlayerUI::OnBtnToggleBattleOptions()
{
	m_ui->chk_emergency->toggle();
	m_ui->chk_daily->toggle();
	m_ui->chk_reward->toggle();
}

void PlayerUI::OnBtnToggleNoneBattleOptions()
{
	m_ui->chk_delegate->toggle();
	m_ui->chk_dev->toggle();
	m_ui->chk_recruit->toggle();
}

void PlayerUI::OnBtnHuodong1()
{
	Init();
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("huodong_t_1_5").c_str(), m_nextStepInputVec);

	m_bPauseNextStepFlag = false;

	ResetPosSelectFlags();
}

void ThreadMapStatus::run()
{
	QString result;
	/* ... here is the expensive or blocking operation ... */
	while (m_parent->GetRunThreadFlag())
	{
		while (!m_parent->GetPauseMapStatusFlag())
		{
			m_parent->UpdateMapStatusRecognizeScript();
// 			m_parent->UpdateAllScript();
			msleep( 100 );
		}

		msleep( 1000 );
	}

	emit resultReady(result);
}

void ThreadMapRecognize::run()
{
	QString result;
	/* ... here is the expensive or blocking operation ... */
	while ( m_parent->GetRunThreadFlag() )
	{
		while (!m_parent->GetPauseMapRecognizeFlag())
		{
			m_parent->UpdateAllMapRecognizeAndBattleScript();
			msleep( 1 );
		}

		msleep( 1000 );
	}

	emit resultReady( result );
}

void ThreadNextStep::run()
{
	QString result;
	/* ... here is the expensive or blocking operation ... */
	while ( m_parent->GetRunThreadFlag() )
	{
		while (!m_parent->GetPauseNextStepFlag())
		{
			m_parent->UpdateNextStepScript();
			msleep( 100 );
		}

		msleep( 1000 );
	}

	emit resultReady( result );
}

void ThreadMapPosSelect::run()
{
	QString result;
	/* ... here is the expensive or blocking operation ... */
	while ( m_parent->GetRunThreadFlag() )
	{
		while (!m_parent->GetPauseMapPosSelectFlag())
		{
			m_parent->UpdateMapPositionSelectScript();

			msleep( 50 );
		}

		msleep( 1000 );
	}

	emit resultReady( result );
}
