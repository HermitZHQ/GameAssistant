#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ZZPicCompareStrategy.h"
#include <tchar.h>
#include "QThread"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
	, m_hWnd(nullptr)
	, m_hChildWnd(nullptr)
	, m_stopFlag(false)
	, m_wndWidth(890)
	, m_wndHeight(588)
	, m_picCompareStrategy(new ZZPicCompareStrategy)
{
	ui->setupUi(this);
	m_picCompareStrategy->SetUi(ui);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::PostMsgThread()
{
// 	ui->list_tip->addItem(QString::fromLocal8Bit("投递消息线程已开始运作..."));
	if (m_inputVec.size() == 0)
	{
		m_timer.stop();
		ui->list_tip->addItem(QString::fromLocal8Bit("脚本中命令数为0，线程退出..."));
		return;
	}

	//执行脚本时为顺序执行，每执行完一个finishflag就标识为true，都完成以后重置所有标识，进行反复循环
// 	while (m_hWnd && !m_stopFlag)
// 	{
		//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
		UpdateGameWindowSize();
		int iCurCount = ui->list_tip->count();
		static int iLastCount = iCurCount;
		//检测提示列表大小，超过一定值清空
		if (iCurCount > 500)
		{
			ui->list_tip->clear();
		}
		else if (iLastCount != iCurCount)
		{
			ui->list_tip->scrollToBottom();
		}
		iLastCount = iCurCount;

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
						(LoadInputModuleFile(input.findPicOvertimeJumpModule));
					break;
				}
				continue;
			}

			ui->edt_cmpPic->setText(QString::fromLocal8Bit("[").toStdString().append(std::to_string(index)).append("]").c_str());

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
					LoadInputModuleFile(input.findPicSucceedJumpModule);
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
	MainWindow *pWnd = (MainWindow *)lParam;

	char strTmp[MAX_PATH] = { 0 };
	GetWindowTextA(hwnd, strTmp, MAX_PATH);
	if (strcmp(strTmp, pWnd->m_gameWndChildName.toLocal8Bit().toStdString().c_str()) == 0)
	{
		pWnd->m_hChildWnd = hwnd;
		RECT rect;
		GetWindowRect(pWnd->m_hChildWnd, &rect);
		pWnd->m_gameWndSize.x = rect.right - rect.left;
		pWnd->m_gameWndSize.y = rect.bottom - rect.top;

		return FALSE;
	}
	return TRUE;
}

void MainWindow::OnBtnStartClick()
{
	m_stopFlag = false;
	if (nullptr == m_hWnd)
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
	ui->list_tip->addItem(QString::fromLocal8Bit("开始脚本处理..."));
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	ui->list_tip->addItem(QString::fromLocal8Bit("停止脚本处理..."));
}

void MainWindow::OnBtnAddInput()
{
	//添加前先更新游戏窗口大小
	UpdateGameWindowSize();

	int repeatTime = ui->edt_repeat->text().toShort();

	InputData input;
	GetInputData(input);

	for (int i = 0; i < repeatTime; ++i)
	{
		m_inputVec.push_back(input);
	}

	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("已添加").append(std::to_string(repeatTime)).append("条指令").c_str()));

	RefreshInputVecUIList();
}

