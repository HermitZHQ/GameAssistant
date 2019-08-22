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
#include "CaptureInputDataMgr.h"


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
	{InputType::Log, Q8("日志")},
	{InputType::Jump, Q8("跳转")},
	{InputType::Wait, Q8("等待")},
};
std::unordered_map<std::string, InputType> StrInputTypeMap = {
	{ ( "鼠标" ), InputType::Mouse},
	{ ( "键盘" ), InputType::Keyboard},
	{ ( "图片" ), InputType::Pic},
	{ ("停止"), InputType::StopScript},
	{ ("日志"), InputType::Log},
	{ ("跳转"), InputType::Jump},
	{ ("等待"), InputType::Wait},
};

std::unordered_map<OpType, QString> OpTypeStrMap = {
	{OpType::Click, Q8("点击")},
	{OpType::Press, Q8("按住")},
	{OpType::Move, Q8("移动")},
	{OpType::Release, Q8("释放")},
};
std::unordered_map<std::string, OpType> StrOpTypeMap = {
	{ ( "点击" ), OpType::Click },
	{ ( "按住" ), OpType::Press },
	{ ( "移动" ), OpType::Move },
	{ ( "释放" ), OpType::Release },
};

QStringList g_horizontalHeaderLables({
		Q8("注释"), Q8("类型"), Q8("操作"), Q8("vk"), "delay",
		"x", "y", "xRate", "yRate",	"x2", "y2", "xRate2", "yRate2",
		Q8("开始时间"), Q8("完成标记"), Q8("初始开始时间标记"),
		Q8("图片对比标记"), "cmpPicRate", "picPath", Q8("查找图片超时"), Q8("比图超时标记"),
		Q8("比图成功跳转"), Q8("比图超时跳转"), Q8("比图成功跳转模块"), Q8("比图超时跳转模块"), Q8("比图点击"),
		Q8("重复"), Q8("对比参数"), Q8("输出参数"), Q8("等待(秒)")
	});

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
	, m_stopFlag(false)
	, m_wndWidth(960)
	, m_wndHeight(540)
	, m_hGameWnd(nullptr)
	, m_picCompareStrategy(new ZZPicCompareStrategy(this))
	, m_playerUI(this)
	, m_year(0)
	, m_lisenceLeftSecond(0)
	, m_bShowHideFlag(true)
	, m_simWndType(None)
	, m_itemDelegate(this)
	, m_simWndInfoMap({
		{Thunder, SimWndInfo(QString::fromLocal8Bit("雷电模拟器"), {true, true, false}, 2, 2)},
		{MuMu, SimWndInfo(QString::fromLocal8Bit("重装战姬 - MuMu模拟器"), {false, true, true}, 3, 2)},
		})
	, m_curSimWndInfo(QString::fromLocal8Bit("雷电模拟器"), { true, true, false }, 2, 2)
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

	m_logTimer.connect( &m_logTimer, &QTimer::timeout, this, &MainWindow::LogTimerFunc );
	m_logTimer.start( 20 );
	m_msgBoxTimer.connect( &m_msgBoxTimer, &QTimer::timeout, this, &MainWindow::MessageBoxTimerFunc );
	m_msgBoxTimer.start( 500 );

#ifdef DEV_VER
	m_ui->setupUi(this);
	m_ui->edt_mac->setText(m_macClient);
	CheckLisence();
	setParent(&m_bkgUI);
	m_picCompareStrategy->SetUi(m_ui);
	m_bkgUI.setGeometry(geometry());
	setWindowTitle("Develop-Ver 1.0.8");

	InitTableView();
	CaptureInputDataMgr::Singleton(this);
	CaptureInputDataMgr::Singleton().BeginCapture();
	connect(&m_captureUpdateTimer, &QTimer::timeout, this, &MainWindow::CaptureUpdate);
	m_captureUpdateTimer.start(10);
#else
	m_ui->setupUi(this);
	ShowMessageBox(std::string("本机mac:").append(m_macClient.toStdString()).c_str());
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

	m_checkGameWndSizeTimer.connect(&m_checkGameWndSizeTimer, &QTimer::timeout, this, &MainWindow::CheckGameWndSize);

	m_bkgUI.show();
}

MainWindow::~MainWindow()
{
	CoUninitialize();
	setParent(nullptr);
	m_playerUI.setParent(nullptr);
	delete m_ui;
}

