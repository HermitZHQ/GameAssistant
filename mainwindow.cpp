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
		ShowMessageBox("�뱣������ͨ���������ͻ���");
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
		ShowMessageBox("���֤�ѹ��ڣ�����ϵ����Ա�����µ����֤");
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
	// 	ui->list_tip->addItem(QString::fromLocal8Bit("Ͷ����Ϣ�߳��ѿ�ʼ����..."));
	if (m_inputVec.size() == 0)
	{
		m_timer.stop();
		AddTipInfo(QString::fromLocal8Bit("�ű���������Ϊ0���߳��˳�..."));
		return;
	}

	//ִ�нű�ʱΪ˳��ִ�У�ÿִ����һ��finishflag�ͱ�ʶΪtrue��������Ժ��������б�ʶ�����з���ѭ��
// 	while (m_hWnd && !m_stopFlag)
// 	{
		//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
	UpdateGameWindowSize();
#ifdef DEV_VER
	int iCurCount = m_ui->list_tip->count();
	static int iLastCount = iCurCount;
	//�����ʾ�б��С������һ��ֵ���
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
		//�ж����
		if ((input.bFinishFlag && input.type != Pic) || (input.type == Pic && (input.bFindPicFlag || input.bFindPicOvertimeFlag)))
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

		//���û���ҵ�ͼƬ������������������һ��
		if (InputType::Pic == input.type && !input.bFindPicFlag)
		{
			break;
		}
		//ͼƬ�Աȳɹ�ָ����ת
		else if (InputType::Pic == input.type && input.bFindPicFlag && -1 != input.findPicSucceedJumpIndex)
		{
			(0xffff != input.findPicSucceedJumpIndex) ? JumpInput(input.findPicSucceedJumpIndex) :
				LoadInputModuleFile(input.findPicSucceedJumpModule);
			break;
		}

		//ֻҪ������ˣ��ͱ��Ϊ�������
		input.bFinishFlag = true;
	}

	if (bAllFinishedFlag)
	{
		ResetAllInputFinishFlag();
	}

	// 		Sleep(1);
	// 	}


	// 	ui->list_tip->addItem(QString::fromLocal8Bit("�߳����˳�..."));
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
	AddTipInfo(QString::fromLocal8Bit("��ʼ�ű�����..."));
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	AddTipInfo(QString::fromLocal8Bit("ֹͣ�ű�����..."));
}

