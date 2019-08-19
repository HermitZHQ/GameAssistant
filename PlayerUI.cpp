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
	, m_mapRecognizeCmpParam(-1)
	, m_mapRecognizeOutputParam(0)
	, m_lastOutputParam(-1)
	, m_nextStepCmpParam(-1)
	, m_nextStepOutputParam(0)
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
	m_mapRecognizeTimer.connect(&m_mapRecognizeTimer, &QTimer::timeout, this, &PlayerUI::UpdateMapStatusRecognizeScript);
	m_nextStepTimer.connect(&m_nextStepTimer, &QTimer::timeout, this, &PlayerUI::UpdateNextStepScript);
	m_autoTimer.connect(&m_autoTimer, &QTimer::timeout, this, &PlayerUI::UpdateAutoScript);
	m_rewardMapRecognizeTimer.connect(&m_rewardMapRecognizeTimer, &QTimer::timeout, this, &PlayerUI::UpdateRewardMapRecognizeScript);

	//预先初始化不会变化的inputVec
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("map_status_recognize").c_str(), m_mapRecognizeInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("next_step").c_str(), m_nextStepInputVec);
	m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_map_recognize_reward").c_str(), m_rewardMapRecognizeInputVec);
}

void PlayerUI::UpdateMapStatusRecognizeScript()
{
	UpdateMapRecognizeInputDataVector(m_mapRecognizeCmpParam);
}

void PlayerUI::UpdateNextStepScript()
{
	if (m_nextStepInputVec.size())
	{
		UpdateNormalInputDataVector(m_nextStepCmpParam, m_nextStepInputVec);
	}
}

void PlayerUI::UpdateAutoScript()
{
	//根据游戏状态，动态加载切换各个脚本

	//情景发生变化时，才替换一次脚本
	if (m_lastOutputParam != m_mapRecognizeOutputParam)
	{
		m_lastOutputParam = m_mapRecognizeOutputParam;

		//赏金
		if (m_ui->chk_reward->isChecked())
		{
			if (ZZ_Map_Param::Battle_Main == m_mapRecognizeOutputParam)
			{
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("zz_battle_findIcon_a4_reward").c_str(), m_autoInputVec);

				m_rewardMapRecognizeTimer.stop();
				m_rewardMapRecognizeTimer.start(2);
			}
			else if (ZZ_Map_Param::Lobby == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Dev == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Recruit == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Member == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Team == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Battle_A1 == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Battle_A2 == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Battle_A3 == m_mapRecognizeOutputParam
				|| ZZ_Map_Param::Battle_A4 == m_mapRecognizeOutputParam)
			{
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("to_battle_main").c_str(), m_autoInputVec);
			}
			else
			{
				if (0 != m_lastOutputParam)
				{
					m_mainWnd->ShowMessageBox(std::string("当前界面无法跳转执行赏金任务_").append(std::to_string(m_lastOutputParam)).c_str());
				}
			}
		}
	}

	if (m_autoInputVec.size())
	{
		UpdateNormalInputDataVector(-1, m_autoInputVec);
	}
}

void PlayerUI::UpdateRewardMapRecognizeScript()
{
	if (m_rewardMapRecognizeInputVec.size())
	{
		UpdateNormalInputDataVector(-1, m_rewardMapRecognizeInputVec);
	}
}

void PlayerUI::UpdateMapRecognizeInputDataVector(int cmpParam)
{
	if (m_mapRecognizeInputVec.size() == 0)
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
	for (auto &input : m_mapRecognizeInputVec)
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
				(0xffff != input.findPicOvertimeJumpIndex) ? m_mainWnd->JumpInput(input.findPicOvertimeJumpIndex, m_mapRecognizeInputVec) :
					(m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicOvertimeJumpModule, m_mapRecognizeInputVec));
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
			m_mapRecognizeOutputParam = input.outputParam;

			(0xffff != input.findPicSucceedJumpIndex) ? m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, m_mapRecognizeInputVec) :
				m_mainWnd->LoadScriptModuleFileToSpecificInputVec(input.findPicSucceedJumpModule, m_mapRecognizeInputVec);
			break;
		}
		else if (InputType::Pic == input.type && input.bFindPicFlag)
		{
			m_mapRecognizeOutputParam = input.outputParam;
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
				m_mainWnd->JumpInput(input.findPicSucceedJumpIndex, m_mapRecognizeInputVec);
			}
			break;
		}
	}

	if (bAllFinishedFlag)
	{
		m_mainWnd->ResetAllInputFinishFlag(m_mapRecognizeInputVec);
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
	m_mapRecognizeTimer.stop();
	m_nextStepTimer.stop();
	m_autoTimer.stop();
	m_mapRecognizeOutputParam = 0;
	m_lastOutputParam = -1;

	//设置指定的模拟器类型
	int index = m_ui->cmb_sim->currentIndex();
	m_mainWnd->SetSimWndType((SimWndType)index);
	m_mainWnd->AddTipInfo("脚本已停止运行");
}

void PlayerUI::StartScript()
{
	m_mapRecognizeTimer.start(5);
	m_nextStepTimer.start(10);
	m_autoTimer.start(3);
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