void MainWindow::PostMsgThread(int cmpParam)
{
	cmpParam = -1;
	//---------------------------------下面的代码应该只在DevVer执行
#ifdef DEV_VER
	if (m_inputVec.size() == 0)
	{
		m_timer.stop();
		AddTipInfo("脚本中命令数为0，线程退出...");
		return;
	}

	//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	UpdateGameWindowSize();

	bool bAllFinishedFlag = true;
	int index = -1;
	for (auto &input : m_inputVec)
	{
		++index;
		//判断操作已经完成或者和对比参数不匹配
		if ((input.bFinishFlag && input.type != Pic) 
			|| (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag))
			|| (-1 != cmpParam && input.cmpParam != cmpParam))
			continue;

#ifdef DEV_VER
		m_ui->edt_cmpPic->setText(QString::fromLocal8Bit("[").toStdString().append(std::to_string(index)).append("]").c_str());

		// 		AddTipInfo(std::string("handle index:").append(std::to_string(index)).c_str());
#endif

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
		case Log:
		{
			AddTipInfo(input.comment);
		}
		break;
		case Jump:
		{
			if (0xffff == input.findPicSucceedJumpIndex)
			{
				LoadScriptModuleFile(input.findPicSucceedJumpModule);
				break;
			}
			else
			{
				if (-1 != input.findPicSucceedJumpIndex)
				{
					JumpInput(input.findPicSucceedJumpIndex, m_inputVec);
				}
			}
		}
		break;
		default:
			break;
		}

		//查询图片是否超时---放在执行之后，这样保证至少先执行一次对比才会超时
		if (input.findPicOvertime != -1 && InputType::Pic == input.type && (GetTickCount() - input.startTime > (DWORD)(input.findPicOvertime + input.delay)))
		{
			input.bFindPicOvertimeFlag = true;
			//判断超时指令跳转
			if (-1 != input.findPicOvertimeJumpIndex)
			{
				(0xffff != input.findPicOvertimeJumpIndex) ? JumpInput(input.findPicOvertimeJumpIndex, m_inputVec) :
					(LoadScriptModuleFile(input.findPicOvertimeJumpModule));
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
			(0xffff != input.findPicSucceedJumpIndex) ? JumpInput(input.findPicSucceedJumpIndex, m_inputVec) :
				LoadScriptModuleFile(input.findPicSucceedJumpModule);
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
#ifdef DEV_VER
			AddTipInfo(std::string("index:").append(std::to_string(index)).append("_").append(std::to_string(input.alreadyRepeatCount)).c_str());
#endif

			//没有达到重复次数时，重置开始时间，并判断是否跳转后，break继续判断延迟
			input.startTime = GetTickCount();
			//这里借用了找图成功跳转索引的变量，因为找图我们都不会使用多次，我也就不想再多加结构体变量了，目前的多次重复命令都用于延迟上
			if (-1 != input.findPicSucceedJumpIndex && 0xffff != input.findPicSucceedJumpIndex)
			{
				JumpInput(input.findPicSucceedJumpIndex, m_inputVec);
			}
			break;
		}
	}

	if (bAllFinishedFlag)
	{
		ResetAllInputFinishFlag(m_inputVec);
	}

#endif // DEV_VER
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

void MainWindow::InsertComparePicOperation(int x, int y, int x2, int y2)
{
	m_ui->edt_x->setText(QString::number(x));
	m_ui->edt_y->setText(QString::number(y));
	m_ui->edt_x2->setText(QString::number(x2));
	m_ui->edt_y2->setText(QString::number(y2));

	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection() && model->selectedIndexes().size() == 1)
	{
		m_ui->cb_inputType->setCurrentIndex(InputType::Pic);
		m_ui->edt_comment->setText(Q8("对比图片"));
		m_ui->edt_delay->setText(QString::number(10));
		m_ui->edt_cmpPic->setText(QString::number(0.77));
		m_ui->edt_findPicOvertime->setText(QString::number(10));

		int index = model->selectedIndexes()[0].row();
		InsertInputData(index);
		SetTableViewIndex(index);
	}
	else
	{
		ShowMessageBox("当前选择的index无法进行插入图片对比操作");
		return;
	}
}

void MainWindow::InsertClickOperation(int x, int y)
{
	m_ui->edt_x->setText(QString::number(x));
	m_ui->edt_y->setText(QString::number(y));

	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection() && model->selectedIndexes().size() == 1)
	{
		m_ui->cb_inputType->setCurrentIndex(InputType::Mouse);
		m_ui->cb_opType->setCurrentIndex(OpType::Click);
		m_ui->edt_comment->setText(Q8("点击"));
		m_ui->edt_delay->setText(QString::number(500));

		int index = model->selectedIndexes()[0].row();
		InsertInputData(index);
	}
	else
	{
		AddTipInfo("当前选择的index无法进行插入点击操作");
		return;
	}
}

void MainWindow::InsertDragOperation(int x, int y, int x2, int y2)
{
	m_ui->edt_x->setText(QString::number(x));
	m_ui->edt_y->setText(QString::number(y));

	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection() && model->selectedIndexes().size() == 1)
	{
		m_ui->cb_inputType->setCurrentIndex(InputType::Mouse);
		m_ui->edt_delay->setText(QString::number(250));
		m_ui->cb_opType->setCurrentIndex(OpType::Press);
		m_ui->edt_comment->setText(Q8("拖动-1"));

		int index = model->selectedIndexes()[0].row();
		InsertInputData(index);

		m_ui->edt_x->setText(QString::number(x2));
		m_ui->edt_y->setText(QString::number(y2));
		m_ui->cb_opType->setCurrentIndex(OpType::Move);
		m_ui->edt_comment->setText(Q8("拖动-2"));
		InsertInputData(index + 1);

		m_ui->cb_opType->setCurrentIndex(OpType::Release);
		m_ui->edt_comment->setText(Q8("拖动-3"));
		InsertInputData(index + 2);
	}
	else
	{
		AddTipInfo("当前选择的index无法进行插入点击操作");
		return;
	}
}

int MainWindow::GetTableViewIndex()
{
	return m_ui->tv_inputVec->currentIndex().row();
}

void MainWindow::SetTableViewIndex(int index)
{
	auto curIndex = m_inputDataModel.index(index, 0);
	m_ui->tv_inputVec->setCurrentIndex(curIndex);
	m_ui->tv_inputVec->scrollTo(curIndex);
}

void MainWindow::CaptureUpdate()
{
	CaptureInputDataMgr::Singleton().CaptureThreadUpdate();
}

