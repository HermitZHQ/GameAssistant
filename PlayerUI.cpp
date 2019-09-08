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
		{WOLT_60, QString( "to_battle_main_wolt60" )},
		{ MAOMAO_60, QString( "to_battle_main_maomao60" ) },
		{ Evelyn_60_1, QString( "to_battle_main_evelyn60_1" ) },
		{ Evelyn_60_2, QString( "to_battle_main_evelyn60_2" ) },
		{ Evelyn_60_3, QString( "to_battle_main_evelyn60_3" ) },
		})

	, m_specificDelegateScriptMap({
		{Slowest, QString("delegate_slowest")},
		})

	, m_specificDevScriptMap({
		{Limit, QString("to_dev")},
		{Normal10, QString("to_dev10")},
		{Normal20, QString("to_dev20")},
		})

	, m_specificHuodongScriptMap( {
		{ t_1_5, QString( "huodong_t_1_5" ) },
		{ t_2_5, QString( "huodong_t_2_5" ) },
		{ t_3_5, QString( "huodong_t_3_5" ) },
		{ t_4_5, QString( "huodong_t_4_5" ) },
	})

	, m_mapStatusCmpParam(-1)
	, m_mapStatusOutputParam(0)
	, m_lastStatusParam(-1)
	, m_bInBattleFlag(false)
	, m_bInitFlag(false)
	, m_bToBattleMainFlag(false)
	, m_bToBattleRewardFlag( false )
	, m_bToBattleDailyFlag(false)
	, m_bToDev(false)
	, m_bToBattleEmergencyFlag(false)
	, m_bToBattleFbFlag(false)

	, m_bFbFinishedFlag(false), m_bDailyFinishedFlag(false)
{
	m_ui->setupUi(this);

	m_mainThreadResetTimer.connect( &m_mainThreadResetTimer, &QTimer::timeout, this, &PlayerUI::UpdateMainThreadTimerReset );
	m_mainThreadResetTimer.start( 50 );

	//��ս������µ�����
	m_checkNoneBattleBlockTimer.interval = 180000;
	m_checkNoneBattleBlockTimer.timer.connect( &m_checkNoneBattleBlockTimer.timer, &QTimer::timeout, [&]() {

		if ( NotInBattleFlag() )
		{
			ForceGotoBattleMain();
			m_mainWnd->AddTipInfo( "---->�ƺ��������ˣ����Իص�ս����������...<----" );
		}
	} );

	//ս���е��������
	m_checkBattleBlockTimer.interval = 1200000;
	m_checkBattleBlockTimer.timer.connect( &m_checkBattleBlockTimer.timer, &QTimer::timeout, [&]() {

		if ( !NotInBattleFlag() )
		{
			ForceQuitBattle();
			m_mainWnd->AddTipInfo( "---->�ƺ���ս���ˣ����Իص�ս����������...<----" );
		}
	} );

	//��ʾ��Ϣ���Ҽ��˵�
	auto actClearInfo = m_menu.addAction( Q8( "�����Ϣ" ) );
	m_ui->list_tip->setContextMenuPolicy( Qt::CustomContextMenu );
	//table view connect
	m_ui->list_tip->connect( m_ui->list_tip, &QListView::customContextMenuRequested, [&]( const QPoint &pt ) {
		auto pos = m_ui->list_tip->mapToGlobal( pt );
		m_menu.move( pos );
		m_menu.show();
	} );
	//menu connect
	m_menu.connect( actClearInfo, &QAction::triggered, this, &PlayerUI::ClearTipInfo );
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

	//0��ʱ��û��ʶ����κ�״̬��ֱ���˳�
	if ( 0 == m_mapStatusOutputParam )
	{
		return;
	}

	//����״̬��ʼ�͹رն�Ӧ��timerִ�нű������Ч�ʣ����ٴ�����
	if ( !NotInBattleFlag() )
	{
		if (!m_bInBattleFlag)
		{
			m_bInBattleFlag = true;

			//��Ϊս���ű���ɺ����ڶ���ֹͣ��������ת�ص�ͼ���ң���������Ƚϼ򵥣������漰����ת�ص�ͼ���Һ��ֽ���ս���ű������⣬������Ϸ��ߵĽű����¶������Ķ���ֹͣ�ű�����ֹͣtimer�����ǰ�inputVec��գ����Ե�������Ҫ�ٴο�����ͼʶ���ʱ��������Ҫ�ȼ���ʶ���ļ�����Ϊ��ʱ��vec�ǿգ�
			std::lock_guard<std::mutex> lock( m_loadScriptMutex );
			m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
			m_mainWnd->AddTipInfo( "�ѽ���ս������--------" );

			//ֻҪ������ս����������������ս����ĵ�ͼѡ����
			ResetPosSelectFlags();

			//ս�������У���ͣ��ת��ͼ������ʶ���ͼ������cpu����
			m_bPauseMapPosSelectFlag = true;
			m_bPauseMapRecognizeFlag = false;

			//����ʱ����
			if ( m_emergencySetting.bShouldExecFlag )
			{
				m_emergencySetting.SetResetFlag();
			}

			//����ս���������
			m_checkBattleBlockTimer.SetResetFlag();
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
		m_checkNoneBattleBlockTimer.SetResetFlag();

		//if���ϵ������ȼ����εݼ����ͽ���ͣ�Ҳ��������������ִ����Ϻ���ִ���ͽ�--------------------------

		if ( m_ui->chk_dev->isChecked() 
			&& m_devSetting.bShouldExecFlag
			&& !m_bInBattleFlag )//����----
		{
			if ( ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam )
			{
				GotoDev();
			}
			else if ( NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main
				&& !m_bToDev)
			{
				GotoBattleMain();
			}
			else if ( ZZ_Map_Param::Dev_ing == m_mapStatusOutputParam )
			{
				m_devSetting.SetResetFlag();
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
			}
			else if ( NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main
				&& !m_bToBattleEmergencyFlag )
			{
				GotoBattleMain();
			}
		}
		else if ( m_ui->chk_daily->isChecked() 
			&& !m_bDailyFinishedFlag)//ÿ������----
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
		else if ( m_ui->chk_fb->isChecked()//��������----
			&& !m_bFbFinishedFlag
			&& !m_bInBattleFlag )
		{
			if ( ZZ_Map_Param::Battle_Main == m_mapStatusOutputParam )
			{
				GotoFb();
			}
			else if ( NotInBattleFlag()
				&& m_mapStatusOutputParam != ZZ_Map_Param::Battle_Main
				&& !m_bToBattleFbFlag )
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
		UpdateMapRecognizeAndBattleInputDataVector(-1);
	}
}

void PlayerUI::handleResults(const QString &)
{

}

void PlayerUI::ClearTipInfo()
{
	m_ui->list_tip->clear();
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

			//ԭ�������ﲻ�ᴥ��break����Ϊ��ͼ״̬ʶ��ű��в�������תģ�������
// 			(0xffff != input.findPicSucceedJumpIndex) ? m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, m_mapStatusInputVec) :
// 				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, m_mapStatusInputVec);
// 			break;
		}
		else if (InputType::Pic == input.type && input.bFindPicFlag)
		{
			m_mapStatusOutputParam = input.outputParam;
		}

		//�������Ϊ�����������㣬���������ø�����ɱ��Ϊ�棬���ҰѶ�λ�ű�ת��battle_main��������ִ���µ�ָ��
		if ( ZZ_Map_Param::Fb_Count_0 == m_mapStatusOutputParam )
		{
			m_bFbFinishedFlag = true;
			m_ui->chk_fb->setChecked( false );
			GotoBattleMain();
		}
		else if ( ZZ_Map_Param::Daily_Count_0 == m_mapStatusOutputParam )
		{
			m_bDailyFinishedFlag = true;
			m_ui->chk_daily->setChecked( false );
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

void PlayerUI::UpdateMapRecognizeAndBattleInputDataVector( int cmpParam )
{
	std::lock_guard<std::mutex> lock( m_loadScriptMutex );
	if ( m_mapRecognizeInputVec.size() == 0 )
	{
		m_mainWnd->AddTipInfo( "�ű���������Ϊ0���߳��˳�..." );
		return;
	}

	//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
	m_mainWnd->UpdateGameWindowSize();

	bool bAllFinishedFlag = true;
	int index = -1;
	for ( auto &input : m_mapRecognizeInputVec )
	{
		++index;
		//�жϲ����Ѿ����
		if ( ( input.bFinishFlag && input.type != Pic )
			|| ( input.type == Pic && ( input.bFindPicFlag || input.bFindPicOvertimeFlag ) )
			|| ( -1 != cmpParam && input.cmpParam != cmpParam ) )
			continue;

		//��ʼ�����뿪ʼʱ��
		if ( !input.bInitStartTimeFlag )
		{
			input.bInitStartTimeFlag = true;
			input.startTime = GetTickCount();
		}

		bAllFinishedFlag = false;

		//�ж��ӳ�
		if ( GetTickCount() - input.startTime < ( DWORD )input.delay )
			break;

		//�ȴ����͵Ļ���������ж��Ƿ��ѵ�ʱ��
		if ( input.type == Wait && GetTickCount() - input.startTime < ( DWORD )( input.waitTime * 1000 ) )
			break;

		switch ( input.type )
		{
		case Mouse:
			m_mainWnd->HandleMouseInput( input );
			break;
		case Keyboard:
			m_mainWnd->HandleKeyboardInput( input );
			break;
		case Pic:
			m_mainWnd->HandleGameImgCompare( input );
			break;
		case StopScript:
		{
			m_mapRecognizeInputVec.clear();
			m_mainWnd->AddTipInfo( "�ű���������ϣ����ֶ�ѡ�������ű�ִ��" );
			return;
		}
		break;
		case Log:
		{
			m_mainWnd->AddTipInfo( input.comment );
		}
		break;
		case Jump:
		{
			if ( 0xffff == input.findPicSucceedJumpIndex )
			{
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec( input.findPicSucceedJumpModule, m_mapRecognizeInputVec );
				break;
			}
			else
			{
				if ( -1 != input.findPicSucceedJumpIndex )
				{
					m_mainWnd->JumpInput( input.findPicSucceedJumpIndex, m_mapRecognizeInputVec );
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
			if ( -1 != input.findPicOvertimeJumpIndex )
			{
				( 0xffff != input.findPicOvertimeJumpIndex ) ? m_mainWnd->JumpInput( input.findPicOvertimeJumpIndex, m_mapRecognizeInputVec ) :
					( m_mainWnd->LoadScriptModuleFileToSpecificInputVec( input.findPicOvertimeJumpModule, m_mapRecognizeInputVec ) );
				break;
			}
			continue;
		}

		//���û���ҵ�ͼƬ������������������һ��
		if ( InputType::Pic == input.type && !input.bFindPicFlag )
		{
			break;
		}
		//ͼƬ�Աȳɹ�ָ����ת
		else if ( InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex )
		{
			( 0xffff != input.findPicSucceedJumpIndex ) ? m_mainWnd->JumpInput( input.findPicSucceedJumpIndex, m_mapRecognizeInputVec ) :
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec( input.findPicSucceedJumpModule, m_mapRecognizeInputVec );
			break;
		}

		//�������������ظ�����������Ҫ�ظ��Ĵ������ͱ��Ϊ������ϣ�Ŀǰ�ظ���������ͼƬ�Ա�������Ч��
		input.alreadyRepeatCount += 1;
		if ( input.alreadyRepeatCount >= input.repeatCount )
		{
			input.bFinishFlag = true;
		}
		else
		{
			//û�дﵽ�ظ�����ʱ�����ÿ�ʼʱ�䣬���ж��Ƿ���ת��break�����ж��ӳ�
			input.startTime = GetTickCount();
			//�����������ͼ�ɹ���ת�����ı�������Ϊ��ͼ���Ƕ�����ʹ�ö�Σ���Ҳ�Ͳ����ٶ�ӽṹ������ˣ�Ŀǰ�Ķ���ظ���������ӳ���
			if ( -1 != input.findPicSucceedJumpIndex && 0xffff != input.findPicSucceedJumpIndex )
			{
				m_mainWnd->JumpInput( input.findPicSucceedJumpIndex, m_mapRecognizeInputVec );
			}
			break;
		}
	}

	if ( bAllFinishedFlag )
	{
		m_mainWnd->ResetAllInputFinishFlag( m_mapRecognizeInputVec );
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

	m_checkNoneBattleBlockTimer.timer.stop();

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

	//��ʱ����
	if (m_ui->chk_emergency->isChecked())
	{
		m_emergencySetting.StartAutoHandle();
	}
	if ( m_ui->chk_dev->isChecked() )
	{
		m_devSetting.StartAutoHandle();
	}
	if ( m_ui->chk_delegate->isChecked() )
	{
		m_delegateSetting.StartAutoHandle();
	}
	if ( m_ui->chk_recruit->isChecked() )
	{
		m_recruitSetting.StartAutoHandle();
	}

	m_checkNoneBattleBlockTimer.timer.start();

	//���ø��������þ���ǣ���Ϊ���ܴ�һ�㸱��ѡ���˾��鸱����
	m_bFbFinishedFlag = false;
	m_bDailyFinishedFlag = false;

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
	m_bToBattleFbFlag = false;

	m_bToDev = false;
}

void PlayerUI::GotoBattleMain()
{
	if ( m_bToBattleMainFlag 
		|| m_bToBattleDailyFlag
		|| m_bToBattleRewardFlag
		|| m_bToBattleFbFlag
		|| m_bToBattleEmergencyFlag)
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToBattleMainFlag = true;
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_battle_main" ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::ForceGotoBattleMain()
{
	if ( m_bToDev )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToBattleMainFlag = true;
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( "to_battle_main" ).c_str(), m_mapPosSelectInputVec );
	m_bPauseMapPosSelectFlag = false;
}

void PlayerUI::ForceQuitBattle()
{

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

void PlayerUI::GotoFb()
{
	if ( m_bToBattleFbFlag )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToBattleFbFlag = true;

	int index = m_ui->cmb_fb->currentIndex();
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( m_specificLevelScriptMap[ZZ_Specific(index)].toStdString() ).c_str(), m_mapPosSelectInputVec );
}

void PlayerUI::GotoDev()
{
	if ( m_bToDev )
	{
		return;
	}

	ResetPosSelectFlags();
	m_bToDev = true;

	int index = m_ui->cmb_dev->currentIndex();
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec( std::string( DEFAULT_PATH ).append( m_specificDevScriptMap[ZZ_Dev( index )].toStdString() ).c_str(), m_mapPosSelectInputVec );
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

void PlayerUI::UpdateMainThreadTimerReset()
{
	m_emergencySetting.CheckReset();
	m_devSetting.CheckReset();
	m_delegateSetting.CheckReset();
	m_recruitSetting.CheckReset();
	m_checkNoneBattleBlockTimer.CheckReset();
}

void PlayerUI::OnBtnHuodong1()
{
	Init();

	int index = m_ui->cmb_huodong->currentIndex();
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append( m_specificHuodongScriptMap[ZZ_Huodong( index )].toStdString() ).c_str(), m_nextStepInputVec);

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