void MainWindow::OnBtnAddInput()
{
	//���ǰ�ȸ�����Ϸ���ڴ�С
	UpdateGameWindowSize();

	int repeatTime = m_ui->edt_repeat->text().toShort();

	InputData input;
	GetInputData(input);

	for (int i = 0; i < repeatTime; ++i)
	{
		m_inputVec.push_back(input);
	}

	AddTipInfo(QString::fromLocal8Bit(std::string("�����").append(std::to_string(repeatTime)).append("��ָ��").c_str()));

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

	AddTipInfo(QString::fromLocal8Bit("��ɾ����һ��ָ��"));
	AddTipInfo(QString::fromLocal8Bit(std::string("��ʣ��").append(std::to_string(m_inputVec.size())).append("��ָ��").c_str()));

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDelAllInput()
{
	m_inputVec.clear();
	AddTipInfo(QString::fromLocal8Bit("��ɾ������ָ��"));

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
	AddTipInfo(QString::fromLocal8Bit("�Ѹ�������ָ��"));
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

		//�ѵ�ǰinput����ѯ��ʾ��editbox��
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
		strTmp += " ����:";
		switch (input.type)
		{
		case Mouse:
			strTmp += "���";
			break;
		case Keyboard:
			strTmp += "����";
			break;
		case Pic:
			strTmp += "ͼƬ";
			break;
		default:
			strTmp += "δ֪";
			break;
		}

		if (InputType::Pic != input.type)
		{
			strTmp += " ��ʽ:";
			switch (input.opType)
			{
			case Click:
				strTmp += "���";
				break;
			case Press:
				strTmp += "��ס";
				break;
			case Move:
				strTmp += "�ƶ�";
				break;
			case Release:
				strTmp += "�ͷ�";
				break;
			}

			strTmp += " �ӳ�:";
			strTmp += std::to_string(input.delay);
		}

		if (InputType::Keyboard == input.type)
		{
			strTmp += " ��ֵ:";
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

			strTmp += " ·��:";
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
		ShowMessageBox("û��ѡ����Ч��Ŀ������");
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
		ShowMessageBox("����Ŀ�����ʼ����ֵ��Ч");
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
		ShowMessageBox("Lisence�·���Ч");
		return;
	}

	//���ն����ƴ洢
	FILE *pFile = nullptr;
	std::string strFilePath = DEFAULT_PATH;
	strFilePath.append("Lisence.nn");

	QFileInfo file(strFilePath.c_str());
	if (file.exists() && file.isFile())
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("�ļ��Ѵ��ڣ��Ƿ񸲸�?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (QMessageBox::Cancel == ret)
		{
			AddTipInfo(QString::fromLocal8Bit("��ȡ������"));
			return;
		}
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	srand(time(nullptr));
	int iRand = 0;
	//��д��20��int���ȵ����ֵ
	for (int i = 0; i < 20; ++i)
	{
		iRand = rand();
		fwrite(&iRand, sizeof(int), 1, pFile);
	}

	//�м�д�������ڴ棬���ﻹ�Ƿֱ�д�룬���²�ͬ����ϵͳĬ�ϵĽṹ���С��ͬ
	QDateTime curDate = QDateTime::currentDateTime();
	QDateTime endDate = curDate.addMonths(lisenceMonth);
// 	QDateTime endDate = curDate;
	int year, month, day, hour, minute, second;

	//����ֻ��д����ֹʱ�䣬��ΪҪ�Ƚϵĵ�ǰʱ���Ǵ������ȡ
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

	//����ٶ�̬д��������ȵ�intֵ����֤ÿ�����ɵ�lisence���Ȳ�һ����������С������̶ֳ�Ӧ�ù���
	int iRandCount = rand() % 100 + 50;
	for (int i = 0; i < iRandCount; ++i)
	{
		iRand = rand();
		fwrite(&iRand, sizeof(int), 1, pFile);
	}

	fclose(pFile);
	AddTipInfo(QString::fromLocal8Bit("�����ļ��ɹ�"));
}

void MainWindow::OnBtnLisenceInfo()
{
	std::string strFilePath = DEFAULT_PATH;
	strFilePath.append("Lisence.nn");

	//���ն����ƶ�ȡ
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		AddTipInfo(QString::fromLocal8Bit(std::string("��ȡ����ģ��[").append(strFilePath).append("]ʧ��").c_str()));
		return;
	}

	//��ȡ��20��int
	int iTmp = 0;
	for (int i = 0; i < 20; ++i)
	{
		fread(&iTmp, sizeof(int), 1, pFile);
	}

	//ȡ������
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

	//�����������ֵ���ô���

	fclose(pFile);
}

bool MainWindow::CheckLisence()
{
	OnBtnLisenceInfo();

	//�Ƚ���ʵ������ʱ��
	m_curDate = QDateTime(QDate(m_year, m_month, m_day), QTime(m_hour, m_minute, m_second));
	qint64 leftSecond = m_curDate.secsTo(m_endDate);

	if (leftSecond > 0)
	{
		ShowMessageBox(std::string("������޻�ʣ��").append(std::to_string(m_curDate.daysTo(m_endDate))).append("��").c_str());
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

	//���ն����ƴ洢
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
		msgBox.setText(QString::fromLocal8Bit("�ļ��Ѵ��ڣ��Ƿ񸲸�?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (QMessageBox::Cancel == ret)
		{
			AddTipInfo(QString::fromLocal8Bit("��ȡ������"));
			return;
		}
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	//��д������������(����+str)
	int nameLen = (int)m_ui->edt_wndName->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(m_ui->edt_wndName->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

	nameLen = (int)m_ui->edt_wndName2->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(m_ui->edt_wndName2->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

	//Ȼ������������Ĵ�С�Լ�����
	int size = (int)m_inputVec.size();
	fwrite(&size, sizeof(int), 1, pFile);
	for (auto &input : m_inputVec)
	{
		fwrite(&input, sizeof(InputData), 1, pFile);
	}

	//���д�봰�ڴ�С����Ϊ���Ǻ�ӵĽṹ��Ϊ�˲�Ӱ����ǰ�Ľű�����
	fwrite(&m_wndWidth, sizeof(int), 1, pFile);
	fwrite(&m_wndHeight, sizeof(int), 1, pFile);

	fclose(pFile);
	AddTipInfo(QString::fromLocal8Bit("�����ļ��ɹ�"));
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

	//���ն����ƶ�ȡ
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		AddTipInfo(QString::fromLocal8Bit(std::string("��ȡ����ģ��[").append(strFilePath).append("]ʧ��").c_str()));
		return;
	}

	//��д������������(����+str)
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

	//Ȼ������������Ĵ�С�Լ�����
	int size = 0;
	fread(&size, sizeof(int), 1, pFile);
	for (int i = 0; i < size; ++i)
	{
		InputData input;
		fread(&input, sizeof(InputData), 1, pFile);
		m_inputVec.push_back(input);
	}

	//����ȡ���ڴ�С����Ϊ�Ǻ���ӵĽṹ
	fread(&m_wndWidth, sizeof(int), 1, pFile);
	fread(&m_wndHeight, sizeof(int), 1, pFile);
#ifdef DEV_VER
	m_ui->edt_wndWidth->setText(std::to_string(m_wndWidth).c_str());
	m_ui->edt_wndHeight->setText(std::to_string(m_wndHeight).c_str());
#endif

	fclose(pFile);

	//���±����ļ���ʾ�����ƣ��������ױ��渲�Ǵ�����Ϊ������ģ����ת����
	auto findPos = strFilePath.find_last_of("/");
	if (std::string::npos != findPos)
	{
		strFilePath = strFilePath.substr(findPos + 1);
	}

#ifdef DEV_VER
	m_ui->edt_saveName->setText(strFilePath.c_str());
	AddTipInfo(QString::fromLocal8Bit(std::string("��ȡģ��[").append(strFilePath).append("]�ɹ�������ȡ����").append(std::to_string(size)).append("��").c_str()));
#endif

	//����������³�ʼ�����ڣ���Ϊ���ڿ����Ѿ��䶯
	InitGameWindow();
	//��Ϊ�����ʱ����ܱ����˸Ķ��ĳ�ʼflag�����Լ���ģ��ʱ�������б�Ƕ�����һ�£���ȷ������ʹ��
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
		//move��ʱ��Ĭ������������
		PostMessage(m_hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
	break;
	default:
		AddTipInfo(QString::fromLocal8Bit("����δ�����������..."));
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
		AddTipInfo(QString::fromLocal8Bit("����δ����ļ��̲���..."));
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

		// 	ui->list_tip->addItem(QString::fromLocal8Bit("��ʼ��ʼ����Ϸ����..."));
		// 	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("�������ƣ�").append(ui->edt_wndName->text().toLocal8Bit().toStdString()).c_str()));

	m_hWnd = FindWindowA(nullptr, m_gameWndParentName.toLocal8Bit().toStdString().c_str());
	if (nullptr == m_hWnd)
	{
		AddTipInfo(QString::fromLocal8Bit("���Ҵ��ھ��ʧ�ܣ���ʼ����Ϸ����ʧ��"));
	}
	else
	{
		RECT rt;
		GetWindowRect(m_hWnd, &rt);

		//����Ĵ�С���ò�Ҫ�ٸĶ��ˣ����ֻ�����������û�й�ϵ����Ҫ�漰��ͼƬ�Աȣ���Ȼ����һ��������ͼƬ̫С�������Ժ�ʼ�ջ�ʧ�棬��Ϊԭ�Ա�ͼƬ�Ĵ�С�Ǵ�890 588�ķֱ����Ͻ�ȡ��
		if ((rt.right - rt.left) != m_wndHeight || (rt.bottom - rt.top) != m_wndHeight)
		{
			::SetWindowPos(m_hWnd, HWND_BOTTOM, rt.left, rt.top, m_wndWidth, m_wndHeight, SWP_NOMOVE | SWP_NOACTIVATE);
		}

		if (m_gameWndChildName.toLocal8Bit().toStdString().compare("") != 0)
		{
			EnumChildWindows(m_hWnd, &MainWindow::EnumChildProc, (LPARAM)this);
			if (nullptr == m_hChildWnd)
			{
				AddTipInfo(QString::fromLocal8Bit("�����Ӵ���ʧ��"));
				m_hWnd = nullptr;
			}
			else
			{
				// 				ui->list_tip->addItem(QString::fromLocal8Bit("�����Ӵ��ڳɹ�"));
				m_hWnd = m_hChildWnd;
			}
		}

		if (nullptr != m_hWnd)
		{
			AddTipInfo(QString::fromLocal8Bit("��ʼ����Ϸ���ڳɹ�"));
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