void MainWindow::OnBtnStartClick()
{
	m_stopFlag = false;
	if (nullptr == m_hGameWnd)
	{
		InitGameWindow();
		if (nullptr == m_hGameWnd)
		{
			AddTipInfo("game wnd null, start failed");
			return;
		}
	}
	ResetAllInputFinishFlag(m_inputVec);

	// 	std::thread t(&MainWindow::PostMsgThread, this);
	// 	t.detach();

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(PostMsgThread()));
	m_timer.setInterval(1);
	// 	m_timer.setSingleShot(true);
	m_timer.start();
	AddTipInfo("开始脚本处理......");
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	AddTipInfo("脚本处理已停止......");
}

void MainWindow::OnBtnAddInput()
{
	//添加前先更新游戏窗口大小
	UpdateGameWindowSize();

	int repeatTime = 1;

	InputData input;
	GetInputData(input);

	for (int i = 0; i < repeatTime; ++i)
	{
		m_inputVec.push_back(input);
	}

	AddTipInfo(std::string("已添加").append(std::to_string(repeatTime)).append("条指令").c_str());

	SetInputDataModel();
}

void MainWindow::GetInputData(InputData &input)
{
	//----comment
	strcpy_s(input.comment, PATH_LEN, m_ui->edt_comment->text().toLocal8Bit().toStdString().c_str());
	//----keyboard
	input.type = (InputType)m_ui->cb_inputType->currentIndex();
	input.opType = (OpType)m_ui->cb_opType->currentIndex();
	input.delay = m_ui->edt_delay->text().toShort();
	input.waitTime = m_ui->edt_timeCount->text().toInt();
	input.vk = m_ui->edt_vk->text().toLocal8Bit()[0];
	input.repeatCount = m_ui->edt_repeat->text().toShort();
	input.cmpParam = m_ui->edt_cmpParam->text().toInt();
	input.outputParam = m_ui->edt_outputParam->text().toInt();
	//change comment to fix string
	if (input.type == Jump)
	{
		strcpy_s(input.comment, PATH_LEN, "[跳转]");
	}
	else if (input.type == Log)
	{
		strcpy_s(input.comment, PATH_LEN, "[日志]");
	}
	else if (input.type == StopScript)
	{
		strcpy_s(input.comment, PATH_LEN, "[停止]");
	}
	else if (input.type == Wait)
	{
		strcpy_s(input.comment, PATH_LEN, "[等待]");
	}

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

void MainWindow::OnBtnDelAllInput()
{
	if (!ShowConfirmBox("确认要删除所有命令吗？"))
	{
		return;
	}

	m_inputVec.clear();
	AddTipInfo("已删除所有指令");

	SetInputDataModel();
}

void MainWindow::OnBtnUpdateAllInput()
{
	int index = GetTableViewIndex();
	UpdateGameWindowSize();

	auto it = m_inputVec.begin();
	for (; it != m_inputVec.end(); ++it)
	{
		UpdateInputData(*it);
	}

	SetInputDataModel();
	SetTableViewIndex(index);
	AddTipInfo("已更新所有指令");
}

void MainWindow::OnTableViewClick()
{
	auto index = m_ui->tv_inputVec->currentIndex();
	m_ui->edt_insertIndex->setText(QString::number(index.row()));
	UpdateInputDataUI( index.row() );
}

void MainWindow::UpdateInputDataUI( int index )
{
	auto it = m_inputVec.begin();
	for ( int i = 0; i < m_inputVec.size(); ++i, ++it )
	{
		if ( i != index )
			continue;

		//把当前input的咨询显示到editbox中
		m_ui->cb_inputType->setCurrentIndex( it->type );
		m_ui->cb_opType->setCurrentIndex( it->opType );
		m_ui->edt_vk->setText( std::string( 1, it->vk ).c_str() );
		m_ui->edt_delay->setText(std::to_string(it->delay).c_str());
		m_ui->edt_timeCount->setText(std::to_string(it->waitTime).c_str());
		m_ui->edt_comment->setText( QString::fromLocal8Bit( it->comment ) );
		m_ui->edt_repeat->setText(std::to_string(it->repeatCount).c_str());
		m_ui->edt_cmpParam->setText(std::to_string(it->cmpParam).c_str());
		m_ui->edt_outputParam->setText(std::to_string(it->outputParam).c_str());

		m_ui->edt_x->setText( std::to_string( it->x ).c_str() );
		m_ui->edt_y->setText( std::to_string( it->y ).c_str() );
		m_ui->edt_x2->setText( std::to_string( it->x2 ).c_str() );
		m_ui->edt_y2->setText( std::to_string( it->y2 ).c_str() );

		m_ui->edt_rate->setText( std::to_string( it->cmpPicRate ).c_str() );
		m_ui->edt_picPath->setText( it->picPath );
		m_ui->edt_findPicOvertime->setText( std::to_string( it->findPicOvertime ).c_str() );
		m_ui->edt_succeedJump->setText( std::to_string( it->findPicSucceedJumpIndex ).c_str() );
		m_ui->edt_overtimeJump->setText( std::to_string( it->findPicOvertimeJumpIndex ).c_str() );
		m_ui->edt_overtimeJumpModule->setText( it->findPicOvertimeJumpModule );
		m_ui->edt_succeedJumpModule->setText( it->findPicSucceedJumpModule );
		m_ui->chk_cmpPicClick->setChecked( it->bCmpPicCheckFlag );

		break;
	}
}

void MainWindow::UpdateSelectInputData( int index )
{
	auto it = m_inputVec.begin();
	for ( int i = 0; i < m_inputVec.size(); ++i, ++it )
	{
		if ( i != index )
			continue;

		GetInputData( *it );

		break;
	}

	SetInputDataModel();
}

void MainWindow::OnBtnInsertInputClick()
{
	int index = m_ui->edt_insertIndex->text().toInt();
	InsertInputData(index);
	SetTableViewIndex(index);
}

void MainWindow::InsertInputData(int index)
{
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

	SetInputDataModel();
}

void MainWindow::OnBtnGetBattleTemplate()
{
	LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("module_battle_normal").c_str(), m_copyInputVec);

	AddTipInfo(std::string("已复制战斗模版:").append("module_battle_normal").c_str());
}

