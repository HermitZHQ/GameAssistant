#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ZZPicCompareStrategy.h"
#include <tchar.h>
#include "QThread"
#include "QFileDialog"
#include "QtWidgets/QMessageBox"
#include "QDebug"
#include <time.h>
#include "ui_player.h"


#ifdef _DEBUG
#import "./NtpTimed.tlb"
#else
#import "./NtpTime.tlb"
#endif
using namespace NtpTime;

const unsigned int g_crypt = 0xe511f;

std::unordered_map<InputType, QString> InputTypeStrMap = {
	{InputType::Mouse, Q8("鼠标")},
	{InputType::Keyboard, Q8("键盘")},
	{InputType::Pic, Q8("图片")},
	{InputType::StopScript, Q8("停止")},
};
std::unordered_map<QString, InputType> StrInputTypeMap = {
	{ Q8( "鼠标" ), InputType::Mouse},
	{ Q8( "键盘" ), InputType::Keyboard},
	{ Q8( "图片" ), InputType::Pic},
	{ Q8( "停止" ), InputType::StopScript},
};

std::unordered_map<OpType, QString> OpTypeStrMap = {
	{OpType::Click, Q8("点击")},
	{OpType::Press, Q8("按住")},
	{OpType::Move, Q8("移动")},
	{OpType::Release, Q8("释放")},
};
std::unordered_map<QString, OpType> StrOpTypeMap = {
	{ Q8( "点击" ), OpType::Click },
	{ Q8( "按住" ), OpType::Press },
	{ Q8( "移动" ), OpType::Move },
	{ Q8( "释放" ), OpType::Release },
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
	, m_stopFlag(false)
	, m_wndWidth(890)
	, m_wndHeight(588)
	, m_hGameWnd(nullptr)
	, m_picCompareStrategy(new ZZPicCompareStrategy)
	, m_playerUI(this)
	, m_year(0)
	, m_lisenceLeftSecond(0)
	, m_bShowHideFlag(true)
	, m_simWndType(None)
	, m_simWndInfoMap({
		{Thunder, SimWndInfo(QString::fromLocal8Bit("雷电模拟器"))},
		{MuMu, SimWndInfo(QString::fromLocal8Bit("重装战姬 - MuMu模拟器"))},
		})
	, m_curSimWndInfo(QString::fromLocal8Bit("雷电模拟器"))
{
	CoUninitialize();
	/*auto res = */CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	char cTmp[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, cTmp);
	std::string strComCmd = "/c RegAsm \"";
	strComCmd.append(cTmp);
	//注册com组件
#ifdef _DEBUG
	strComCmd.append("/NtpTime.dll\" /tlb:NtpTimed.tlb /codebase");
#else
	strComCmd.append("/NtpTime.dll\" /tlb:NtpTime.tlb /codebase");
#endif
	/*auto res2 = */ShellExecuteA(nullptr, "open", "cmd", strComCmd.c_str(), "C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319", SW_HIDE);

	Sleep(2500);

	// 	auto res = CoInitialize(nullptr);
	ITimeHelperPtr timeHelper(__uuidof(TimeHelper));
	std::string strRes = timeHelper->getWebTime(&m_year, &m_month, &m_day, &m_hour, &m_minute, &m_second);
	m_macClient = timeHelper->getMac();

	if (m_year == 666)
	{
		ShowMessageBox("请保持网络通畅，重启客户端");
		destroy();
		return;
	}


#ifdef DEV_VER
	m_ui->setupUi(this);
	m_ui->edt_mac->setText(m_macClient);
	CheckLisence();
	setParent(&m_bkgUI);
	m_picCompareStrategy->SetUi(m_ui);
	m_bkgUI.setGeometry(geometry());
	setWindowTitle("Develop-Ver 1.0.8");

	//set table view model
	m_ui->tv_inputVec->setModel(&m_inputDataModel);
	m_ui->tv_inputVec->setItemDelegate( &m_itemDelegate );
#else
	m_bkgUI.setWindowTitle("Game-Assistant");
	if (!CheckLisence())
	{
		destroy();
		return;
	}
	setGeometry(QRect(0, 0, 0, 0));
	setParent(&m_bkgUI);
	setVisible(false);
	m_playerUI.setParent(&m_bkgUI);
	m_playerUI.show();
	m_bkgUI.setGeometry(m_playerUI.geometry());
#endif

	m_bkgUI.show();
}

MainWindow::~MainWindow()
{
	CoUninitialize();
	setParent(nullptr);
	m_playerUI.setParent(nullptr);
	delete m_ui;
}

void MainWindow::PostMsgThread()
{
	// 	ui->list_tip->addItem(QString::fromLocal8Bit("投递消息线程已开始运作..."));
	if (m_inputVec.size() == 0)
	{
		m_timer.stop();
		AddTipInfo("脚本中命令数为0，线程退出...");
		return;
	}

	//执行脚本时为顺序执行，每执行完一个finishflag就标识为true，都完成以后重置所有标识，进行反复循环
// 	while (m_hWnd && !m_stopFlag)
// 	{

	//检测游戏本体窗口是否已经设置到了指定大小
	m_curSimWndInfo.CheckGameWndSize(m_wndWidth, m_wndHeight);
	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	UpdateGameWindowSize();
	//强制更新窗口内容，即便窗口最小化
// 	::SetActiveWindow(m_hWnd);
// 	InvalidateRect(m_hWnd, nullptr, TRUE);
// 	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
// 	PostMessage(m_hWnd, WM_ACTIVATE, 0, 0);
// 	PostMessage(m_hWnd, WM_ERASEBKGND, 0, 0);
// 	PostMessage(m_hWnd, WM_PAINT, 0, 0);

#ifdef DEV_VER
	int iCurCount = m_ui->list_tip->count();
	static int iLastCount = iCurCount;
	//检测提示列表大小，超过一定值清空
	if (iCurCount > 500)
	{
		m_ui->list_tip->clear();
	}
	else if (iLastCount != iCurCount)
	{
		m_ui->list_tip->scrollToBottom();
	}
	iLastCount = iCurCount;
#endif

	//需要把父窗口设置bottom属性，才不会弹出？？
// 	::SetWindowPos( m_hParentWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
	bool bAllFinishedFlag = true;
	int index = -1;
	for (auto &input : m_inputVec)
	{
		++index;
		//判断完成
		if ((input.bFinishFlag && input.type != Pic) || (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag)))
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
				(0xffff != input.findPicOvertimeJumpIndex) ? JumpInput(input.findPicOvertimeJumpIndex) :
					(LoadScriptModuleFile(input.findPicOvertimeJumpModule));
				break;
			}
			continue;
		}

#ifdef DEV_VER
		m_ui->edt_cmpPic->setText(QString::fromLocal8Bit("[").toStdString().append(std::to_string(index)).append("]").c_str());
#endif

		switch (input.type)
		{
		case Mouse:
			HandleMouseInput(input);
			break;
		case Keyboard:
			HandleKeyboardInput(input);
			break;
		case Pic:
			HandleGameImgCompare(input);
			break;
		case StopScript:
		{
			m_timer.stop();
			AddTipInfo("脚本已运行完毕，请手动选择其他脚本执行");
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
			(0xffff != input.findPicSucceedJumpIndex) ? JumpInput(input.findPicSucceedJumpIndex) :
				LoadScriptModuleFile(input.findPicSucceedJumpModule);
			break;
		}

		//只要处理过了，就标记为处理完毕
		input.bFinishFlag = true;
	}

	if (bAllFinishedFlag)
	{
		ResetAllInputFinishFlag();
	}

	// 		Sleep(1);
	// 	}


	// 	ui->list_tip->addItem(QString::fromLocal8Bit("线程已退出..."));
}

