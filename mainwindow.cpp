#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ZZPicCompareStrategy.h"
#include <tchar.h>
#include "QThread"
#include "QFileDialog"
#include "QtWidgets/QMessageBox"
#include "QDebug"
#include <time.h>

#import "./NtpTime.tlb"
using namespace NtpTime;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
	, m_hWnd(nullptr)
	, m_hChildWnd(nullptr)
	, m_stopFlag(false)
	, m_wndWidth(890)
	, m_wndHeight(588)
	, m_picCompareStrategy(new ZZPicCompareStrategy)
	, m_playerUI(this)
	, m_year(0)
{
	CoUninitialize();
	auto res = CoInitialize(nullptr);
	ITimeHelperPtr timeHelper(__uuidof(TimeHelper));
	std::string strRes = timeHelper->getWebTime(&m_year, &m_month, &m_day, &m_hour, &m_minute, &m_second);
	m_mac = timeHelper->getMac();

	if (m_year == 666)
	{
		ShowMessageBox("请保持网络通畅，重启客户端");
		destroy();
		return;
	}


#ifdef DEV_VER
	m_ui->setupUi(this);
	setParent(&m_bkgUI);
	m_picCompareStrategy->SetUi(m_ui);
	m_bkgUI.setGeometry(geometry());
	setWindowTitle("Develop-Ver 1.0.8");
#else
	m_bkgUI.setWindowTitle("Game-Assistant");
	if (!CheckLisence())
	{
		ShowMessageBox("许可证已过期，请联系管理员购买新的许可证");
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
		AddTipInfo(QString::fromLocal8Bit("脚本中命令数为0，线程退出..."));
		return;
	}

	//执行脚本时为顺序执行，每执行完一个finishflag就标识为true，都完成以后重置所有标识，进行反复循环
// 	while (m_hWnd && !m_stopFlag)
// 	{
		//获取最新的游戏窗口大小，通过比例来进行鼠标点击，可以保证窗口任意大小都能点击正确
	UpdateGameWindowSize();
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
	AddTipInfo(QString::fromLocal8Bit("开始脚本处理..."));
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	AddTipInfo(QString::fromLocal8Bit("停止脚本处理..."));
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

	AddTipInfo(QString::fromLocal8Bit(std::string("已添加").append(std::to_string(repeatTime)).append("条指令").c_str()));

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

	AddTipInfo(QString::fromLocal8Bit("已删除上一条指令"));
	AddTipInfo(QString::fromLocal8Bit(std::string("还剩下").append(std::to_string(m_inputVec.size())).append("条指令").c_str()));

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDelAllInput()
{
	m_inputVec.clear();
	AddTipInfo(QString::fromLocal8Bit("已删除所有指令"));

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
	AddTipInfo(QString::fromLocal8Bit("已更新所有指令"));
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

void MainWindow::OnBtnInputListClick()
{
	auto index = m_ui->list_inputVec->currentIndex();

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

				if (0 == insertCount)
				{
					input.opType = Press;
				}
				else if (1 == insertCount)
				{
					input.opType = Move;
					input.x = input.x2;
					input.y = input.y2;
					input.xRate = input.xRate2;
					input.yRate = input.yRate2;
				}
				else if (2 == insertCount)
				{
					input.opType = Release;
					input.x = input.x2;
					input.y = input.y2;
					input.xRate = input.xRate2;
					input.yRate = input.yRate2;
				}

				m_inputVec.insert(it, input);

				++index;
				++insertCount;
				break;
			}
		}
	}

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

		m_ui->list_inputVec->addItem(QString::fromLocal8Bit(strTmp.c_str()));
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

void MainWindow::OnBtnSetOverwriteTargetIndex()
{
	auto index = m_ui->list_inputVec->currentIndex();
	m_ui->edt_overwriteIndex->setText(std::to_string(index.row()).c_str());
}

void MainWindow::ShowMessageBox(const char *content)
{
	QMessageBox mb;
	mb.setWindowTitle("Info");
	mb.setText(QString::fromLocal8Bit(content));
	mb.setDefaultButton(QMessageBox::Ok);
	mb.exec();
}

void MainWindow::AddTipInfo(const QString &str)
{
#ifdef DEV_VER
	m_ui->list_tip->addItem(str);
#endif
}