void MainWindow::OnBtnGetBattleTemplate2()
{
	LoadScriptModuleFileToSpecificInputVec(std::string(DEFAULT_PATH).append("module_battle_normal_scale").c_str(), m_copyInputVec);

	AddTipInfo(std::string("已复制战斗模版2:").append("module_battle_normal_scale").c_str());
}

void MainWindow::OnBtnStartTimeCount()
{
	m_waitTime = GetTickCount();
	m_timeCountTimer.connect(&m_timeCountTimer, &QTimer::timeout, [&]() {
		m_ui->edt_timeCount->setText(QString::number((GetTickCount() - m_waitTime) / 1000));
	});
	m_timeCountTimer.start(500);
}

void MainWindow::OnBtnEndTimeCount()
{
	m_timeCountTimer.stop();

	InputData input;
	input.type = Wait;
	strcpy_s(input.comment, MAX_PATH, "[等待]");
	int secondCount = m_ui->edt_timeCount->text().toInt();
	input.waitTime = secondCount;

	m_inputVec.push_back(input);
	SetInputDataModel();
}

void MainWindow::OnBtnClearTipInfo()
{
	m_ui->list_tip->clear();
}

void MainWindow::ShowMessageBox(const char *content)
{
	m_lockMsgbox.lock();
	m_msgBoxInfoList.push_back( Q8( content ) );
	if ( m_msgBoxInfoList.size() > 10 )
	{
		return;
	}
	m_lockMsgbox.unlock();
}

void MainWindow::MessageBoxTimerFunc()
{
	if ( m_msgBoxInfoList.size() == 0 )
	{
		return;
	}

	QMessageBox mb;
	mb.setWindowTitle( "Info" );

	m_lockMsgbox.lock();
	mb.setText(m_msgBoxInfoList[0]);
	m_msgBoxInfoList.erase(m_msgBoxInfoList.begin());
	m_lockMsgbox.unlock();

	mb.setDefaultButton( QMessageBox::Ok );
	mb.exec();

}

