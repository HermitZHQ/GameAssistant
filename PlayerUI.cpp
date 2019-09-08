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

	//非战斗情况下的阻塞
	m_checkNoneBattleBlockTimer.interval = 180000;
	m_checkNoneBattleBlockTimer.timer.connect( &m_checkNoneBattleBlockTimer.timer, &QTimer::timeout, [&]() {

		if ( NotInBattleFlag() )
		{
			ForceGotoBattleMain();
			m_mainWnd->AddTipInfo( "---->似乎卡界面了，尝试回到战斗主界面中...<----" );
		}
	} );

	//战斗中的阻塞检查
	m_checkBattleBlockTimer.interval = 1200000;
	m_checkBattleBlockTimer.timer.connect( &m_checkBattleBlockTimer.timer, &QTimer::timeout, [&]() {

		if ( !NotInBattleFlag() )
		{
			ForceQuitBattle();
			m_mainWnd->AddTipInfo( "---->似乎卡战斗了，尝试回到战斗主界面中...<----" );
		}
	} );

	//提示信息的右键菜单
	auto actClearInfo = m_menu.addAction( Q8( "清除信息" ) );
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
// 	m_mainWnd->AddTipInfo(std::string("整个更新耗时：").append(std::to_string(GetTickCount() - time)).c_str());
}