void MainWindow::GetInputData(InputData &input)
{
	//----comment
	strcpy_s(input.comment, PATH_LEN, ui->edt_comment->text().toLocal8Bit().toStdString().c_str());
	//----keyboard
	input.type = (InputType)ui->cb_inputType->currentIndex();
	input.opType = (OpType)ui->cb_opType->currentIndex();
	input.delay = ui->edt_delay->text().toShort();
	input.vk = ui->edt_vk->text().toLocal8Bit()[0];
	//----mouse
	if (InputType::Mouse == input.type || InputType::Pic == input.type)
	{
		input.x = ui->edt_x->text().toShort();
		input.y = ui->edt_y->text().toShort();
		input.xRate = (float)input.x / (float)m_gameWndSize.x;
		input.yRate = (float)input.y / (float)m_gameWndSize.y;
	}
	//----pic
	if (InputType::Pic == input.type)
	{
		input.bCmpPicCheckFlag = ui->chk_cmpPicClick->isChecked();
		input.findPicOvertime = ui->edt_findPicOvertime->text().toShort();
		input.findPicSucceedJumpIndex = ui->edt_succeedJump->text().toInt();
		input.findPicOvertimeJumpIndex = ui->edt_overtimeJump->text().toInt();
		input.cmpPicRate = ui->edt_rate->text().toFloat();
		strcpy_s(input.findPicSucceedJumpModule, PATH_LEN, ui->edt_succeedJumpModule->text().toLocal8Bit().toStdString().c_str());
		strcpy_s(input.findPicOvertimeJumpModule, PATH_LEN, ui->edt_overtimeJumpModule->text().toLocal8Bit().toStdString().c_str());

		input.x2 = ui->edt_x2->text().toShort();
		input.y2 = ui->edt_y2->text().toShort();
		input.xRate2 = (float)input.x2 / (float)m_gameWndSize.x;
		input.yRate2 = (float)input.y2 / (float)m_gameWndSize.y;
		strcpy_s(input.picPath, PATH_LEN, ui->edt_picPath->text().toLocal8Bit().toStdString().c_str());
	}
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

	ui->list_tip->addItem(QString::fromLocal8Bit("已删除上一条指令"));
	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("还剩下").append(std::to_string(m_inputVec.size())).append("条指令").c_str()));

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDelAllInput()
{
	m_inputVec.clear();
	ui->list_tip->addItem(QString::fromLocal8Bit("已删除所有指令"));

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
	ui->list_tip->addItem(QString::fromLocal8Bit("已更新所有指令"));
}

void MainWindow::OnBtnDelSelectInputClick()
{
	auto index = ui->list_inputVec->currentIndex();

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

void MainWindow::OnBtnInputListClick()
{
	auto index = ui->list_inputVec->currentIndex();

	auto it = m_inputVec.begin();
	for (int i = 0; i < m_inputVec.size(); ++i, ++it)
	{
		if (i != index.row())
			continue;

		//把当前input的咨询显示到editbox中
		ui->cb_inputType->setCurrentIndex(it->type);
		ui->cb_opType->setCurrentIndex(it->opType);
		ui->edt_vk->setText(std::string(1, it->vk).c_str());
		ui->edt_delay->setText(std::to_string(it->delay).c_str());
		ui->edt_comment->setText(QString::fromLocal8Bit(it->comment));

		ui->edt_x->setText(std::to_string(it->x).c_str());
		ui->edt_y->setText(std::to_string(it->y).c_str());
		ui->edt_x2->setText(std::to_string(it->x2).c_str());
		ui->edt_y2->setText(std::to_string(it->y2).c_str());

		ui->edt_rate->setText(std::to_string(it->cmpPicRate).c_str());
		ui->edt_picPath->setText(it->picPath);
		ui->edt_findPicOvertime->setText(std::to_string(it->findPicOvertime).c_str());
		ui->edt_succeedJump->setText(std::to_string(it->findPicSucceedJumpIndex).c_str());
		ui->edt_overtimeJump->setText(std::to_string(it->findPicOvertimeJumpIndex).c_str());
		ui->edt_overtimeJumpModule->setText(it->findPicOvertimeJumpModule);
		ui->edt_succeedJumpModule->setText(it->findPicSucceedJumpModule);
		ui->chk_cmpPicClick->setChecked(it->bCmpPicCheckFlag);

		break;
	}
}

void MainWindow::OnBtnUpdateSelectInputClick()
{
	UpdateGameWindowSize();
	auto index = ui->list_inputVec->currentIndex();

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
	int index = ui->edt_insertIndex->text().toInt();

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

void MainWindow::RefreshInputVecUIList()
{
	ui->list_inputVec->clear();

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
		default:
			strTmp += "未知";
			break;
		}

		if (InputType::Pic != input.type)
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

		if (InputType::Keyboard == input.type)
		{
			strTmp += " 键值:";
			strTmp += input.vk;
		}

		if (InputType::Mouse == input.type || InputType::Pic == input.type)
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

		if (InputType::Pic == input.type)
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

		ui->list_inputVec->addItem(QString::fromLocal8Bit(strTmp.c_str()));
	}
}