BOOL CALLBACK MainWindow::EnumChildProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	SimWndInfo *pInfo = (SimWndInfo*)lParam;

	pInfo->AddLayer();
	//不使用名称查找的话，直接返回找到的第一个窗口，目前mumu和雷电都是这样
	if (!pInfo->bUseLayerNameFlag[pInfo->curLayer])
	{
		pInfo->layerWnd[pInfo->curLayer] = hwnd;
		return pInfo->CheckFindFinishe() ? FALSE : TRUE;
	}

	char strTmp[MAX_PATH] = { 0 };
	GetWindowTextA(hwnd, strTmp, MAX_PATH);
	if (strcmp(strTmp, pInfo->layerWndName[pInfo->curLayer].toLocal8Bit().toStdString().c_str()) == 0)
	{
		pInfo->layerWnd[pInfo->curLayer] = hwnd;
		return pInfo->CheckFindFinishe() ? FALSE : TRUE;
	}

	return TRUE;
}

void MainWindow::OnBtnStartClick()
{
	m_stopFlag = false;
	if (nullptr == m_hGameWnd)
	{
		return;
	}
	ResetAllInputFinishFlag();

	// 	std::thread t(&MainWindow::PostMsgThread, this);
	// 	t.detach();

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(PostMsgThread()));
	m_timer.setInterval(1);
	// 	m_timer.setSingleShot(true);
	m_timer.start();
	AddTipInfo("开始脚本处理...");
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	AddTipInfo("停止脚本处理...");
}

void MainWindow::OnBtnAddInput()
{
	//添加前先更新游戏窗口大小
	UpdateGameWindowSize();

	int repeatTime = m_ui->edt_repeat->text().toShort();

	InputData input;
	GetInputData(input);

	for (int i = 0; i < repeatTime; ++i)
	{
		m_inputVec.push_back(input);
	}

	AddTipInfo(std::string("已添加").append(std::to_string(repeatTime)).append("条指令").c_str());

	RefreshInputVecUIList();
}

void MainWindow::GetInputData(InputData &input)
{
	//----comment
	strcpy_s(input.comment, PATH_LEN, m_ui->edt_comment->text().toLocal8Bit().toStdString().c_str());
	//----keyboard
	input.type = (InputType)m_ui->cb_inputType->currentIndex();
	input.opType = (OpType)m_ui->cb_opType->currentIndex();
	input.delay = m_ui->edt_delay->text().toShort();
	input.vk = m_ui->edt_vk->text().toLocal8Bit()[0];

	if (InputType::StopScript == input.type)
	{
		return;
	}

	//----mouse
// 	if (InputType::Mouse == input.type || InputType::Pic == input.type)
// 	{
	input.x = m_ui->edt_x->text().toShort();
	input.y = m_ui->edt_y->text().toShort();
	input.xRate = (float)input.x / (float)m_gameWndSize.x;
	input.yRate = (float)input.y / (float)m_gameWndSize.y;
	// 	}

		//----pic
	// 	if (InputType::Pic == input.type)
	// 	{
	input.bCmpPicCheckFlag = m_ui->chk_cmpPicClick->isChecked();
	input.findPicOvertime = m_ui->edt_findPicOvertime->text().toShort();
	input.findPicSucceedJumpIndex = m_ui->edt_succeedJump->text().toInt();
	input.findPicOvertimeJumpIndex = m_ui->edt_overtimeJump->text().toInt();
	input.cmpPicRate = m_ui->edt_rate->text().toFloat();
	strcpy_s(input.findPicSucceedJumpModule, PATH_LEN, m_ui->edt_succeedJumpModule->text().toLocal8Bit().toStdString().c_str());
	strcpy_s(input.findPicOvertimeJumpModule, PATH_LEN, m_ui->edt_overtimeJumpModule->text().toLocal8Bit().toStdString().c_str());

	input.x2 = m_ui->edt_x2->text().toShort();
	input.y2 = m_ui->edt_y2->text().toShort();
	input.xRate2 = (float)input.x2 / (float)m_gameWndSize.x;
	input.yRate2 = (float)input.y2 / (float)m_gameWndSize.y;
	strcpy_s(input.picPath, PATH_LEN, m_ui->edt_picPath->text().toLocal8Bit().toStdString().c_str());
	// 	}
}

void MainWindow::UpdateInputData(InputData &input)
{
	input.xRate = (float)input.x / (float)m_gameWndSize.x;
	input.yRate = (float)input.y / (float)m_gameWndSize.y;
	input.xRate2 = (float)input.x2 / (float)m_gameWndSize.x;
	input.yRate2 = (float)input.y2 / (float)m_gameWndSize.y;
}