void PlayerUI::UpdateMapStatusRecognizeScript()
{
	UpdateMapStatusInputDataVector(m_mapStatusCmpParam);

	//0的时候还没有识别出任何状态，直接退出
	if ( 0 == m_mapStatusOutputParam )
	{
		return;
	}

	//根据状态开始和关闭对应的timer执行脚本，提高效率，减少错误点击
	if ( !NotInBattleFlag() )
	{
		if (!m_bInBattleFlag)
		{
			m_bInBattleFlag = true;

			//因为战斗脚本完成后，现在都会停止，不会跳转回地图查找，这样处理比较简单，否则还涉及到跳转回地图查找后又进入战斗脚本的问题，具体游戏这边的脚本更新都有所改动，停止脚本不再停止timer，而是把inputVec清空，所以当我们需要再次开启地图识别的时候，我们需要先加载识别文件（因为这时候vec是空）
			std::lock_guard<std::mutex> lock( m_loadScriptMutex );
			m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
			m_mainWnd->AddTipInfo( "已进入战斗场景--------" );

			//只要进入了战斗场景则重置所有战斗外的地图选择标记
			ResetPosSelectFlags();

			//战斗场景中，暂停跳转地图，开启识别地图，降低cpu消耗
			m_bPauseMapPosSelectFlag = true;
			m_bPauseMapRecognizeFlag = false;

			//重置时间检查
			if ( m_emergencySetting.bShouldExecFlag )
			{
				m_emergencySetting.SetResetFlag();
			}

			//开启战斗防卡检测
			m_checkBattleBlockTimer.SetResetFlag();
		}

	}
	else if (NotInBattleFlag())
	{
		if (m_bInBattleFlag)
		{
			m_bInBattleFlag = false;

			m_mainWnd->AddTipInfo("已从战斗场景退出--------");

			//战斗场景中，开启跳转地图，暂停识别地图，降低cpu消耗
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
	//根据游戏状态，动态加载切换各个脚本

	//情景发生变化时，才替换一次脚本
	if (m_lastStatusParam != m_mapStatusOutputParam)
	{
		m_lastStatusParam = m_mapStatusOutputParam;
		m_checkNoneBattleBlockTimer.SetResetFlag();

		//if从上到下优先级依次递减，赏金最低，也就是其他操作都执行完毕后再执行赏金--------------------------

		if ( m_ui->chk_dev->isChecked() 
			&& m_devSetting.bShouldExecFlag
			&& !m_bInBattleFlag )//开发----
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
		else if ( m_ui->chk_emergency->isChecked()//紧急任务----
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
			&& !m_bDailyFinishedFlag)//每日任务----
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
		else if ( m_ui->chk_fb->isChecked()//副本任务----
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
		//赏金（优先级低，先进行开发招募等需求）
		else if (m_ui->chk_reward->isChecked())//赏金任务----
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
					m_mainWnd->AddTipInfo(std::string("当前界面无法跳转执行赏金任务_").append(std::to_string(m_lastStatusParam)).c_str());
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
		//这里的参数根据用户的勾选变换，目前暂时为-1
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
		m_mainWnd->AddTipInfo("脚本中命令数为0，线程退出...");
		return;
	}

	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	m_mainWnd->UpdateGameWindowSize();

	bool bAllFinishedFlag = true;
	int index = -1;
	for (auto &input : m_mapStatusInputVec)
	{
		++index;
		//判断操作已经完成
		if ((input.bFinishFlag && input.type != Pic)
			|| (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag))
			|| (-1 != cmpParam && input.cmpParam != cmpParam))
		{
			continue;
		}

		//初始化输入开始时间
		if (!input.bInitStartTimeFlag)
		{
			input.bInitStartTimeFlag = true;
			input.startTime = GetTickCount();
		}

		bAllFinishedFlag = false;

		//判断延迟
		DWORD dwTime = GetTickCount() - input.startTime;
		if (GetTickCount() - input.startTime < (DWORD)input.delay)
			break;

		//等待类型的话，则继续判断是否已到时间
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
			m_mainWnd->AddTipInfo("脚本已运行完毕，请手动选择其他脚本执行");
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

		//查询图片是否超时---放在执行之后，这样保证至少先执行一次对比才会超时
		dwTime = GetTickCount() - input.startTime;
		if ( input.findPicOvertime != -1
			&& InputType::Pic == input.type
			&& ( GetTickCount() - input.startTime > ( DWORD )( input.findPicOvertime + input.delay ) )
			&& !input.bFindPicFlag )
		{
// 			m_mainWnd->AddTipInfo(std::string("cost time:").append(std::to_string(dwTime)).c_str());
			input.bFindPicOvertimeFlag = true;
			//判断超时指令跳转
			if (-1 != input.findPicOvertimeJumpIndex)
			{
				(0xffff != input.findPicOvertimeJumpIndex) ? m_mainWnd->JumpInput(input.findPicOvertimeJumpIndex, m_mapStatusInputVec) :
					(m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicOvertimeJumpModule, m_mapStatusInputVec));
				break;
			}
			continue;
		}

		//如果没有找到图片就跳过，继续处理这一项
		if (InputType::Pic == input.type && !input.bFindPicFlag)
		{
			break;
		}
		//图片对比成功指令跳转
		else if (InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex)
		{
			m_mapStatusOutputParam = input.outputParam;

			//原理上这里不会触发break，因为地图状态识别脚本中不存在跳转模块的设置
// 			(0xffff != input.findPicSucceedJumpIndex) ? m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, m_mapStatusInputVec) :
// 				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, m_mapStatusInputVec);
// 			break;
		}
		else if (InputType::Pic == input.type && input.bFindPicFlag)
		{
			m_mapStatusOutputParam = input.outputParam;
		}

		//如果参数为副本次数不足，则马上设置副本完成标记为真，并且把定位脚本转到battle_main，已重新执行新的指令
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

		//处理完后，如果已重复次数等于需要重复的次数，就标记为处理完毕（目前重复次数不对图片对比流程生效）
		input.alreadyRepeatCount += 1;
		if (input.alreadyRepeatCount >= input.repeatCount)
		{
			input.bFinishFlag = true;
		}
		else
		{
			//没有达到重复次数时，重置开始时间，并判断是否跳转后，break继续判断延迟
			input.startTime = GetTickCount();
			//这里借用了找图成功跳转索引的变量，因为找图我们都不会使用多次，我也就不想再多加结构体变量了，目前的多次重复命令都用于延迟上
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
		m_mainWnd->AddTipInfo( "脚本中命令数为0，线程退出..." );
		return;
	}

	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	m_mainWnd->UpdateGameWindowSize();

	bool bAllFinishedFlag = true;
	int index = -1;
	for ( auto &input : m_mapRecognizeInputVec )
	{
		++index;
		//判断操作已经完成
		if ( ( input.bFinishFlag && input.type != Pic )
			|| ( input.type == Pic && ( input.bFindPicFlag || input.bFindPicOvertimeFlag ) )
			|| ( -1 != cmpParam && input.cmpParam != cmpParam ) )
			continue;

		//初始化输入开始时间
		if ( !input.bInitStartTimeFlag )
		{
			input.bInitStartTimeFlag = true;
			input.startTime = GetTickCount();
		}

		bAllFinishedFlag = false;

		//判断延迟
		if ( GetTickCount() - input.startTime < ( DWORD )input.delay )
			break;

		//等待类型的话，则继续判断是否已到时间
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
			m_mainWnd->AddTipInfo( "脚本已运行完毕，请手动选择其他脚本执行" );
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

		//查询图片是否超时---放在执行之后，这样保证至少先执行一次对比才会超时
		if ( input.findPicOvertime != -1
			&& InputType::Pic == input.type
			&& ( GetTickCount() - input.startTime > ( DWORD )( input.findPicOvertime + input.delay ) )
			&& !input.bFindPicFlag )
		{
			input.bFindPicOvertimeFlag = true;
			//判断超时指令跳转
			if ( -1 != input.findPicOvertimeJumpIndex )
			{
				( 0xffff != input.findPicOvertimeJumpIndex ) ? m_mainWnd->JumpInput( input.findPicOvertimeJumpIndex, m_mapRecognizeInputVec ) :
					( m_mainWnd->LoadScriptModuleFileToSpecificInputVec( input.findPicOvertimeJumpModule, m_mapRecognizeInputVec ) );
				break;
			}
			continue;
		}

		//如果没有找到图片就跳过，继续处理这一项
		if ( InputType::Pic == input.type && !input.bFindPicFlag )
		{
			break;
		}
		//图片对比成功指令跳转
		else if ( InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex )
		{
			( 0xffff != input.findPicSucceedJumpIndex ) ? m_mainWnd->JumpInput( input.findPicSucceedJumpIndex, m_mapRecognizeInputVec ) :
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec( input.findPicSucceedJumpModule, m_mapRecognizeInputVec );
			break;
		}

		//处理完后，如果已重复次数等于需要重复的次数，就标记为处理完毕（目前重复次数不对图片对比流程生效）
		input.alreadyRepeatCount += 1;
		if ( input.alreadyRepeatCount >= input.repeatCount )
		{
			input.bFinishFlag = true;
		}
		else
		{
			//没有达到重复次数时，重置开始时间，并判断是否跳转后，break继续判断延迟
			input.startTime = GetTickCount();
			//这里借用了找图成功跳转索引的变量，因为找图我们都不会使用多次，我也就不想再多加结构体变量了，目前的多次重复命令都用于延迟上
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
		m_mainWnd->AddTipInfo("脚本中命令数为0，线程退出...");
		return;
	}

	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	m_mainWnd->UpdateGameWindowSize();

	bool bAllFinishedFlag = true;
	int index = -1;
	for (auto &input : inputVec)
	{
		++index;
		//判断操作已经完成
		if ((input.bFinishFlag && input.type != Pic)
			|| (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag))
			|| (-1 != cmpParam && input.cmpParam != cmpParam))
			continue;

		//初始化输入开始时间
		if (!input.bInitStartTimeFlag)
		{
			input.bInitStartTimeFlag = true;
			input.startTime = GetTickCount();
		}

		bAllFinishedFlag = false;

		//判断延迟
		if (GetTickCount() - input.startTime < (DWORD)input.delay)
			break;

		//等待类型的话，则继续判断是否已到时间
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
			m_mainWnd->AddTipInfo("脚本已运行完毕，请手动选择其他脚本执行");
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

		//查询图片是否超时---放在执行之后，这样保证至少先执行一次对比才会超时
		if ( input.findPicOvertime != -1
			&& InputType::Pic == input.type
			&& ( GetTickCount() - input.startTime > ( DWORD )( input.findPicOvertime + input.delay ) )
			&& !input.bFindPicFlag )
		{
			input.bFindPicOvertimeFlag = true;
			//判断超时指令跳转
			if (-1 != input.findPicOvertimeJumpIndex)
			{
				(0xffff != input.findPicOvertimeJumpIndex) ? m_mainWnd->JumpInput(input.findPicOvertimeJumpIndex, inputVec) :
					(m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicOvertimeJumpModule, inputVec));
				break;
			}
			continue;
		}

		//如果没有找到图片就跳过，继续处理这一项
		if (InputType::Pic == input.type && !input.bFindPicFlag)
		{
			break;
		}
		//图片对比成功指令跳转
		else if (InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex)
		{
			(0xffff != input.findPicSucceedJumpIndex) ? m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, inputVec) :
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, inputVec);
			break;
		}

		//处理完后，如果已重复次数等于需要重复的次数，就标记为处理完毕（目前重复次数不对图片对比流程生效）
		input.alreadyRepeatCount += 1;
		if (input.alreadyRepeatCount >= input.repeatCount)
		{
			input.bFinishFlag = true;
		}
		else
		{
			//没有达到重复次数时，重置开始时间，并判断是否跳转后，break继续判断延迟
			input.startTime = GetTickCount();
			//这里借用了找图成功跳转索引的变量，因为找图我们都不会使用多次，我也就不想再多加结构体变量了，目前的多次重复命令都用于延迟上
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

	//设置指定的模拟器类型
	int index = m_ui->cmb_sim->currentIndex();
	m_mainWnd->SetSimWndType((SimWndType)index);
	m_mainWnd->AddTipInfo("脚本已停止运行");
}

void PlayerUI::StartScript()
{
	Init();

	//start时重新获取一次挂机设置的延迟
	m_emergencySetting.interval = m_ui->edt_emergencyInterval->text().toInt();
	m_devSetting.interval = m_ui->edt_devInterval->text().toInt();
	m_delegateSetting.interval = m_ui->edt_delegateInterval->text().toInt();
	m_recruitSetting.interval = m_ui->edt_recruitInterval->text().toInt();

	//定时设置
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

	//重置副本次数用尽标记（因为可能从一般副本选择了剧情副本）
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