void MainWindow::OnBtnClearTipInfo()
{
	ui->list_tip->clear();
}

void MainWindow::OnBtnOpenFileDialog()
{
	auto res = QFileDialog::getOpenFileName(this, "", DEFAULT_PATH);
	if (res.compare("") == 0)
	{
		return;
	}

// 	ui->edt_saveName->setText(res);
	LoadInputModuleFile(res.toLocal8Bit().toStdString().c_str());
}

void MainWindow::OnBtnOpenFileDialog_PicPath()
{
	auto res = QFileDialog::getOpenFileName(this, "", DEFAULT_PATH);
	ui->edt_picPath->setText(res);
}

void MainWindow::OnBtnSaveClick()
{
	m_wndWidth = ui->edt_wndWidth->text().toInt();
	m_wndHeight = ui->edt_wndHeight->text().toInt();

	//按照二进制存储
	FILE *pFile = nullptr;
	std::string strFilePath = ui->edt_saveName->text().toLocal8Bit().toStdString();
	auto pos = strFilePath.find("/");
	if (std::string::npos == pos)
	{
		strFilePath = DEFAULT_PATH + strFilePath;
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	//先写入两个窗口名(长度+str)
	int nameLen = (int)ui->edt_wndName->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(ui->edt_wndName->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

	nameLen = (int)ui->edt_wndName2->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(ui->edt_wndName2->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

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
	ui->list_tip->addItem(QString::fromLocal8Bit("保存文件成功"));
}

void MainWindow::OnBtnLoadClick()
{
	std::string strFilePath = ui->edt_saveName->text().toLocal8Bit().toStdString();
	auto pos = strFilePath.find("/");
	if (std::string::npos == pos)
	{
		strFilePath = DEFAULT_PATH + strFilePath;
	}

	LoadInputModuleFile(strFilePath.c_str());
}

void MainWindow::LoadInputModuleFile(const char *file)
{
	std::string strFilePath = file;
	m_inputVec.clear();

	//按照二进制读取
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		ui->list_tip->addItem(QString::fromLocal8Bit(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str()));
		return;
	}

	//先写入两个窗口名(长度+str)
	int nameLen = 0;
	fread(&nameLen, sizeof(int), 1, pFile);
	char *pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);
	ui->edt_wndName->setText(QString::fromLocal8Bit(pStr));
	delete[]pStr;
	pStr = nullptr;

	fread(&nameLen, sizeof(int), 1, pFile);
	pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);
	ui->edt_wndName2->setText(QString::fromLocal8Bit(pStr));
	delete[]pStr;
	pStr = nullptr;

	//然后存入操作数组的大小以及数据
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
	ui->edt_wndWidth->setText(std::to_string(m_wndWidth).c_str());
	ui->edt_wndHeight->setText(std::to_string(m_wndHeight).c_str());

	fclose(pFile);

	//更新保存文件显示的名称，否则容易保存覆盖错误，因为现在有模块跳转功能
	auto findPos = strFilePath.find_last_of("/");
	if (std::string::npos != findPos)
	{
		strFilePath = strFilePath.substr(findPos + 1);
	}

	ui->edt_saveName->setText(strFilePath.c_str());
	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("读取模块[").append(strFilePath).append("]成功，共读取命令").append(std::to_string(size)).append("条").c_str()));

	//加载完后重新初始化窗口，因为窗口可能已经变动
	InitGameWindow();
	//因为保存的时候可能保存了改动的初始flag，所以加载模块时，把所有标记都重置一下，以确保正常使用
	ResetAllInputFinishFlag();
	RefreshInputVecUIList();
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
		PostMessage(m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
		PostMessage(m_hWnd, WM_LBUTTONUP, 0, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
		break;
	case Press:
	{
		PostMessage(m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
		break;
	case Release:
	{
		PostMessage(m_hWnd, WM_LBUTTONUP, 0, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
	break;
	case Move:
	{
		//move的时候默认鼠标左键按下
		PostMessage(m_hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
		break;
	default:
		ui->list_tip->addItem(QString::fromLocal8Bit("错误：未处理的鼠标操作..."));
		break;
	}
}

void MainWindow::HandleKeyboardInput(InputData &input)
{
	switch (input.opType)
	{
	case Click:
	{
		PostMessage(m_hWnd, WM_KEYDOWN, input.vk, 0);
		PostMessage(m_hWnd, WM_KEYUP, input.vk, 0);
	}
	break;
	case Press:
	case Move:
	default:
		ui->list_tip->addItem(QString::fromLocal8Bit("错误：未处理的键盘操作..."));
		break;
	}
}

void MainWindow::HandleGameImgCompare(InputData &input)
{
	m_picCompareStrategy->HandlePicCompare(input, m_hWnd, m_gameWndSize);
}

void MainWindow::InitGameWindow()
{
	m_hWnd = nullptr;
	m_hChildWnd = nullptr;

	m_gameWndParentName = ui->edt_wndName->text();
	m_gameWndChildName = ui->edt_wndName2->text();

// 	ui->list_tip->addItem(QString::fromLocal8Bit("开始初始化游戏窗口..."));
// 	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("窗口名称：").append(ui->edt_wndName->text().toLocal8Bit().toStdString()).c_str()));

	m_hWnd = FindWindowA(nullptr, m_gameWndParentName.toLocal8Bit().toStdString().c_str());
	if (nullptr == m_hWnd)
	{
		ui->list_tip->addItem(QString::fromLocal8Bit("查找窗口句柄失败，初始化游戏窗口失败"));
	}
	else
	{
		RECT rt;
		GetWindowRect(m_hWnd, &rt);

		//这里的大小设置不要再改动了，如果只是鼠标点击倒是没有关系，主要涉及到图片对比，虽然比例一样，但是图片太小了拉伸以后始终会失真，因为原对比图片的大小是从890 588的分辨率上截取的
		if ((rt.right - rt.left) != m_wndHeight || (rt.bottom - rt.top) != m_wndHeight)
		{
			::SetWindowPos(m_hWnd, HWND_BOTTOM, rt.left, rt.top, m_wndWidth, m_wndHeight, SWP_NOMOVE | SWP_NOACTIVATE);
		}

		if (m_gameWndChildName.toLocal8Bit().toStdString().compare("") != 0)
		{
			EnumChildWindows(m_hWnd, &MainWindow::EnumChildProc, (LPARAM)this);
			if (nullptr == m_hChildWnd)
			{
				ui->list_tip->addItem(QString::fromLocal8Bit("查找子窗口失败"));
				m_hWnd = nullptr;
			}
			else
			{
// 				ui->list_tip->addItem(QString::fromLocal8Bit("查找子窗口成功"));
				m_hWnd = m_hChildWnd;
			}
		}

		if (nullptr != m_hWnd)
		{
			ui->list_tip->addItem(QString::fromLocal8Bit("初始化游戏窗口成功"));
		}
	}
}

void MainWindow::UpdateGameWindowSize()
{
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	m_gameWndSize.x = rect.right - rect.left;
	m_gameWndSize.y = rect.bottom - rect.top;
}