void MainWindow::OnBtnDelLastInput()
{
	if (m_inputVec.size() <= 0)
		return;

	m_inputVec.pop_back();

	AddTipInfo("已删除上一条指令");
	AddTipInfo(std::string("还剩下").append(std::to_string(m_inputVec.size())).append("条指令").c_str());

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDelAllInput()
{
	m_inputVec.clear();
	AddTipInfo("已删除所有指令");

	RefreshInputVecUIList();
}

void MainWindow::OnBtnUpdateAllInput()
{
	UpdateGameWindowSize();

	auto it = m_inputVec.begin();
	for (; it != m_inputVec.end(); ++it)
	{
		UpdateInputData(*it);
	}

	RefreshInputVecUIList();
	AddTipInfo("已更新所有指令");
}

void MainWindow::OnBtnDelSelectInputClick()
{
	auto index = m_ui->list_inputVec->currentIndex();

	auto it = m_inputVec.begin();
	for (int i = 0; i < m_inputVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		m_inputVec.erase(it);
		break;
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDel3Inputs()
{
	auto index = m_ui->list_inputVec->currentIndex();
	if (index.row() + 2 > m_inputVec.size() - 1)
	{
		ShowMessageBox("无法连续删除3条指令");
		return;
	}

	for (int j = 0; j < 3; ++j)
	{
		auto it = m_inputVec.begin();
		for (int i = 0; i < m_inputVec.size(); ++i, ++it)
		{
			if (i != index.row())
				continue;

			m_inputVec.erase(it);
			break;
		}
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnInputListClick()
{
	auto index = m_ui->list_inputVec->currentIndex();
	m_ui->edt_indexStart->setText(std::string(std::to_string(index.row())).c_str());

	auto it = m_inputVec.begin();
	for (int i = 0; i < m_inputVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		//把当前input的咨询显示到editbox中
		m_ui->cb_inputType->setCurrentIndex(it->type);
		m_ui->cb_opType->setCurrentIndex(it->opType);
		m_ui->edt_vk->setText(std::string(1, it->vk).c_str());
		m_ui->edt_delay->setText(std::to_string(it->delay).c_str());
		m_ui->edt_comment->setText(QString::fromLocal8Bit(it->comment));

		m_ui->edt_x->setText(std::to_string(it->x).c_str());
		m_ui->edt_y->setText(std::to_string(it->y).c_str());
		m_ui->edt_x2->setText(std::to_string(it->x2).c_str());
		m_ui->edt_y2->setText(std::to_string(it->y2).c_str());

		m_ui->edt_rate->setText(std::to_string(it->cmpPicRate).c_str());
		m_ui->edt_picPath->setText(it->picPath);
		m_ui->edt_findPicOvertime->setText(std::to_string(it->findPicOvertime).c_str());
		m_ui->edt_succeedJump->setText(std::to_string(it->findPicSucceedJumpIndex).c_str());
		m_ui->edt_overtimeJump->setText(std::to_string(it->findPicOvertimeJumpIndex).c_str());
		m_ui->edt_overtimeJumpModule->setText(it->findPicOvertimeJumpModule);
		m_ui->edt_succeedJumpModule->setText(it->findPicSucceedJumpModule);
		m_ui->chk_cmpPicClick->setChecked(it->bCmpPicCheckFlag);

		break;
	}
}

void MainWindow::OnBtnUpdateSelectInputClick()
{
	UpdateGameWindowSize();
	auto index = m_ui->list_inputVec->currentIndex();

	auto it = m_inputVec.begin();
	for (int i = 0; i < m_inputVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		GetInputData(*it);

		break;
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnInsertInputClick()
{
	int index = m_ui->edt_insertIndex->text().toInt();

	auto size = m_inputVec.size();
	auto it = m_inputVec.begin();
	for (int i = 0; i < size; ++i, ++it)
	{
		if (index == i)
		{
			InputData input;
			GetInputData(input);

			m_inputVec.insert(it, input);

			break;
		}
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnInsertDrag()
{
	int index = m_ui->edt_insertIndex->text().toInt();
	int insertCount = 0;
	auto size = m_inputVec.size();

	while (insertCount < 3 && size > 0)
	{
		auto it = m_inputVec.begin();
		for (int i = 0; i < size; ++i, ++it)
		{
			if (index == i)
			{
				InputData input;
				GetInputData(input);
				input.type = Mouse;
				input.delay = 250;//默认插入250延迟

				if (0 == insertCount)
				{
					input.opType = Press;
					strcpy_s(input.comment, MAX_PATH, "拖动-1");
				}
				else if (1 == insertCount)
				{
					input.opType = Move;
					input.x = input.x2;
					input.y = input.y2;
					input.xRate = input.xRate2;
					input.yRate = input.yRate2;
					strcpy_s(input.comment, MAX_PATH, "拖动-2");
				}
				else if (2 == insertCount)
				{
					input.opType = Release;
					input.x = input.x2;
					input.y = input.y2;
					input.xRate = input.xRate2;
					input.yRate = input.yRate2;
					strcpy_s(input.comment, MAX_PATH, "拖动-3");
				}

				m_inputVec.insert(it, input);

				++index;
				++insertCount;
				break;
			}
		}
	}

	//插入后更新插入位置
	m_ui->edt_insertIndex->setText(std::to_string(index).c_str());
	//调换xy1，2的位置，方便下次输入
	m_ui->edt_x->setText(m_ui->edt_x2->text());
	m_ui->edt_y->setText(m_ui->edt_y2->text());

	RefreshInputVecUIList();
}

void MainWindow::RefreshInputVecUIList()
{
	m_ui->list_inputVec->clear();

	int index = -1;
	for (auto &input : m_inputVec)
	{
		++index;
		std::string strTmp;
		strTmp = "<";
		strTmp += std::to_string(index);
		strTmp += ">";
		strTmp += "(";
		strTmp += input.comment;
		strTmp += ")";
		strTmp += " 类型:";
		switch (input.type)
		{
		case Mouse:
			strTmp += "鼠标";
			break;
		case Keyboard:
			strTmp += "键盘";
			break;
		case Pic:
			strTmp += "图片";
			break;
		case StopScript:
			strTmp += "停止";
			break;
		default:
			strTmp += "未知";
			break;
		}

		if (InputType::Pic != input.type && InputType::StopScript != input.type)
		{
			strTmp += " 方式:";
			switch (input.opType)
			{
			case Click:
				strTmp += "点击";
				break;
			case Press:
				strTmp += "按住";
				break;
			case Move:
				strTmp += "移动";
				break;
			case Release:
				strTmp += "释放";
				break;
			}

			strTmp += " 延迟:";
			strTmp += std::to_string(input.delay);
		}

		if (InputType::Keyboard == input.type && InputType::StopScript != input.type)
		{
			strTmp += " 键值:";
			strTmp += input.vk;
		}

		if ((InputType::Mouse == input.type || InputType::Pic == input.type) && InputType::StopScript != input.type)
		{
			strTmp += " [x:";
			strTmp += std::to_string(input.x);
			strTmp += " y:";
			strTmp += std::to_string(input.y);
			strTmp += "] xRate:";
			strTmp += Left2Precision(input.xRate);
			strTmp += " yRate:";
			strTmp += Left2Precision(input.yRate);
		}

		if (InputType::Pic == input.type && InputType::StopScript != input.type)
		{
			strTmp += " [x2:";
			strTmp += std::to_string(input.x2);
			strTmp += " y2:";
			strTmp += std::to_string(input.y2);
			strTmp += "] xRate2:";
			strTmp += Left2Precision(input.xRate2);
			strTmp += " yRate2:";
			strTmp += Left2Precision(input.yRate2);

			strTmp += " 路径:";
			strTmp += input.picPath;
		}

		m_ui->list_inputVec->addItem(QString::fromLocal8Bit(strTmp.c_str()));
	}
}

void MainWindow::RefreshInputModuleVecUIList()
{
	m_ui->list_inputModule->clear();

	int index = -1;
	for (auto &input : m_inputModuleVec)
	{
		++index;
		std::string strTmp;
		strTmp = "<";
		strTmp += std::to_string(index);
		strTmp += ">";
		strTmp += "(";
		strTmp += input.comment;
		strTmp += ")";
		strTmp += " 类型:";
		switch (input.type)
		{
		case Mouse:
			strTmp += "鼠标";
			break;
		case Keyboard:
			strTmp += "键盘";
			break;
		case Pic:
			strTmp += "图片";
			break;
		case StopScript:
			strTmp += "停止";
			break;
		default:
			strTmp += "未知";
			break;
		}

		if (InputType::Pic != input.type && InputType::StopScript != input.type)
		{
			strTmp += " 方式:";
			switch (input.opType)
			{
			case Click:
				strTmp += "点击";
				break;
			case Press:
				strTmp += "按住";
				break;
			case Move:
				strTmp += "移动";
				break;
			case Release:
				strTmp += "释放";
				break;
			}

			strTmp += " 延迟:";
			strTmp += std::to_string(input.delay);
		}

		if (InputType::Keyboard == input.type && InputType::StopScript != input.type)
		{
			strTmp += " 键值:";
			strTmp += input.vk;
		}

		if ((InputType::Mouse == input.type || InputType::Pic == input.type) && InputType::StopScript != input.type)
		{
			strTmp += " [x:";
			strTmp += std::to_string(input.x);
			strTmp += " y:";
			strTmp += std::to_string(input.y);
			strTmp += "] xRate:";
			strTmp += Left2Precision(input.xRate);
			strTmp += " yRate:";
			strTmp += Left2Precision(input.yRate);
		}

		if (InputType::Pic == input.type && InputType::StopScript != input.type)
		{
			strTmp += " [x2:";
			strTmp += std::to_string(input.x2);
			strTmp += " y2:";
			strTmp += std::to_string(input.y2);
			strTmp += "] xRate2:";
			strTmp += Left2Precision(input.xRate2);
			strTmp += " yRate2:";
			strTmp += Left2Precision(input.yRate2);

			strTmp += " 路径:";
			strTmp += input.picPath;
		}

		m_ui->list_inputModule->addItem(QString::fromLocal8Bit(strTmp.c_str()));
	}
}

void MainWindow::OnBtnClearTipInfo()
{
	m_ui->list_tip->clear();
}

void MainWindow::OnBtnOverwrite()
{
	auto index = m_ui->list_inputVec->currentIndex();
	if (!index.isValid())
	{
		ShowMessageBox("没有选择有效的目标索引");
		return;
	}

	int overwriteNum = m_ui->edt_overwriteNum->text().toInt();
	int overwriteTargetIndex = m_ui->edt_overwriteIndex->text().toInt();
	int overwriteSrcIndex = index.row();

	auto size = m_inputVec.size();
	if (overwriteTargetIndex + (overwriteNum - 1) > size - 1
		|| overwriteTargetIndex < 0
		|| overwriteTargetIndex == overwriteSrcIndex)
	{
		ShowMessageBox("复制目标的起始索引值无效");
		return;
	}

	int alreadOverwriteNum = 0;
	for (decltype(size) i = 0; i < size; ++i)
	{
		if (i < overwriteSrcIndex)
		{
			continue;
		}

		if (alreadOverwriteNum < overwriteNum)
		{
			m_inputVec[overwriteTargetIndex + alreadOverwriteNum] = m_inputVec[i];
			++alreadOverwriteNum;
		}
		else
		{
			break;
		}
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnOverwriteDelay()
{
	auto index = m_ui->list_inputVec->currentIndex();
	if (!index.isValid())
	{
		ShowMessageBox("没有选择有效的目标索引");
		return;
	}

	int overwriteNum = m_ui->edt_overwriteNum->text().toInt();
	int overwriteTargetIndex = m_ui->edt_overwriteIndex->text().toInt();
	int overwriteSrcIndex = index.row();

	auto size = m_inputVec.size();
	if (overwriteTargetIndex + (overwriteNum - 1) > size - 1
		|| overwriteTargetIndex < 0
		|| overwriteTargetIndex == overwriteSrcIndex)
	{
		ShowMessageBox("复制目标的起始索引值无效");
		return;
	}

	short delay = m_ui->edt_delay->text().toShort();
	for (decltype(size) i = 0; i < size; ++i)
	{
		if (i < overwriteSrcIndex || i > overwriteTargetIndex)
		{
			continue;
		}

		m_inputVec[i].delay = delay;
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnSetOverwriteTargetIndex()
{
	auto index = m_ui->list_inputVec->currentIndex();
	m_ui->edt_overwriteIndex->setText(std::to_string(index.row()).c_str());
	m_ui->edt_indexEnd->setText(std::to_string(index.row()).c_str());
}

void MainWindow::ShowMessageBox(const char *content)
{
	QMessageBox mb;
	mb.setWindowTitle("Info");
	mb.setText(QString::fromLocal8Bit(content));
	mb.setDefaultButton(QMessageBox::Ok);
	mb.exec();
}

void MainWindow::AddTipInfo(const char *str, bool bConvertFlag)
{
#ifdef DEV_VER
	m_ui->list_tip->addItem((bConvertFlag ? (QString::fromLocal8Bit(str)) : (str)));
	m_ui->list_tip->scrollToBottom();
#else
	m_playerUI.GetUI()->list_tip->addItem((bConvertFlag ? (QString::fromLocal8Bit(str)) : (str)));
	m_playerUI.GetUI()->list_tip->scrollToBottom();
#endif
}

void MainWindow::OnBtnShowHide()
{
	const int width = 470;
	auto geo = m_bkgUI.geometry();

	if (m_bShowHideFlag)
	{
		m_bkgUI.setGeometry(geo.left(), geo.top(), geo.width() - width, geo.height());
	}
	else
	{
		m_bkgUI.setGeometry(geo.left(), geo.top(), geo.width() + width, geo.height());
	}

	m_bShowHideFlag = !m_bShowHideFlag;
}

void MainWindow::OnBtnGetModule()
{
	int indexStart = m_ui->edt_indexStart->text().toInt();
	int indexEnd = m_ui->edt_indexEnd->text().toInt();
	auto size = m_inputVec.size();
	m_inputModuleVec.clear();

	if (indexStart < 0 || indexStart > size - 1
		|| indexEnd < 0 || indexEnd > size - 1
		|| indexEnd < indexStart)
	{
		ShowMessageBox("模块索引设置错误");
		return;
	}

	for (int i = 0; i < size; ++i)
	{
		if (i < indexStart || i > indexEnd) 
		{
			continue;
		}

		m_inputModuleVec.push_back(m_inputVec[i]);
	}

	RefreshInputModuleVecUIList();
}

void MainWindow::OnBtnInsertModule()
{
	int index = m_ui->edt_insertIndex->text().toInt();
	auto size = m_inputVec.size();
	auto insertCount = m_inputModuleVec.size();

	if (0 == size)
	{
		for (auto &input : m_inputModuleVec)
		{
			m_inputVec.push_back(input);
		}
	} 
	else
	{
		if (index > size - 1)
		{
			ShowMessageBox("插入索引错误");
			return;
		}

		int alreadyInsertCount = 0;
		while (size > 0 && alreadyInsertCount < insertCount)
		{
			int i = 0;
			for (auto it = m_inputVec.begin(); it != m_inputVec.end(); ++it, ++i)
			{
				if (i < index)
				{
					continue;
				}
	
				m_inputVec.insert(it, m_inputModuleVec[alreadyInsertCount]);
				++alreadyInsertCount;
				//连续插入时，需要把index后移
				++index;

				break;
			}
		}
	}

	RefreshInputVecUIList();
}

void MainWindow::OnBtnSaveModule()
{
	m_wndWidth = m_ui->edt_wndWidth->text().toInt();
	m_wndHeight = m_ui->edt_wndHeight->text().toInt();

	//按照二进制存储
	FILE *pFile = nullptr;
	std::string strFilePath = m_ui->edt_saveName->text().toLocal8Bit().toStdString();
	auto pos = strFilePath.find("/");
	if (std::string::npos == pos)
	{
		strFilePath = DEFAULT_MODULE_PATH + strFilePath;
	}

	QFileInfo file(strFilePath.c_str());
	if (file.exists() && file.isFile())
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("文件已存在，是否覆盖?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (QMessageBox::Cancel == ret)
		{
			AddTipInfo("已取消保存");
			return;
		}
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	//然后存入操作数组的大小以及数据
	int size = (int)m_inputModuleVec.size();
	fwrite(&size, sizeof(int), 1, pFile);
	for (auto &input : m_inputModuleVec)
	{
		fwrite(&input, sizeof(InputData), 1, pFile);
	}

	fclose(pFile);
	AddTipInfo("保存文件成功");
}

void MainWindow::OnBtnLoadModule()
{
	auto res = QFileDialog::getOpenFileName(this, "", DEFAULT_MODULE_PATH);
	if (res.compare("") == 0)
	{
		return;
	}

	// 	ui->edt_saveName->setText(res);
	LoadModuleFile(res.toLocal8Bit().toStdString().c_str());
}

void MainWindow::OnBtnDelSelectModuleInput()
{
	auto index = m_ui->list_inputModule->currentIndex();

	auto it = m_inputModuleVec.begin();
	for (int i = 0; i < m_inputModuleVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		m_inputModuleVec.erase(it);
		break;
	}

	RefreshInputModuleVecUIList();
}

void MainWindow::OnBtnModuleListClick()
{
	auto index = m_ui->list_inputModule->currentIndex();
	//m_ui->edt_indexStart->setText(std::string(std::to_string(index.row())).c_str());

	auto it = m_inputModuleVec.begin();
	for (int i = 0; i < m_inputModuleVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		//把当前input的咨询显示到editbox中
		m_ui->cb_inputType->setCurrentIndex(it->type);
		m_ui->cb_opType->setCurrentIndex(it->opType);
		m_ui->edt_vk->setText(std::string(1, it->vk).c_str());
		m_ui->edt_delay->setText(std::to_string(it->delay).c_str());
		m_ui->edt_comment->setText(QString::fromLocal8Bit(it->comment));

		m_ui->edt_x->setText(std::to_string(it->x).c_str());
		m_ui->edt_y->setText(std::to_string(it->y).c_str());
		m_ui->edt_x2->setText(std::to_string(it->x2).c_str());
		m_ui->edt_y2->setText(std::to_string(it->y2).c_str());

		m_ui->edt_rate->setText(std::to_string(it->cmpPicRate).c_str());
		m_ui->edt_picPath->setText(it->picPath);
		m_ui->edt_findPicOvertime->setText(std::to_string(it->findPicOvertime).c_str());
		m_ui->edt_succeedJump->setText(std::to_string(it->findPicSucceedJumpIndex).c_str());
		m_ui->edt_overtimeJump->setText(std::to_string(it->findPicOvertimeJumpIndex).c_str());
		m_ui->edt_overtimeJumpModule->setText(it->findPicOvertimeJumpModule);
		m_ui->edt_succeedJumpModule->setText(it->findPicSucceedJumpModule);
		m_ui->chk_cmpPicClick->setChecked(it->bCmpPicCheckFlag);

		break;
	}
}

void MainWindow::OnBtnClearModuleInput()
{
	m_inputModuleVec.clear();

	RefreshInputModuleVecUIList();
}

void MainWindow::OnBtnUpdateSelectModuleInput()
{
	UpdateGameWindowSize();
	auto index = m_ui->list_inputModule->currentIndex();

	auto it = m_inputModuleVec.begin();
	for (int i = 0; i < m_inputModuleVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		GetInputData(*it);

		break;
	}

	RefreshInputModuleVecUIList();
}

void MainWindow::OnBtnLisence()
{
	int lisenceMonth = m_ui->edt_month->text().toShort();
	if (lisenceMonth < 0 || lisenceMonth > 12)
	{
		ShowMessageBox("Lisence月份无效");
		return;
	}

	QString strMac = GetMAC();
	if (strMac.compare("") == 0)
	{
		return;
	}

	//按照二进制存储
	FILE *pFile = nullptr;
	std::string strFilePath = DEFAULT_PATH;
	strFilePath.append("Lisence.nn");

	QFileInfo file(strFilePath.c_str());
	if (file.exists() && file.isFile())
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("文件已存在，是否覆盖?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (QMessageBox::Cancel == ret)
		{
			AddTipInfo("已取消保存");
			return;
		}
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	srand(time(nullptr));
	int iRand = 0;
	//先写入20个int长度的随机值
	for (int i = 0; i < 20; ++i)
	{
		iRand = rand();
		fwrite(&iRand, sizeof(int), 1, pFile);
	}

	//中间写入日期内存，这里还是分别写入，我怕不同操作系统默认的结构体大小不同
	QDateTime curDate = QDateTime::currentDateTime();
	QDateTime endDate;
	if (0 != lisenceMonth)
	{
		endDate = curDate.addMonths(lisenceMonth);
	}
	else
	{
		endDate = curDate.addSecs(1800);
	}
	// 	QDateTime endDate = curDate;
	int year, month, day, hour, minute, second;

	//这里只用写入终止时间，因为要比较的当前时间是从网络获取
	year = endDate.date().year() ^ g_crypt;
	month = endDate.date().month() ^ g_crypt;
	day = endDate.date().day() ^ g_crypt;
	hour = endDate.time().hour() ^ g_crypt;
	minute = endDate.time().minute() ^ g_crypt;
	second = endDate.time().second() ^ g_crypt;
	fwrite(&year, sizeof(int), 1, pFile);
	fwrite(&hour, sizeof(int), 1, pFile);
	fwrite(&month, sizeof(int), 1, pFile);
	fwrite(&minute, sizeof(int), 1, pFile);
	fwrite(&day, sizeof(int), 1, pFile);
	fwrite(&second, sizeof(int), 1, pFile);

	//写入mac地址
	char cTmp[13] = { 0 };
	strcpy_s(cTmp, 13, strMac.toStdString().c_str());
	for (int i = 0; i < 12; ++i)
	{
		cTmp[i] ^= g_crypt;
	}
	fwrite(cTmp, 1, 13, pFile);

	//最后再动态写入随机长度的int值，保证每次生成的lisence长度不一样，我这种小软件这种程度应该够了
	int iRandCount = rand() % 100 + 50;
	for (int i = 0; i < iRandCount; ++i)
	{
		iRand = rand();
		fwrite(&iRand, sizeof(int), 1, pFile);
	}

	fclose(pFile);
	AddTipInfo("保存文件成功");
}

bool MainWindow::OnBtnLisenceInfo()
{
	std::string strFilePath = DEFAULT_PATH;
	strFilePath.append("Lisence.nn");

	//按照二进制读取
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		AddTipInfo(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str());
		return false;
	}

	//先取出20个int
	int iTmp = 0;
	for (int i = 0; i < 20; ++i)
	{
		fread(&iTmp, sizeof(int), 1, pFile);
	}

	//取出日期
	int year, month, day, hour, minute, second;

	fread(&year, sizeof(int), 1, pFile);
	fread(&hour, sizeof(int), 1, pFile);
	fread(&month, sizeof(int), 1, pFile);
	fread(&minute, sizeof(int), 1, pFile);
	fread(&day, sizeof(int), 1, pFile);
	fread(&second, sizeof(int), 1, pFile);
	year ^= g_crypt;
	hour ^= g_crypt;
	month ^= g_crypt;
	minute ^= g_crypt;
	day ^= g_crypt;
	second ^= g_crypt;
	m_endDate = QDateTime(QDate(year, month, day), QTime(hour, minute, second));

	//取出mac地址
	char cTmp[13] = { 0 };
	fread(cTmp, 1, 13, pFile);
	for (int i = 0; i < 12; ++i)
	{
		cTmp[i] ^= g_crypt;
	}
	m_macLisence = cTmp;

#ifdef DEV_VER
	AddTipInfo(cTmp);
	AddTipInfo(m_endDate.toString().toStdString().c_str(), false);
#endif

	//最后的随机混乱值不用处理

	fclose(pFile);

	return true;
}

bool MainWindow::CheckLisence()
{
	auto bRes = OnBtnLisenceInfo();
	if (!bRes)
	{
		ShowMessageBox("许可证文件丢失或破损");
		return false;
	}

	//比较mac地址
	m_macClient = m_macClient.toLower();
	m_macLisence = m_macLisence.toLower();
	if (0 != m_macClient.compare(m_macLisence))
	{
		ShowMessageBox("许可证无效");
		return false;
	}

	//比较真实的网络时间
	m_curDate = QDateTime(QDate(m_year, m_month, m_day), QTime(m_hour, m_minute, m_second));
	m_lisenceLeftSecond = m_curDate.secsTo(m_endDate);

	if (m_lisenceLeftSecond > 0)
	{
		auto leftDay = m_curDate.daysTo(m_endDate);
		if (leftDay > 1)
		{
			ShowMessageBox(std::string("许可期限还剩：").append(std::to_string(leftDay)).append("天").c_str());
		}
		else
		{
			ShowMessageBox(std::string("许可期限还剩：").append(std::to_string(m_lisenceLeftSecond / 3600)).append("小时:").append(std::to_string(m_lisenceLeftSecond / 60)).append("分").c_str());
		}

		m_lisenceCheckTimer.connect(&m_lisenceCheckTimer, &QTimer::timeout, [&]() {
			m_lisenceLeftSecond -= 10;
			if (m_lisenceLeftSecond <= 0)
			{
				ShowMessageBox("许可证已过期");
				m_lisenceCheckTimer.stop();
				destroy();
				::terminate();
			}
		});
		m_lisenceCheckTimer.setInterval(10000);
		m_lisenceCheckTimer.start();

		return true;
	}
	else
	{
		ShowMessageBox("许可证已过期，请联系管理员购买新的许可证");
		return false;
	}
}

QString MainWindow::GetMAC()
{
	QString strMac = m_ui->edt_mac->text();
	if (strMac.length() != 12)
	{
		ShowMessageBox("MAC地址无效");
		return "";
	}

	return strMac;
}

void MainWindow::OnBtnOpenFileDialog()
{
	auto res = QFileDialog::getOpenFileName(this, "", DEFAULT_PATH);
	if (res.compare("") == 0)
	{
		return;
	}

	// 	ui->edt_saveName->setText(res);
	LoadScriptModuleFile(res.toLocal8Bit().toStdString().c_str());
}

void MainWindow::OnBtnOpenFileDialog_PicPath()
{
	auto res = QFileDialog::getOpenFileName(this, "", DEFAULT_PATH);
	m_ui->edt_picPath->setText(res);
}

void MainWindow::OnBtnSaveClick()
{
	m_wndWidth = m_ui->edt_wndWidth->text().toInt();
	m_wndHeight = m_ui->edt_wndHeight->text().toInt();

	//按照二进制存储
	FILE *pFile = nullptr;
	std::string strFilePath = m_ui->edt_saveName->text().toLocal8Bit().toStdString();
	auto pos = strFilePath.find("/");
	if (std::string::npos == pos)
	{
		strFilePath = DEFAULT_PATH + strFilePath;
	}

	QFileInfo file(strFilePath.c_str());
	if (file.exists() && file.isFile())
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("文件已存在，是否覆盖?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (QMessageBox::Cancel == ret)
		{
			AddTipInfo("已取消保存");
			return;
		}
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	//先写入两个窗口名(长度+str)
	int nameLen = (int)m_ui->edt_wndName->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(m_ui->edt_wndName->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

	nameLen = (int)m_ui->edt_wndName2->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(m_ui->edt_wndName2->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

	//然后存入操作数组的大小以及数据
	int size = (int)m_inputVec.size();
	fwrite(&size, sizeof(int), 1, pFile);
	for (auto &input : m_inputVec)
	{
		fwrite(&input, sizeof(InputData), 1, pFile);
	}

	//最后写入窗口大小，因为这是后加的结构，为了不影响以前的脚本数据
	fwrite(&m_wndWidth, sizeof(int), 1, pFile);
	fwrite(&m_wndHeight, sizeof(int), 1, pFile);

	fclose(pFile);
	AddTipInfo("保存文件成功");
}

void MainWindow::OnBtnLoadClick()
{
	std::string strFilePath = m_ui->edt_saveName->text().toLocal8Bit().toStdString();
	auto pos = strFilePath.find("/");
	if (std::string::npos == pos)
	{
		strFilePath = DEFAULT_PATH + strFilePath;
	}

	LoadScriptModuleFile(strFilePath.c_str());
}

void MainWindow::LoadScriptModuleFile(const char *file)
{
	std::string strFilePath = file;
	m_inputVec.clear();

	//按照二进制读取
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		AddTipInfo(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str());
		return;
	}

	//先读入两个窗口名(长度+str)
	int nameLen = 0;
	fread(&nameLen, sizeof(int), 1, pFile);
	char *pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);
#ifdef DEV_VER
	m_ui->edt_wndName->setText(QString::fromLocal8Bit(pStr));
#endif
	delete[]pStr;
	pStr = nullptr;

	fread(&nameLen, sizeof(int), 1, pFile);
	pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);
#ifdef DEV_VER
	m_ui->edt_wndName2->setText(QString::fromLocal8Bit(pStr));
#endif
	delete[]pStr;
	pStr = nullptr;

	//然后读取操作数组的大小以及数据
	int size = 0;
	fread(&size, sizeof(int), 1, pFile);
	for (int i = 0; i < size; ++i)
	{
		InputData input;
		fread(&input, sizeof(InputData), 1, pFile);
		m_inputVec.push_back(input);
	}

	//最后读取窗口大小，因为是后添加的结构
	fread(&m_wndWidth, sizeof(int), 1, pFile);
	fread(&m_wndHeight, sizeof(int), 1, pFile);
#ifdef DEV_VER
	m_ui->edt_wndWidth->setText(std::to_string(m_wndWidth).c_str());
	m_ui->edt_wndHeight->setText(std::to_string(m_wndHeight).c_str());
#endif

	fclose(pFile);

	//更新保存文件显示的名称，否则容易保存覆盖错误，因为现在有模块跳转功能
	auto findPos = strFilePath.find_last_of("/");
	if (std::string::npos != findPos)
	{
		strFilePath = strFilePath.substr(findPos + 1);
	}

#ifdef DEV_VER
	m_ui->edt_saveName->setText(strFilePath.c_str());
	AddTipInfo(std::string("读取模块[").append(strFilePath).append("]成功，共读取命令").append(std::to_string(size)).append("条").c_str());
#endif

	//加载完后重新初始化窗口，因为窗口可能已经变动
	InitGameWindow();
	//因为保存的时候可能保存了改动的初始flag，所以加载模块时，把所有标记都重置一下，以确保正常使用
	ResetAllInputFinishFlag();
#ifdef DEV_VER
	RefreshInputVecUIList();
#endif

	SetInputDataModel();
}

void MainWindow::LoadModuleFile(const char *file)
{
	std::string strFilePath = file;
	m_inputModuleVec.clear();

	//按照二进制读取
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		AddTipInfo(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str());
		return;
	}

	//然后读取操作数组的大小以及数据
	int size = 0;
	fread(&size, sizeof(int), 1, pFile);
	for (int i = 0; i < size; ++i)
	{
		InputData input;
		fread(&input, sizeof(InputData), 1, pFile);
		m_inputModuleVec.push_back(input);
	}

	fclose(pFile);

	//更新保存文件显示的名称，否则容易保存覆盖错误，因为现在有模块跳转功能
	auto findPos = strFilePath.find_last_of("/");
	if (std::string::npos != findPos)
	{
		strFilePath = strFilePath.substr(findPos + 1);
	}

#ifdef DEV_VER
	m_ui->edt_saveName->setText(strFilePath.c_str());
	AddTipInfo(std::string("读取模块[").append(strFilePath).append("]成功，共读取命令").append(std::to_string(size)).append("条").c_str());
#endif

#ifdef DEV_VER
	RefreshInputModuleVecUIList();
#endif
}

void MainWindow::SetInputDataModel()
{
	auto row = m_inputVec.size();
	m_inputDataModel.clear();
	m_inputDataModel.setRowCount(row);
	m_inputDataModel.setColumnCount(26);
	m_inputDataModel.setHorizontalHeaderLabels(QStringList({
		Q8("注释"), Q8("类型"), Q8("操作"), Q8("vk"), "delay", 
		"x", "y", "xRate", "yRate",	"x2", "y2", "xRate2", "yRate2",
		Q8("开始时间"), Q8("完成标记"), Q8("初始开始时间标记"),
		Q8("图片对比标记"), "cmpPicRate", "picPath", Q8("查找图片超时"), Q8( "比图超时标记" ),
		Q8("比图成功跳转"), Q8("比图超时跳转"), Q8("比图成功跳转模块"), Q8("比图超时跳转模块"), Q8("比图点击"),
		}));

	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < 26; ++j)
		{
			QModelIndex index = m_inputDataModel.index(i, j, QModelIndex());

			switch (j)
			{
			case 0:
				m_inputDataModel.setData( index, Q8( m_inputVec[i].comment ) );
				break;
			case 1:
				m_inputDataModel.setData( index, InputTypeStrMap[( m_inputVec[i].type )] );
				break;
			case 2:
				m_inputDataModel.setData( index, OpTypeStrMap[( m_inputVec[i].opType )] );
				break;
			case 3:
				m_inputDataModel.setData( index, std::string( 1, ( m_inputVec[i].vk ) ).c_str() );
				break;
			case 4:
				m_inputDataModel.setData( index, m_inputVec[i].delay );
				break;
			case 5:
				m_inputDataModel.setData( index, m_inputVec[i].x );
				break;
			case 6:
				m_inputDataModel.setData( index, m_inputVec[i].y );
				break;
			case 7:
				m_inputDataModel.setData( index, m_inputVec[i].xRate );
				break;
			case 8:
				m_inputDataModel.setData( index, m_inputVec[i].yRate );
				break;
			case 9:
				m_inputDataModel.setData( index, m_inputVec[i].x2 );
				break;
			case 10:
				m_inputDataModel.setData( index, m_inputVec[i].y2 );
				break;
			case 11:
				m_inputDataModel.setData( index, m_inputVec[i].xRate2 );
				break;
			case 12:
				m_inputDataModel.setData( index, m_inputVec[i].yRate2 );
				break;
			case 13:
				m_inputDataModel.setData( index, (UINT)m_inputVec[i].startTime );
				break;
			case 14:
				m_inputDataModel.setData( index, m_inputVec[i].bFinishFlag );
				break;
			case 15:
				m_inputDataModel.setData( index, m_inputVec[i].bInitStartTimeFlag );
				break;
			case 16:
				m_inputDataModel.setData( index, m_inputVec[i].bFindPicFlag );
				break;
			case 17:
				m_inputDataModel.setData( index, m_inputVec[i].cmpPicRate );
				break;
			case 18:
				m_inputDataModel.setData( index, m_inputVec[i].picPath );
				break;
			case 19:
				m_inputDataModel.setData( index, m_inputVec[i].findPicOvertime );
				break;
			case 20:
				m_inputDataModel.setData( index, m_inputVec[i].bFindPicOvertimeFlag );
				break;
			case 21:
				m_inputDataModel.setData( index, m_inputVec[i].findPicSucceedJumpIndex );
				break;
			case 22:
				m_inputDataModel.setData( index, m_inputVec[i].findPicOvertimeJumpIndex );
				break;
			case 23:
				m_inputDataModel.setData( index, m_inputVec[i].findPicSucceedJumpModule );
				break;
			case 24:
				m_inputDataModel.setData( index, m_inputVec[i].findPicOvertimeJumpModule );
				break;
			case 25:
				m_inputDataModel.setData( index, m_inputVec[i].bCmpPicCheckFlag );
				break;
			default:
			{
				m_inputDataModel.setData(index, "default data");
			}
				break;
			}
		}
	}

	m_ui->tv_inputVec->resizeColumnsToContents();
}

void MainWindow::GetInputDataModel()
{
	auto row = m_inputDataModel.rowCount();
	auto col = m_inputDataModel.columnCount();

	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			QModelIndex index = m_inputDataModel.index( i, j, QModelIndex() );

			switch ( j )
			{
			case 0:
				strcpy_s( m_inputVec[i].comment, MAX_PATH, m_inputDataModel.data(index).toString().toLocal8Bit().toStdString().c_str());
				break;
			case 1:
				m_inputVec[i].type = StrInputTypeMap[m_inputDataModel.data(index).toString()];
				break;
			case 2:
				m_inputVec[i].opType = StrOpTypeMap[m_inputDataModel.data(index).toString()];
				break;
			case 3:
				m_inputVec[i].vk = m_inputDataModel.data(index).toString().toStdString().at(0);
				break;
			case 4:
				m_inputVec[i].delay = m_inputDataModel.data(index).toString().toShort();
				break;
			case 5:
				m_inputVec[i].x = m_inputDataModel.data(index).toString().toInt();
				break;
			case 6:
				m_inputVec[i].y = m_inputDataModel.data(index).toString().toInt();
				break;
			case 7:
				m_inputVec[i].xRate = m_inputDataModel.data(index).toString().toFloat();
				break;
			case 8:
				m_inputVec[i].yRate = m_inputDataModel.data( index ).toString().toFloat();
				break;
			case 9:
				m_inputVec[i].x2 = m_inputDataModel.data( index ).toString().toInt();
				break;
			case 10:
				m_inputVec[i].y2 = m_inputDataModel.data( index ).toString().toInt();
				break;
			case 11:
				m_inputVec[i].xRate2 = m_inputDataModel.data( index ).toString().toFloat();
				break;
			case 12:
				m_inputVec[i].yRate2 = m_inputDataModel.data( index ).toString().toFloat();
				break;
			case 13:
				m_inputVec[i].startTime = m_inputDataModel.data(index).toString().toUInt();
				break;
			case 14:
				m_inputVec[i].bFinishFlag = m_inputDataModel.data(index).toBool();
				break;
			case 15:
				m_inputVec[i].bInitStartTimeFlag = m_inputDataModel.data( index ).toBool();
				break;
			case 16:
				m_inputVec[i].bFindPicFlag = m_inputDataModel.data( index ).toBool();
				break;
			case 17:
				m_inputVec[i].cmpPicRate = m_inputDataModel.data( index ).toString().toFloat();
				break;
			case 18:
				strcpy_s( m_inputVec[i].picPath, MAX_PATH, m_inputDataModel.data( index ).toString().toLocal8Bit().toStdString().c_str() );
				break;
			case 19:
				m_inputVec[i].findPicOvertime = m_inputDataModel.data( index ).toString().toInt();
				break;
			case 20:
				m_inputVec[i].bFindPicOvertimeFlag = m_inputDataModel.data( index ).toBool();
				break;
			case 21:
				m_inputVec[i].findPicSucceedJumpIndex = m_inputDataModel.data( index ).toString().toInt();
				break;
			case 22:
				m_inputVec[i].findPicOvertimeJumpIndex = m_inputDataModel.data( index ).toString().toInt();
				break;
			case 23:
				strcpy_s( m_inputVec[i].findPicSucceedJumpModule, MAX_PATH, m_inputDataModel.data( index ).toString().toLocal8Bit().toStdString().c_str() );
				break;
			case 24:
				strcpy_s( m_inputVec[i].findPicOvertimeJumpModule, MAX_PATH, m_inputDataModel.data( index ).toString().toLocal8Bit().toStdString().c_str() );
				break;
			case 25:
				m_inputVec[i].bCmpPicCheckFlag = m_inputDataModel.data( index ).toBool();
				break;
			default:
			{
				ShowMessageBox( "出现致命model读取错误" );
			}
			break;
			}
		}
	}
}

void MainWindow::ResetAllInputFinishFlag()
{
	for (auto &input : m_inputVec)
	{
		input.bFinishFlag = false;
		input.bInitStartTimeFlag = false;
		input.bFindPicFlag = false;
		input.bFindPicOvertimeFlag = false;
	}
}

void MainWindow::JumpInput(int index)
{
	int size = (int)m_inputVec.size();
	for (int i = 0; i < size; ++i)
	{
		if (i < index)
		{
			m_inputVec[i].bFinishFlag = true;
			m_inputVec[i].bFindPicFlag = true;
			m_inputVec[i].bInitStartTimeFlag = true;
		}
		else
		{
			m_inputVec[i].bFinishFlag = false;
			m_inputVec[i].bFindPicFlag = false;
			m_inputVec[i].bInitStartTimeFlag = false;
			m_inputVec[i].bFindPicOvertimeFlag = false;
		}
	}
}

void MainWindow::HandleMouseInput(InputData &input)
{
	switch (input.opType)
	{
	case Click:
	{
		PostMessage(m_hGameWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
		PostMessage( m_hGameWnd, WM_LBUTTONUP, 0, MAKELONG( m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate ) );
	}
	break;
	case Press:
	{
		PostMessage(m_hGameWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
	break;
	case Release:
	{
		PostMessage(m_hGameWnd, WM_LBUTTONUP, 0, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
	break;
	case Move:
	{
		//move的时候默认鼠标左键按下
		PostMessage(m_hGameWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
	break;
	default:
		AddTipInfo("错误：未处理的鼠标操作...");
		break;
	}
}

void MainWindow::HandleKeyboardInput(InputData &input)
{
	switch (input.opType)
	{
	case Click:
	{
		PostMessage(m_hGameWnd, WM_KEYDOWN, input.vk, 0);
		PostMessage(m_hGameWnd, WM_KEYUP, input.vk, 0);
	}
	break;
	case Press:
	case Move:
	default:
		AddTipInfo("错误：未处理的键盘操作...");
		break;
	}
}

void MainWindow::HandleGameImgCompare(InputData &input)
{
	m_picCompareStrategy->HandlePicCompare(input, m_hGameWnd, m_gameWndSize);
}

void MainWindow::InitGameWindow()
{
	ResetSimWndInfo();
	m_hGameWnd = nullptr;
	m_hParentWnd = nullptr;

	if (None == m_simWndType)
	{
		m_simWndType = Thunder;
	}

	auto it = m_simWndInfoMap.find(m_simWndType);
	if (it == m_simWndInfoMap.end())
	{
		return;
	}

	//首先查找指定类型
	it->second.layerWnd[it->second.curLayer] = FindWindowA(nullptr, it->second.layerWndName[it->second.curLayer].toLocal8Bit().toStdString().c_str());
	//没有找到指定类型
	if (nullptr == it->second.layerWnd[it->second.curLayer])
	{
		AddTipInfo(std::string("查找指定模拟器[").append(it->second.layerWndName[it->second.curLayer].toLocal8Bit().toStdString()).append("]失败，开始自动查找其他模拟器").c_str());

		for (auto &simWndInfo : m_simWndInfoMap)
		{
			simWndInfo.second.layerWnd[simWndInfo.second.curLayer] = FindWindowA(nullptr, simWndInfo.second.layerWndName[simWndInfo.second.curLayer].toLocal8Bit().toStdString().c_str());

			if (nullptr != simWndInfo.second.layerWnd[simWndInfo.second.curLayer])
			{
				EnumChildWindows(simWndInfo.second.layerWnd[simWndInfo.second.curLayer], &MainWindow::EnumChildProc, (LPARAM)(&simWndInfo.second));

				if (nullptr != simWndInfo.second.gameWnd)
				{
					m_hGameWnd = simWndInfo.second.gameWnd;
					m_hParentWnd = simWndInfo.second.parentWnd;
					m_curSimWndInfo = simWndInfo.second;
					//根据之前的差值比例设定父窗口的大小，有些模拟器比如雷电可以直接设置子窗口，父窗口会放大，但是mumu只设置子窗口，父窗口是不跟随变化的
					//设置游戏本体窗口到指定大小，这样图片识别应该会比较一致，之前设置的是外层窗口，就会导致游戏本体窗口大小有差别
					simWndInfo.second.SetGameWndSize(m_wndWidth, m_wndHeight);

					AddTipInfo(std::string("找到模拟器[").append(simWndInfo.second.layerWndName[0].toLocal8Bit().toStdString()).append("]").c_str());
					break;
				}
			}
		}
	}
	//找到指定类型
	else
	{
// 		RECT rt;
// 		GetWindowRect(m_hWnd, &rt);
// 
// 		//这里的大小设置不要再改动了，如果只是鼠标点击倒是没有关系，主要涉及到图片对比，虽然比例一样，但是图片太小了拉伸以后始终会失真，因为原对比图片的大小是从890 588的分辨率上截取的
// 		if ((rt.right - rt.left) != m_wndHeight || (rt.bottom - rt.top) != m_wndHeight)
// 		{
// 			::SetWindowPos(m_hWnd, HWND_BOTTOM, rt.left, rt.top, m_wndWidth, m_wndHeight, SWP_NOMOVE | SWP_NOACTIVATE);
// 		}

		EnumChildWindows(it->second.layerWnd[it->second.curLayer], &MainWindow::EnumChildProc, (LPARAM)(&it->second));

		if (nullptr != it->second.gameWnd)
		{
			m_hGameWnd = it->second.gameWnd;
			m_hParentWnd = it->second.parentWnd;
			m_curSimWndInfo = it->second;
			//根据之前的差值比例设定父窗口的大小，有些模拟器比如雷电可以直接设置子窗口，父窗口会放大，但是mumu只设置子窗口，父窗口是不跟随变化的
			//设置游戏本体窗口到指定大小，这样图片识别应该会比较一致，之前设置的是外层窗口，就会导致游戏本体窗口大小有差别
			it->second.SetGameWndSize(m_wndWidth, m_wndHeight);

			AddTipInfo(std::string("找到模拟器[").append(it->second.layerWndName[0].toLocal8Bit().toStdString()).append("]").c_str());
		}
	}

	if (nullptr != m_hGameWnd)
	{
		UpdateGameWindowSize();
	}
}

void MainWindow::UpdateGameWindowSize()
{
	RECT rect;
	GetWindowRect(m_hGameWnd, &rect);
	m_gameWndSize.x = rect.right - rect.left;
	m_gameWndSize.y = rect.bottom - rect.top;
}

void MainWindow::ResetSimWndInfo()
{
	for (auto &info : m_simWndInfoMap)
	{
		info.second.curLayer = 0;
		info.second.gameWnd = nullptr;
	}
}

void MainWindow::SetSimWndType(SimWndType type)
{
	m_simWndType = type;
}

void SimWndInfo::SetGameWndSize(int width, int height)
{
	if (nullptr == gameWnd)
	{
		return;
	}

	UpdateRect();
	
	//n层窗口可以算出n-1层的比例，以主窗口为参考（比例1），算出其他相应的比例，就可以设置大小了
	double rateW[MAX_LAYER - 1] = { 0 };
	double rateH[MAX_LAYER - 1] = { 0 };
	rateW[0] = 1.0;
	rateH[0] = 1.0;

	int w = 0, h = 0, w2 = 0, h2 = 0;
	GetLayerSize(0, w, h);//获取到主窗口的大小

	for (int i = 1; i < totalFindLayer; ++i)
	{
		GetLayerSize(i, w2, h2);//获取子窗口大小，并算出和主窗口的比例，最后就可以一次性按比例设置所有窗口
		rateW[i] = (double)w / (double)w2;
		rateH[i] = (double)h / (double)h2;
	}

	//从最外层开始，逐层设置大小
	for (int i = 0; i < totalFindLayer; ++i)
	{
		::SetWindowPos(layerWnd[i], HWND_BOTTOM, 0, 0, (width * rateW[totalFindLayer - 1 - i]), (height * rateH[totalFindLayer - 1 - i]), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		InvalidateRect(layerWnd[i], nullptr, TRUE);
		Sleep(200);
	}
}