bool MainWindow::ShowConfirmBox(const char *str)
{
	QMessageBox mb;
	mb.setWindowTitle("Info");
	mb.setText(QString::fromLocal8Bit(str));
	mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	mb.setDefaultButton(QMessageBox::Cancel);
	if (QMessageBox::Ok == mb.exec())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MainWindow::AddTipInfo(const char *str, bool bConvertFlag)
{
	m_lockLog.lock();
	m_logList.push_back( QTime::currentTime().toString().append( ":" ).append( bConvertFlag ? Q8( str ) : str ) );
	if ( m_logList.size() > 500 )
	{
		m_logList.clear();
	}
	m_lockLog.unlock();
}

void MainWindow::LogTimerFunc()
{
	if ( m_logList.size() == 0 )
	{
		return;
	}

#ifdef DEV_VER
	if ( m_ui->list_tip->count() > 200 )
	{
		m_ui->list_tip->clear();
	}

	m_lockLog.lock();
	m_ui->list_tip->addItem(m_logList[0]);
	m_logList.erase(m_logList.begin());
	m_lockLog.unlock();

	m_ui->list_tip->scrollToBottom();
#else
	if ( m_playerUI.GetUI()->list_tip->count() > 200 )
	{
		m_playerUI.GetUI()->list_tip->clear();
	}

	m_lockLog.lock();
	m_playerUI.GetUI()->list_tip->addItem(m_logList[0]);
	m_logList.erase(m_logList.begin());
	m_lockLog.unlock();

	m_playerUI.GetUI()->list_tip->scrollToBottom();
#endif
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
		endDate = curDate.addSecs(3600);
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

void MainWindow::InitTableView()
{
	//set table view model
	m_ui->tv_inputVec->setModel(&m_inputDataModel);
	m_ui->tv_inputVec->setItemDelegate(&m_itemDelegate);

	auto actUpdateDelay = m_menu.addAction(Q8("更新延迟"));
	auto updateSingleRow = m_menu.addAction(Q8("更新单行"));
	auto actCopy = m_menu.addAction(Q8("复制内容"));
	auto actPaste = m_menu.addAction(Q8("粘贴内容"));
	auto actCopyInput = m_menu.addAction(Q8("复制行"));
	auto actInsertCopyInput = m_menu.addAction( Q8( "插入复制行（上）" ) );
	auto actInsertCopyInputDown = m_menu.addAction( Q8( "插入复制行（下）" ) );
	auto actPasteOverwriteInput = m_menu.addAction(Q8("粘贴覆盖行"));
	auto actDel = m_menu.addAction(Q8("删除"));
	auto actDelAll = m_menu.addAction(Q8("删除所有"));
	auto actJump = m_menu.addAction(Q8("跳转命令"));

	m_ui->tv_inputVec->setContextMenuPolicy(Qt::CustomContextMenu);
	//table view connect
	m_ui->tv_inputVec->connect(m_ui->tv_inputVec, &QTableView::customContextMenuRequested, [&](const QPoint &pt) {
		auto pos = m_ui->tv_inputVec->mapToGlobal(pt);
		m_menu.move(pos);
		m_menu.show();
	});
	m_ui->tv_inputVec->connect( m_ui->tv_inputVec, &QTableView::clicked, this, &MainWindow::OnTableViewClick );

	//menu connect
	m_menu.connect(actUpdateDelay, &QAction::triggered, this, &MainWindow::TableViewUpdateDelay);
	m_menu.connect(updateSingleRow, &QAction::triggered, this, &MainWindow::TableViewUpdateSingleView);
	m_menu.connect(actCopy, &QAction::triggered, this, &MainWindow::TableViewCopy);
	m_menu.connect(actPaste, &QAction::triggered, this, &MainWindow::TableViewPaste);
	m_menu.connect(actCopyInput, &QAction::triggered, this, &MainWindow::TableViewCopyInput);
	m_menu.connect( actInsertCopyInput, &QAction::triggered, this, &MainWindow::TableViewInsertCopyInput );
	m_menu.connect( actInsertCopyInputDown, &QAction::triggered, this, &MainWindow::TableViewInsertCopyInputDown );
	m_menu.connect(actPasteOverwriteInput, &QAction::triggered, this, &MainWindow::TableViewPasteOverwriteInput);
	m_menu.connect(actDel, &QAction::triggered, this, &MainWindow::TableViewDel);
	m_menu.connect(actDelAll, &QAction::triggered, this, &MainWindow::OnBtnDelAllInput);
	m_menu.connect(actJump, &QAction::triggered, this, &MainWindow::TableViewJump);
}

void MainWindow::TableViewUpdateDelay()
{
	short delay = m_ui->edt_delay->text().toShort();

	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		auto indexList = model->selectedIndexes();
		for (auto &index : indexList)
		{
			if (index.column() != 4)
			{
				ShowMessageBox("选择了错误的列更新延迟");
				return;
			}

			m_inputDataModel.setData(index, delay);

			GetInputDataFromModel(index.row(), index.column());
		}
	}
}

void MainWindow::TableViewCopy()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		//先清空复制列表
		m_copyList.clear();

		auto indexList = model->selectedIndexes();
		for (auto &index : indexList)
		{
			m_copyList.push_back(m_inputDataModel.data(index));
		}
	}
}

void MainWindow::TableViewPaste()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		auto indexList = model->selectedIndexes();
		auto size = indexList.size();

		if (m_copyList.size() != 1)
		{
			if (size != m_copyList.size())
			{
				ShowMessageBox("复制的个数和要粘贴的个数不一样");
				return;
			}
	
			for (int i = 0; i < size; ++i)
			{
				m_inputDataModel.setData(indexList[i], m_copyList[i]);
				GetInputDataFromModel(indexList[i].row(), indexList[i].column());
			}
		} 
		else
		{
			for (auto &index : indexList)
			{
				m_inputDataModel.setData(index, m_copyList[0]);
				GetInputDataFromModel(index.row(), index.column());
			}
		}
	}
}

void MainWindow::TableViewCopyInput()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		m_copyInputVec.clear();
		auto indexList = model->selectedIndexes();
		for (auto &index : indexList)
		{
			m_copyInputVec.push_back(m_inputVec[index.row()]);
		}
	}
}

void MainWindow::TableViewInsertCopyInput()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		int insertCount = (int)m_copyInputVec.size();

		auto indexList = model->selectedIndexes();
		if (indexList.size() > 1)
		{
			ShowMessageBox("粘贴行时，只能选定一行作为插入索引");
			return;
		}
		else if (0 == insertCount)
		{
			ShowMessageBox("粘贴内容为空");
			return;
		}

		int index = indexList[0].row();
		int alreadyInsertCount = 0;
		while (alreadyInsertCount < insertCount)
		{
			int i = 0;
			for (auto it = m_inputVec.begin(); it != m_inputVec.end(); ++it, ++i)
			{
				if (i < index)
				{
					continue;
				}

				m_inputVec.insert(it, m_copyInputVec[alreadyInsertCount]);
				++alreadyInsertCount;
				//连续插入时，需要把index后移
				++index;

				break;
			}
		}

		SetInputDataModel();
		m_ui->tv_inputVec->setCurrentIndex( m_inputDataModel.index( index - 1, 0 ) );
	}
}

void MainWindow::TableViewInsertCopyInputDown()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if ( model->hasSelection() )
	{
		int insertCount = ( int )m_copyInputVec.size();

		auto indexList = model->selectedIndexes();
		if ( indexList.size() > 1 )
		{
			ShowMessageBox( "粘贴行时，只能选定一行作为插入索引" );
			return;
		}
		else if ( 0 == insertCount )
		{
			ShowMessageBox( "粘贴内容为空" );
			return;
		}

		int index = indexList[0].row();
		index += 1;//向下插入
		int alreadyInsertCount = 0;
		if (index < m_inputVec.size() )
		{
			while ( alreadyInsertCount < insertCount )
			{
				int i = 0;
				for ( auto it = m_inputVec.begin(); it != m_inputVec.end(); ++it, ++i )
				{
					if ( i < index )
					{
						continue;
					}
	
					m_inputVec.insert( it, m_copyInputVec[alreadyInsertCount] );
					++alreadyInsertCount;
					//连续插入时，需要把index后移
					++index;
	
					break;
				}
			}
		} 
		else
		{
			for (auto &input : m_copyInputVec)
			{
				m_inputVec.push_back( input );
			}
		}

		SetInputDataModel();
		m_ui->tv_inputVec->setCurrentIndex( m_inputDataModel.index( index - 1, 0 ) );
	}
}