void MainWindow::OnBtnLisence()
{
	int lisenceMonth = m_ui->edt_month->text().toShort();
	if (lisenceMonth <= 0 || lisenceMonth > 12)
	{
		ShowMessageBox("Lisence月份无效");
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
			AddTipInfo(QString::fromLocal8Bit("已取消保存"));
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
	QDateTime endDate = curDate.addMonths(lisenceMonth);
// 	QDateTime endDate = curDate;
	int year, month, day, hour, minute, second;

	//这里只用写入终止时间，因为要比较的当前时间是从网络获取
	year = endDate.date().year();
	month = endDate.date().month();
	day = endDate.date().day();
	hour = endDate.time().hour();
	minute = endDate.time().minute();
	second = endDate.time().second();
	fwrite(&year, sizeof(int), 1, pFile);
	fwrite(&hour, sizeof(int), 1, pFile);
	fwrite(&month, sizeof(int), 1, pFile);
	fwrite(&minute, sizeof(int), 1, pFile);
	fwrite(&day, sizeof(int), 1, pFile);
	fwrite(&second, sizeof(int), 1, pFile);

	//最后再动态写入随机长度的int值，保证每次生成的lisence长度不一样，我这种小软件这种程度应该够了
	int iRandCount = rand() % 100 + 50;
	for (int i = 0; i < iRandCount; ++i)
	{
		iRand = rand();
		fwrite(&iRand, sizeof(int), 1, pFile);
	}

	fclose(pFile);
	AddTipInfo(QString::fromLocal8Bit("保存文件成功"));
}

void MainWindow::OnBtnLisenceInfo()
{
	std::string strFilePath = DEFAULT_PATH;
	strFilePath.append("Lisence.nn");

	//按照二进制读取
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		AddTipInfo(QString::fromLocal8Bit(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str()));
		return;
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
	m_endDate = QDateTime(QDate(year, month, day), QTime(hour, minute, second));

	AddTipInfo(m_curDate.toString());
	AddTipInfo(m_endDate.toString());

	//最后的随机混乱值不用处理

	fclose(pFile);
}

bool MainWindow::CheckLisence()
{
	OnBtnLisenceInfo();

	//比较真实的网络时间
	m_curDate = QDateTime(QDate(m_year, m_month, m_day), QTime(m_hour, m_minute, m_second));
	qint64 leftSecond = m_curDate.secsTo(m_endDate);

	if (leftSecond > 0)
	{
		ShowMessageBox(std::string("许可期限还剩：").append(std::to_string(m_curDate.daysTo(m_endDate))).append("天").c_str());
		return true;
	}
	else
	{
		return false;
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
	LoadInputModuleFile(res.toLocal8Bit().toStdString().c_str());
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
			AddTipInfo(QString::fromLocal8Bit("已取消保存"));
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
	AddTipInfo(QString::fromLocal8Bit("保存文件成功"));
}

void MainWindow::OnBtnLoadClick()
{
	std::string strFilePath = m_ui->edt_saveName->text().toLocal8Bit().toStdString();
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
		AddTipInfo(QString::fromLocal8Bit(std::string("读取输入模块[").append(strFilePath).append("]失败").c_str()));
		return;
	}

	//先写入两个窗口名(长度+str)
	int nameLen = 0;
	fread(&nameLen, sizeof(int), 1, pFile);
	char *pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);
	m_gameWndParentName = QString::fromLocal8Bit(pStr);
#ifdef DEV_VER
	m_ui->edt_wndName->setText(QString::fromLocal8Bit(pStr));
#endif
	delete[]pStr;
	pStr = nullptr;

	fread(&nameLen, sizeof(int), 1, pFile);
	pStr = new char[nameLen];
	fread(pStr, 1, nameLen, pFile);
	m_gameWndChildName = QString::fromLocal8Bit(pStr);
#ifdef DEV_VER
	m_ui->edt_wndName2->setText(QString::fromLocal8Bit(pStr));
#endif
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
	AddTipInfo(QString::fromLocal8Bit(std::string("读取模块[").append(strFilePath).append("]成功，共读取命令").append(std::to_string(size)).append("条").c_str()));
#endif

	//加载完后重新初始化窗口，因为窗口可能已经变动
	InitGameWindow();
	//因为保存的时候可能保存了改动的初始flag，所以加载模块时，把所有标记都重置一下，以确保正常使用
	ResetAllInputFinishFlag();
#ifdef DEV_VER
	RefreshInputVecUIList();
#endif
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
		AddTipInfo(QString::fromLocal8Bit("错误：未处理的鼠标操作..."));
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
		AddTipInfo(QString::fromLocal8Bit("错误：未处理的键盘操作..."));
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

	// 	m_gameWndParentName = m_ui->edt_wndName->text();
	// 	m_gameWndChildName = m_ui->edt_wndName2->text();

		// 	ui->list_tip->addItem(QString::fromLocal8Bit("开始初始化游戏窗口..."));
		// 	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("窗口名称：").append(ui->edt_wndName->text().toLocal8Bit().toStdString()).c_str()));

	m_hWnd = FindWindowA(nullptr, m_gameWndParentName.toLocal8Bit().toStdString().c_str());
	if (nullptr == m_hWnd)
	{
		AddTipInfo(QString::fromLocal8Bit("查找窗口句柄失败，初始化游戏窗口失败"));
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
				AddTipInfo(QString::fromLocal8Bit("查找子窗口失败"));
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
			AddTipInfo(QString::fromLocal8Bit("初始化游戏窗口成功"));
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