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
	{InputType::Mouse, Q8("���")},
	{InputType::Keyboard, Q8("����")},
	{InputType::Pic, Q8("ͼƬ")},
	{InputType::StopScript, Q8("ֹͣ")},
};
std::unordered_map<QString, InputType> StrInputTypeMap = {
	{ Q8( "���" ), InputType::Mouse},
	{ Q8( "����" ), InputType::Keyboard},
	{ Q8( "ͼƬ" ), InputType::Pic},
	{ Q8( "ֹͣ" ), InputType::StopScript},
};

std::unordered_map<OpType, QString> OpTypeStrMap = {
	{OpType::Click, Q8("���")},
	{OpType::Press, Q8("��ס")},
	{OpType::Move, Q8("�ƶ�")},
	{OpType::Release, Q8("�ͷ�")},
};
std::unordered_map<QString, OpType> StrOpTypeMap = {
	{ Q8( "���" ), OpType::Click },
	{ Q8( "��ס" ), OpType::Press },
	{ Q8( "�ƶ�" ), OpType::Move },
	{ Q8( "�ͷ�" ), OpType::Release },
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
		{Thunder, SimWndInfo(QString::fromLocal8Bit("�׵�ģ����"))},
		{MuMu, SimWndInfo(QString::fromLocal8Bit("��װս�� - MuMuģ����"))},
		})
	, m_curSimWndInfo(QString::fromLocal8Bit("�׵�ģ����"))
{
	CoUninitialize();
	/*auto res = */CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	char cTmp[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, cTmp);
	std::string strComCmd = "/c RegAsm \"";
	strComCmd.append(cTmp);
	//ע��com���
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
		ShowMessageBox("�뱣������ͨ���������ͻ���");
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
	// 	ui->list_tip->addItem(QString::fromLocal8Bit("Ͷ����Ϣ�߳��ѿ�ʼ����..."));
	if (m_inputVec.size() == 0)
	{
		m_timer.stop();
		AddTipInfo("�ű���������Ϊ0���߳��˳�...");
		return;
	}

	//ִ�нű�ʱΪ˳��ִ�У�ÿִ����һ��finishflag�ͱ�ʶΪtrue��������Ժ��������б�ʶ�����з���ѭ��
// 	while (m_hWnd && !m_stopFlag)
// 	{

	//�����Ϸ���崰���Ƿ��Ѿ����õ���ָ����С
	m_curSimWndInfo.CheckGameWndSize(m_wndWidth, m_wndHeight);
	//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
	UpdateGameWindowSize();
	//ǿ�Ƹ��´������ݣ����㴰����С��
// 	::SetActiveWindow(m_hWnd);
// 	InvalidateRect(m_hWnd, nullptr, TRUE);
// 	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
// 	PostMessage(m_hWnd, WM_ACTIVATE, 0, 0);
// 	PostMessage(m_hWnd, WM_ERASEBKGND, 0, 0);
// 	PostMessage(m_hWnd, WM_PAINT, 0, 0);

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

	//��Ҫ�Ѹ���������bottom���ԣ��Ų��ᵯ������
// 	::SetWindowPos( m_hParentWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
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
			AddTipInfo("�ű���������ϣ����ֶ�ѡ�������ű�ִ��");
		}
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
				LoadScriptModuleFile(input.findPicSucceedJumpModule);
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
	SimWndInfo *pInfo = (SimWndInfo*)lParam;

	pInfo->AddLayer();
	//��ʹ�����Ʋ��ҵĻ���ֱ�ӷ����ҵ��ĵ�һ�����ڣ�Ŀǰmumu���׵綼������
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
	AddTipInfo("��ʼ�ű�����...");
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	AddTipInfo("ֹͣ�ű�����...");
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

	AddTipInfo(std::string("�����").append(std::to_string(repeatTime)).append("��ָ��").c_str());

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

	AddTipInfo("��ɾ����һ��ָ��");
	AddTipInfo(std::string("��ʣ��").append(std::to_string(m_inputVec.size())).append("��ָ��").c_str());

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDelAllInput()
{
	m_inputVec.clear();
	AddTipInfo("��ɾ������ָ��");

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
	AddTipInfo("�Ѹ�������ָ��");
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
		ShowMessageBox("�޷�����ɾ��3��ָ��");
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
				input.delay = 250;//Ĭ�ϲ���250�ӳ�

				if (0 == insertCount)
				{
					input.opType = Press;
					strcpy_s(input.comment, MAX_PATH, "�϶�-1");
				}
				else if (1 == insertCount)
				{
					input.opType = Move;
					input.x = input.x2;
					input.y = input.y2;
					input.xRate = input.xRate2;
					input.yRate = input.yRate2;
					strcpy_s(input.comment, MAX_PATH, "�϶�-2");
				}
				else if (2 == insertCount)
				{
					input.opType = Release;
					input.x = input.x2;
					input.y = input.y2;
					input.xRate = input.xRate2;
					input.yRate = input.yRate2;
					strcpy_s(input.comment, MAX_PATH, "�϶�-3");
				}

				m_inputVec.insert(it, input);

				++index;
				++insertCount;
				break;
			}
		}
	}

	//�������²���λ��
	m_ui->edt_insertIndex->setText(std::to_string(index).c_str());
	//����xy1��2��λ�ã������´�����
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
		case StopScript:
			strTmp += "ֹͣ";
			break;
		default:
			strTmp += "δ֪";
			break;
		}

		if (InputType::Pic != input.type && InputType::StopScript != input.type)
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

		if (InputType::Keyboard == input.type && InputType::StopScript != input.type)
		{
			strTmp += " ��ֵ:";
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

			strTmp += " ·��:";
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
		case StopScript:
			strTmp += "ֹͣ";
			break;
		default:
			strTmp += "δ֪";
			break;
		}

		if (InputType::Pic != input.type && InputType::StopScript != input.type)
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

		if (InputType::Keyboard == input.type && InputType::StopScript != input.type)
		{
			strTmp += " ��ֵ:";
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

			strTmp += " ·��:";
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

void MainWindow::OnBtnOverwriteDelay()
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
		ShowMessageBox("ģ���������ô���");
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
			ShowMessageBox("������������");
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
				//��������ʱ����Ҫ��index����
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

	//���ն����ƴ洢
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
		msgBox.setText(QString::fromLocal8Bit("�ļ��Ѵ��ڣ��Ƿ񸲸�?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (QMessageBox::Cancel == ret)
		{
			AddTipInfo("��ȡ������");
			return;
		}
	}

	fopen_s(&pFile, strFilePath.c_str(), "wb");
	if (nullptr == pFile)
		return;

	//Ȼ������������Ĵ�С�Լ�����
	int size = (int)m_inputModuleVec.size();
	fwrite(&size, sizeof(int), 1, pFile);
	for (auto &input : m_inputModuleVec)
	{
		fwrite(&input, sizeof(InputData), 1, pFile);
	}

	fclose(pFile);
	AddTipInfo("�����ļ��ɹ�");
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
		ShowMessageBox("Lisence�·���Ч");
		return;
	}

	QString strMac = GetMAC();
	if (strMac.compare("") == 0)
	{
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
			AddTipInfo("��ȡ������");
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

	//����ֻ��д����ֹʱ�䣬��ΪҪ�Ƚϵĵ�ǰʱ���Ǵ������ȡ
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

	//д��mac��ַ
	char cTmp[13] = { 0 };
	strcpy_s(cTmp, 13, strMac.toStdString().c_str());
	for (int i = 0; i < 12; ++i)
	{
		cTmp[i] ^= g_crypt;
	}
	fwrite(cTmp, 1, 13, pFile);

	//����ٶ�̬д��������ȵ�intֵ����֤ÿ�����ɵ�lisence���Ȳ�һ����������С������̶ֳ�Ӧ�ù���
	int iRandCount = rand() % 100 + 50;
	for (int i = 0; i < iRandCount; ++i)
	{
		iRand = rand();
		fwrite(&iRand, sizeof(int), 1, pFile);
	}

	fclose(pFile);
	AddTipInfo("�����ļ��ɹ�");
}

bool MainWindow::OnBtnLisenceInfo()
{
	std::string strFilePath = DEFAULT_PATH;
	strFilePath.append("Lisence.nn");

	//���ն����ƶ�ȡ
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		AddTipInfo(std::string("��ȡ����ģ��[").append(strFilePath).append("]ʧ��").c_str());
		return false;
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
	year ^= g_crypt;
	hour ^= g_crypt;
	month ^= g_crypt;
	minute ^= g_crypt;
	day ^= g_crypt;
	second ^= g_crypt;
	m_endDate = QDateTime(QDate(year, month, day), QTime(hour, minute, second));

	//ȡ��mac��ַ
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

	//�����������ֵ���ô���

	fclose(pFile);

	return true;
}

bool MainWindow::CheckLisence()
{
	auto bRes = OnBtnLisenceInfo();
	if (!bRes)
	{
		ShowMessageBox("���֤�ļ���ʧ������");
		return false;
	}

	//�Ƚ�mac��ַ
	m_macClient = m_macClient.toLower();
	m_macLisence = m_macLisence.toLower();
	if (0 != m_macClient.compare(m_macLisence))
	{
		ShowMessageBox("���֤��Ч");
		return false;
	}

	//�Ƚ���ʵ������ʱ��
	m_curDate = QDateTime(QDate(m_year, m_month, m_day), QTime(m_hour, m_minute, m_second));
	m_lisenceLeftSecond = m_curDate.secsTo(m_endDate);

	if (m_lisenceLeftSecond > 0)
	{
		auto leftDay = m_curDate.daysTo(m_endDate);
		if (leftDay > 1)
		{
			ShowMessageBox(std::string("������޻�ʣ��").append(std::to_string(leftDay)).append("��").c_str());
		}
		else
		{
			ShowMessageBox(std::string("������޻�ʣ��").append(std::to_string(m_lisenceLeftSecond / 3600)).append("Сʱ:").append(std::to_string(m_lisenceLeftSecond / 60)).append("��").c_str());
		}

		m_lisenceCheckTimer.connect(&m_lisenceCheckTimer, &QTimer::timeout, [&]() {
			m_lisenceLeftSecond -= 10;
			if (m_lisenceLeftSecond <= 0)
			{
				ShowMessageBox("���֤�ѹ���");
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
		ShowMessageBox("���֤�ѹ��ڣ�����ϵ����Ա�����µ����֤");
		return false;
	}
}

QString MainWindow::GetMAC()
{
	QString strMac = m_ui->edt_mac->text();
	if (strMac.length() != 12)
	{
		ShowMessageBox("MAC��ַ��Ч");
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
			AddTipInfo("��ȡ������");
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
	AddTipInfo("�����ļ��ɹ�");
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

	//���ն����ƶ�ȡ
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		AddTipInfo(std::string("��ȡ����ģ��[").append(strFilePath).append("]ʧ��").c_str());
		return;
	}

	//�ȶ�������������(����+str)
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

	//Ȼ���ȡ��������Ĵ�С�Լ�����
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
	AddTipInfo(std::string("��ȡģ��[").append(strFilePath).append("]�ɹ�������ȡ����").append(std::to_string(size)).append("��").c_str());
#endif

	//����������³�ʼ�����ڣ���Ϊ���ڿ����Ѿ��䶯
	InitGameWindow();
	//��Ϊ�����ʱ����ܱ����˸Ķ��ĳ�ʼflag�����Լ���ģ��ʱ�������б�Ƕ�����һ�£���ȷ������ʹ��
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

	//���ն����ƶ�ȡ
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		AddTipInfo(std::string("��ȡ����ģ��[").append(strFilePath).append("]ʧ��").c_str());
		return;
	}

	//Ȼ���ȡ��������Ĵ�С�Լ�����
	int size = 0;
	fread(&size, sizeof(int), 1, pFile);
	for (int i = 0; i < size; ++i)
	{
		InputData input;
		fread(&input, sizeof(InputData), 1, pFile);
		m_inputModuleVec.push_back(input);
	}

	fclose(pFile);

	//���±����ļ���ʾ�����ƣ��������ױ��渲�Ǵ�����Ϊ������ģ����ת����
	auto findPos = strFilePath.find_last_of("/");
	if (std::string::npos != findPos)
	{
		strFilePath = strFilePath.substr(findPos + 1);
	}

#ifdef DEV_VER
	m_ui->edt_saveName->setText(strFilePath.c_str());
	AddTipInfo(std::string("��ȡģ��[").append(strFilePath).append("]�ɹ�������ȡ����").append(std::to_string(size)).append("��").c_str());
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
		Q8("ע��"), Q8("����"), Q8("����"), Q8("vk"), "delay", 
		"x", "y", "xRate", "yRate",	"x2", "y2", "xRate2", "yRate2",
		Q8("��ʼʱ��"), Q8("��ɱ��"), Q8("��ʼ��ʼʱ����"),
		Q8("ͼƬ�Աȱ��"), "cmpPicRate", "picPath", Q8("����ͼƬ��ʱ"), Q8( "��ͼ��ʱ���" ),
		Q8("��ͼ�ɹ���ת"), Q8("��ͼ��ʱ��ת"), Q8("��ͼ�ɹ���תģ��"), Q8("��ͼ��ʱ��תģ��"), Q8("��ͼ���"),
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
				ShowMessageBox( "��������model��ȡ����" );
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
		//move��ʱ��Ĭ������������
		PostMessage(m_hGameWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
	break;
	default:
		AddTipInfo("����δ�����������...");
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
		AddTipInfo("����δ����ļ��̲���...");
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

	//���Ȳ���ָ������
	it->second.layerWnd[it->second.curLayer] = FindWindowA(nullptr, it->second.layerWndName[it->second.curLayer].toLocal8Bit().toStdString().c_str());
	//û���ҵ�ָ������
	if (nullptr == it->second.layerWnd[it->second.curLayer])
	{
		AddTipInfo(std::string("����ָ��ģ����[").append(it->second.layerWndName[it->second.curLayer].toLocal8Bit().toStdString()).append("]ʧ�ܣ���ʼ�Զ���������ģ����").c_str());

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
					//����֮ǰ�Ĳ�ֵ�����趨�����ڵĴ�С����Щģ���������׵����ֱ�������Ӵ��ڣ������ڻ�Ŵ󣬵���mumuֻ�����Ӵ��ڣ��������ǲ�����仯��
					//������Ϸ���崰�ڵ�ָ����С������ͼƬʶ��Ӧ�û�Ƚ�һ�£�֮ǰ���õ�����㴰�ڣ��ͻᵼ����Ϸ���崰�ڴ�С�в��
					simWndInfo.second.SetGameWndSize(m_wndWidth, m_wndHeight);

					AddTipInfo(std::string("�ҵ�ģ����[").append(simWndInfo.second.layerWndName[0].toLocal8Bit().toStdString()).append("]").c_str());
					break;
				}
			}
		}
	}
	//�ҵ�ָ������
	else
	{
// 		RECT rt;
// 		GetWindowRect(m_hWnd, &rt);
// 
// 		//����Ĵ�С���ò�Ҫ�ٸĶ��ˣ����ֻ�����������û�й�ϵ����Ҫ�漰��ͼƬ�Աȣ���Ȼ����һ��������ͼƬ̫С�������Ժ�ʼ�ջ�ʧ�棬��Ϊԭ�Ա�ͼƬ�Ĵ�С�Ǵ�890 588�ķֱ����Ͻ�ȡ��
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
			//����֮ǰ�Ĳ�ֵ�����趨�����ڵĴ�С����Щģ���������׵����ֱ�������Ӵ��ڣ������ڻ�Ŵ󣬵���mumuֻ�����Ӵ��ڣ��������ǲ�����仯��
			//������Ϸ���崰�ڵ�ָ����С������ͼƬʶ��Ӧ�û�Ƚ�һ�£�֮ǰ���õ�����㴰�ڣ��ͻᵼ����Ϸ���崰�ڴ�С�в��
			it->second.SetGameWndSize(m_wndWidth, m_wndHeight);

			AddTipInfo(std::string("�ҵ�ģ����[").append(it->second.layerWndName[0].toLocal8Bit().toStdString()).append("]").c_str());
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
	
	//n�㴰�ڿ������n-1��ı�������������Ϊ�ο�������1�������������Ӧ�ı������Ϳ������ô�С��
	double rateW[MAX_LAYER - 1] = { 0 };
	double rateH[MAX_LAYER - 1] = { 0 };
	rateW[0] = 1.0;
	rateH[0] = 1.0;

	int w = 0, h = 0, w2 = 0, h2 = 0;
	GetLayerSize(0, w, h);//��ȡ�������ڵĴ�С

	for (int i = 1; i < totalFindLayer; ++i)
	{
		GetLayerSize(i, w2, h2);//��ȡ�Ӵ��ڴ�С��������������ڵı��������Ϳ���һ���԰������������д���
		rateW[i] = (double)w / (double)w2;
		rateH[i] = (double)h / (double)h2;
	}

	//������㿪ʼ��������ô�С
	for (int i = 0; i < totalFindLayer; ++i)
	{
		::SetWindowPos(layerWnd[i], HWND_BOTTOM, 0, 0, (width * rateW[totalFindLayer - 1 - i]), (height * rateH[totalFindLayer - 1 - i]), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		InvalidateRect(layerWnd[i], nullptr, TRUE);
		Sleep(200);
	}
}