void MainWindow::TableViewPasteOverwriteInput()
{
	if (m_inputVec.size())
	{
		auto model = m_ui->tv_inputVec->selectionModel();
		if (model->hasSelection())
		{
			auto indexList = model->selectedIndexes();
			int index = indexList[0].row();
	
			if (indexList.size() != m_copyInputVec.size())
			{
				ShowMessageBox("复制的行数和要粘贴覆盖的行数不一致");
				return;
			}
	
			int alreadOverwriteNum = 0;
			for (auto &index : indexList)
			{
				m_inputVec[index.row()] = m_copyInputVec[alreadOverwriteNum++];
			}
	
			SetInputDataModel();
			SetTableViewIndex(index);
		}
// 		else
// 		{
// 			for (auto &input : m_copyInputVec)
// 			{
// 				m_inputVec.push_back(input);
// 			}
// 			SetInputDataModel();
// 		}
	} 
	else
	{
		for (auto &input : m_copyInputVec)
		{
			m_inputVec.push_back(input);
		}

		SetInputDataModel();
		SetTableViewIndex(0);
	}
}

void MainWindow::TableViewDel()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		int lastDelIndex = 0;
		auto indexList = model->selectedIndexes();
		decltype(m_inputVec) inputVecTmp;
		for (int i = 0; i < m_inputVec.size(); ++i)
		{
			bool bDelFlag = false;
			for (auto &index : indexList)
			{
				if (index.row() == i)
				{
					lastDelIndex = index.row();
					bDelFlag = true;
					break;
				}
			}

			if (bDelFlag)
				continue;

			inputVecTmp.push_back(m_inputVec[i]);
		}
		m_inputVec.swap(inputVecTmp);

		SetInputDataModel();

		m_ui->tv_inputVec->setCurrentIndex(m_inputDataModel.index(lastDelIndex - 1, 0));
	}
}

void MainWindow::TableViewUpdateSingleView()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if ( model->hasSelection() )
	{
		auto indexList = model->selectedIndexes();
		if ( indexList.size() != 1 )
		{
			ShowMessageBox( "一次只能更新一行数据" );
			return;
		}

		UpdateSelectInputData( indexList[0].row() );
		SetTableViewIndex(indexList[0].row());
	}
}

void MainWindow::TableViewJump()
{
	auto model = m_ui->tv_inputVec->selectionModel();
	if (model->hasSelection())
	{
		auto indexList = model->selectedIndexes();

		JumpInput(indexList[0].row(), m_inputVec);
	}
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

	//test code...
	//GetInputDataModel();
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
		SetInputDataModel();
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
	std::string strTmp;
	for (int i = 0; i < size; ++i)
	{
		InputData input;
		fread(&input, sizeof(InputData), 1, pFile);
		//临时改动，我先读取所有脚本一次，把重复次数都改到1，让以前的脚本可用
// 		input.repeatCount = 1;

		//插入数据前，先把picPath等路径转换到相对路径下
		strTmp = input.picPath;
		auto pos = strTmp.find_last_of( "/" );
		if ( std::string::npos != pos )
		{
			strTmp = strTmp.substr( pos + 1 );
			strTmp = DEFAULT_PIC_PATH + strTmp;
			strcpy_s( input.picPath, MAX_PATH, strTmp.c_str() );
		}

		strTmp = input.findPicSucceedJumpModule;
		pos = strTmp.find_last_of( "/" );
		if ( std::string::npos != pos )
		{
			strTmp = strTmp.substr( pos + 1 );
			strTmp = DEFAULT_PATH + strTmp;
			strcpy_s( input.findPicSucceedJumpModule, MAX_PATH, strTmp.c_str() );
		}

		strTmp = input.findPicOvertimeJumpModule;
		pos = strTmp.find_last_of( "/" );
		if ( std::string::npos != pos )
		{
			strTmp = strTmp.substr( pos + 1 );
			strTmp = DEFAULT_PATH + strTmp;
			strcpy_s( input.findPicOvertimeJumpModule, MAX_PATH, strTmp.c_str() );
		}

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
	ResetAllInputFinishFlag(m_inputVec);

#ifdef DEV_VER
	SetInputDataModel();
#endif
}

void MainWindow::LoadScriptModuleFileToSpecificInputVec(const char *file, std::vector<InputData> &inputVec)
{
	std::string strFilePath = file;
	inputVec.clear();

	//按照二进制读取
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		SetInputDataModel();
		AddTipInfo(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str());
		return;
	}

	//先读入两个窗口名(长度+str)
	int nameLen = 0;
	fread(&nameLen, sizeof(int), 1, pFile);
	char *pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);

	delete[]pStr;
	pStr = nullptr;

	fread(&nameLen, sizeof(int), 1, pFile);
	pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);

	delete[]pStr;
	pStr = nullptr;

	//然后读取操作数组的大小以及数据
	int size = 0;
	fread(&size, sizeof(int), 1, pFile);
	std::string strTmp;
	for (int i = 0; i < size; ++i)
	{
		InputData input;
		fread(&input, sizeof(InputData), 1, pFile);
		//临时改动，我先读取所有脚本一次，把重复次数都改到1，让以前的脚本可用
// 		input.repeatCount = 1;

		//插入数据前，先把picPath等路径转换到相对路径下
		strTmp = input.picPath;
		auto pos = strTmp.find_last_of("/");
		if (std::string::npos != pos)
		{
			strTmp = strTmp.substr(pos + 1);
			strTmp = DEFAULT_PIC_PATH + strTmp;
			strcpy_s(input.picPath, MAX_PATH, strTmp.c_str());
		}

		strTmp = input.findPicSucceedJumpModule;
		pos = strTmp.find_last_of("/");
		if (std::string::npos != pos)
		{
			strTmp = strTmp.substr(pos + 1);
			strTmp = DEFAULT_PATH + strTmp;
			strcpy_s(input.findPicSucceedJumpModule, MAX_PATH, strTmp.c_str());
		}

		strTmp = input.findPicOvertimeJumpModule;
		pos = strTmp.find_last_of("/");
		if (std::string::npos != pos)
		{
			strTmp = strTmp.substr(pos + 1);
			strTmp = DEFAULT_PATH + strTmp;
			strcpy_s(input.findPicOvertimeJumpModule, MAX_PATH, strTmp.c_str());
		}

		inputVec.push_back(input);
	}

	//最后读取窗口大小，因为是后添加的结构
	fread(&m_wndWidth, sizeof(int), 1, pFile);
	fread(&m_wndHeight, sizeof(int), 1, pFile);

	fclose(pFile);

