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
	//链接对应timer的函数
	m_mapStatusTimer.connect(&m_mapStatusTimer, &QTimer::timeout, this, &PlayerUI::UpdateMapStatusRecognizeScript);
	m_nextStepTimer.connect(&m_nextStepTimer, &QTimer::timeout, this, &PlayerUI::UpdateNextStepScript);
	m_mapPosSelectTimer.connect(&m_mapPosSelectTimer, &QTimer::timeout, this, &PlayerUI::UpdateMapPositionSelectScript);
	m_mapRecognizeTimer.connect(&m_mapRecognizeTimer, &QTimer::timeout, this, &PlayerUI::UpdateAllMapRecognizeAndBattleScript);

	//预先初始化不会变化的inputVec
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("map_status_recognize").c_str(), m_mapStatusInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("next_step").c_str(), m_nextStepInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
}

void PlayerUI::UpdateMapStatusRecognizeScript()
{
	UpdateMapRecognizeInputDataVector(m_mapStatusCmpParam);

	//根据状态开始和关闭对应的timer执行脚本，提高效率，减少错误点击
	if (m_mapStatusOutputParam >= ZZ_Map_Param::Battle_deploy
		&& m_mapStatusOutputParam <= ZZ_Map_Param::Battle_end)
	{
		if (!m_mapRecognizeTimer.isActive())
		{
			//因为战斗脚本完成后，现在都会停止，不会跳转回地图查找，这样处理比较简单，否则还涉及到跳转回地图查找后又进入战斗脚本的问题，具体游戏这边的脚本更新都有所改动，停止脚本不再停止timer，而是把inputVec清空，所以当我们需要再次开启地图识别的时候，我们需要先加载识别文件（因为这时候vec是空）
			m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize").c_str(), m_mapRecognizeInputVec);
			m_mapRecognizeTimer.start(2);
			m_mainWnd->AddTipInfo("已进入战斗场景");
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
			m_mainWnd->AddTipInfo("已从战斗场景退出");
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
	//根据游戏状态，动态加载切换各个脚本

	//情景发生变化时，才替换一次脚本
	if (m_lastStatusParam != m_mapStatusOutputParam)
	{
		m_lastStatusParam = m_mapStatusOutputParam;

		//赏金
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
		UpdateNormalInputDataVector(-1, m_mapRecognizeInputVec);
	}
}

void PlayerUI::UpdateMapRecognizeInputDataVector(int cmpParam)
{
	if (m_mapStatusInputVec.size() == 0)
	{
		m_mainWnd->AddTipInfo("脚本中命令数为0，线程退出...");
		return;
	}

	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	m_mainWnd->UpdateGameWindowSize();
	//强制更新窗口内容，即便窗口最小化
// 	::SetActiveWindow(m_hWnd);
// 	InvalidateRect(m_hWnd, nullptr, TRUE);
// 	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
// 	PostMessage(m_hWnd, WM_ACTIVATE, 0, 0);
// 	PostMessage(m_hWnd, WM_ERASEBKGND, 0, 0);
// 	PostMessage(m_hWnd, WM_PAINT, 0, 0);

	//需要把父窗口设置bottom属性，才不会弹出？？
// 	::SetWindowPos( m_hParentWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
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
		if (GetTickCount() - input.startTime < (DWORD)input.delay)
			break;

		//查询图片是否超时
		if (input.findPicOvertime != -1 && InputType::Pic == input.type && (GetTickCount() - input.startTime > (DWORD)(input.findPicOvertime + input.delay)))
		{
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

		//如果没有找到图片就跳过，继续处理这一项
		if (InputType::Pic == input.type && !input.bFindPicFlag)
		{
			break;
		}
		//图片对比成功指令跳转
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

void PlayerUI::UpdateNormalInputDataVector(int cmpParam, std::vector<InputData> &inputVec)
{
	if (inputVec.size() == 0)
	{
		m_mainWnd->AddTipInfo("脚本中命令数为0，线程退出...");
		return;
	}

	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	m_mainWnd->UpdateGameWindowSize();
	//强制更新窗口内容，即便窗口最小化
// 	::SetActiveWindow(m_hWnd);
// 	InvalidateRect(m_hWnd, nullptr, TRUE);
// 	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
// 	PostMessage(m_hWnd, WM_ACTIVATE, 0, 0);
// 	PostMessage(m_hWnd, WM_ERASEBKGND, 0, 0);
// 	PostMessage(m_hWnd, WM_PAINT, 0, 0);

	//需要把父窗口设置bottom属性，才不会弹出？？
// 	::SetWindowPos( m_hParentWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
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

		//查询图片是否超时
		if (input.findPicOvertime != -1 && InputType::Pic == input.type && (GetTickCount() - input.startTime > (DWORD)(input.findPicOvertime + input.delay)))
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
	m_mapStatusTimer.stop();
	m_nextStepTimer.stop();
	m_mapPosSelectTimer.stop();
	m_mapStatusOutputParam = 0;
	m_lastStatusParam = -1;

	//设置指定的模拟器类型
	int index = m_ui->cmb_sim->currentIndex();
	m_mainWnd->SetSimWndType((SimWndType)index);
	m_mainWnd->AddTipInfo("脚本已停止运行");
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