#ifdef DEV_VER
	AddTipInfo(std::string("读取模块[").append(strFilePath).append("]成功，共读取命令").append(std::to_string(size)).append("条").c_str());
#endif

	//加载完后重新初始化窗口，因为窗口可能已经变动
	InitGameWindow();

	//因为保存的时候可能保存了改动的初始flag，所以加载模块时，把所有标记都重置一下，以确保正常使用
	ResetAllInputFinishFlag(inputVec);
}

void MainWindow::SetInputDataModel()
{
	auto row = m_inputVec.size();
	m_inputDataModel.clear();
	m_inputDataModel.setRowCount((int)row);

	m_inputDataModel.setColumnCount(g_horizontalHeaderLables.size());
	m_inputDataModel.setHorizontalHeaderLabels(g_horizontalHeaderLables);

	QStringList verticalHeaderList;
	for (int i = 0; i < row; ++i)
	{
		verticalHeaderList.push_back(QString::number(i));
	}
	m_inputDataModel.setVerticalHeaderLabels(verticalHeaderList);

	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < g_horizontalHeaderLables.size(); ++j)
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
			case 26:
				m_inputDataModel.setData(index, m_inputVec[i].repeatCount);
				break;
			case 27:
				m_inputDataModel.setData(index, m_inputVec[i].cmpParam);
				break;
			case 28:
				m_inputDataModel.setData(index, m_inputVec[i].outputParam);
				break;
			case 29:
				m_inputDataModel.setData(index, m_inputVec[i].waitTime);
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

void MainWindow::GetInputDataFromModel(int row, int col)
{
	int i = row;
	QModelIndex index = m_inputDataModel.index(row, col, QModelIndex());

	switch (col)
	{
	case 0:
		strcpy_s(m_inputVec[i].comment, MAX_PATH, m_inputDataModel.data(index).toString().toLocal8Bit().toStdString().c_str());
		break;
	case 1:
		m_inputVec[i].type = StrInputTypeMap[m_inputDataModel.data(index).toString().toLocal8Bit().toStdString()];
		break;
	case 2:
		m_inputVec[i].opType = StrOpTypeMap[m_inputDataModel.data(index).toString().toLocal8Bit().toStdString()];
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
		m_inputVec[i].yRate = m_inputDataModel.data(index).toString().toFloat();
		break;
	case 9:
		m_inputVec[i].x2 = m_inputDataModel.data(index).toString().toInt();
		break;
	case 10:
		m_inputVec[i].y2 = m_inputDataModel.data(index).toString().toInt();
		break;
	case 11:
		m_inputVec[i].xRate2 = m_inputDataModel.data(index).toString().toFloat();
		break;
	case 12:
		m_inputVec[i].yRate2 = m_inputDataModel.data(index).toString().toFloat();
		break;
	case 13:
		m_inputVec[i].startTime = m_inputDataModel.data(index).toString().toUInt();
		break;
	case 14:
		m_inputVec[i].bFinishFlag = m_inputDataModel.data(index).toBool();
		break;
	case 15:
		m_inputVec[i].bInitStartTimeFlag = m_inputDataModel.data(index).toBool();
		break;
	case 16:
		m_inputVec[i].bFindPicFlag = m_inputDataModel.data(index).toBool();
		break;
	case 17:
		m_inputVec[i].cmpPicRate = m_inputDataModel.data(index).toString().toFloat();
		break;
	case 18:
		strcpy_s(m_inputVec[i].picPath, MAX_PATH, m_inputDataModel.data(index).toString().toLocal8Bit().toStdString().c_str());
		break;
	case 19:
		m_inputVec[i].findPicOvertime = m_inputDataModel.data(index).toString().toInt();
		break;
	case 20:
		m_inputVec[i].bFindPicOvertimeFlag = m_inputDataModel.data(index).toBool();
		break;
	case 21:
		m_inputVec[i].findPicSucceedJumpIndex = m_inputDataModel.data(index).toString().toInt();
		break;
	case 22:
		m_inputVec[i].findPicOvertimeJumpIndex = m_inputDataModel.data(index).toString().toInt();
		break;
	case 23:
		strcpy_s(m_inputVec[i].findPicSucceedJumpModule, MAX_PATH, m_inputDataModel.data(index).toString().toLocal8Bit().toStdString().c_str());
		break;
	case 24:
		strcpy_s(m_inputVec[i].findPicOvertimeJumpModule, MAX_PATH, m_inputDataModel.data(index).toString().toLocal8Bit().toStdString().c_str());
		break;
	case 25:
		m_inputVec[i].bCmpPicCheckFlag = m_inputDataModel.data(index).toBool();
		break;
	case 26:
		m_inputVec[i].repeatCount = m_inputDataModel.data(index).toString().toShort();
		break;
	case 27:
		m_inputVec[i].cmpParam = m_inputDataModel.data(index).toString().toInt();
		break;
	case 28:
		m_inputVec[i].outputParam = m_inputDataModel.data(index).toString().toInt();
		break;
	case 29:
		m_inputVec[i].waitTime = m_inputDataModel.data(index).toString().toInt();
		break;
	default:
	{
		ShowMessageBox("出现致命model读取错误");
	}
	break;
	}
}

void MainWindow::ResetAllInputFinishFlag(std::vector<InputData> &inputVec)
{
	for (auto &input : inputVec)
	{
		input.bFinishFlag = false;
		input.bInitStartTimeFlag = false;
		input.bFindPicFlag = false;
		input.bFindPicOvertimeFlag = false;
		input.alreadyRepeatCount = 0;
	}
}

void MainWindow::JumpInput(int index, std::vector<InputData> &inputVec)
{
	int size = (int)inputVec.size();
	for (int i = 0; i < size; ++i)
	{
		if (i < index)
		{
			inputVec[i].bFinishFlag = true;
			inputVec[i].bFindPicFlag = true;
			inputVec[i].bInitStartTimeFlag = true;
		}
		else
		{
			inputVec[i].bFinishFlag = false;
			inputVec[i].bFindPicFlag = false;
			inputVec[i].bInitStartTimeFlag = false;
			inputVec[i].bFindPicOvertimeFlag = false;
// 			inputVec[i].alreadyRepeatCount = 0;
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

		//检测游戏本体窗口是否已经设置到了指定大小
		//暂时放弃自动调整窗口大小了，问题太多，一时半会儿改不好，让玩家自己保证游戏窗口的分辨率为960，540，以后都统一用这个了
// 		m_checkGameWndSizeTimer.stop();
// 		m_checkGameWndSizeTimer.start(1000);
	}
}

void MainWindow::UpdateGameWindowSize()
{
	if ( nullptr == m_hGameWnd )
	{
		return;
	}

	RECT rect;
	GetWindowRect(m_hGameWnd, &rect);
	m_gameWndSize.x = rect.right - rect.left;
	m_gameWndSize.y = rect.bottom - rect.top;

	if (m_gameWndSize.x != m_wndWidth || m_gameWndSize.y != m_wndHeight)
	{
		ShowMessageBox("您的游戏分辨率已经不是960x540，请调整好后重新开启脚本挂机");
	}
}

void MainWindow::CheckGameWndSize()
{
	m_curSimWndInfo.CheckGameWndSize(m_wndWidth, m_wndHeight);
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

void MainWindow::OnReturnPressedResolutionRateX()
{
	int width = m_ui->edt_wndWidth->text().toInt();
	ResolutionRateType type = (ResolutionRateType)m_ui->cb_resolutionRate->currentIndex();
	switch (type)
	{
	case Rate169:
		m_ui->edt_wndHeight->setText(QString::number(int(width / 1.777777777777778)));
		break;
	case Rate1610:
		m_ui->edt_wndHeight->setText(QString::number(int(width / 1.6)));
		break;
	default:
		break;
	}
}

void MainWindow::OnReturnPressedResolutionRateY()
{
	int height = m_ui->edt_wndHeight->text().toInt();
	ResolutionRateType type = (ResolutionRateType)m_ui->cb_resolutionRate->currentIndex();
	switch (type)
	{
	case Rate169:
		m_ui->edt_wndWidth->setText(QString::number(int(height * 1.777777777777778)));
		break;
	case Rate1610:
		m_ui->edt_wndWidth->setText(QString::number(int(height * 1.6)));
		break;
	default:
		break;
	}
}

void SimWndInfo::SetGameWndSize(int width, int height)
{
	if (nullptr == gameWnd)
	{
		return;
	}

	UpdateRect();
	
	//n层窗口可以算出n-1层的比例，以主窗口为参考（比例1），算出其他相应的比例，就可以设置大小了
	double rateW[MAX_LAYER] = { 0 };
	double rateH[MAX_LAYER] = { 0 };
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
		if (!bSetSizeFlagVec[i])
		{
			continue;
		}

		::SetWindowPos(layerWnd[i], HWND_BOTTOM, 0, 0, (width * rateW[totalFindLayer - 1] / rateW[i]), (height *  rateH[totalFindLayer - 1] / rateH[i]), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		InvalidateRect(layerWnd[i], nullptr, TRUE);
		Sleep(200);
	}
}
