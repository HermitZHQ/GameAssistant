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
// 	ui->list_tip->addItem(QString::fromLocal8Bit("Ͷ����Ϣ�߳��ѿ�ʼ����..."));
	if (m_inputVec.size() == 0)
	{
		m_timer.stop();
		ui->list_tip->addItem(QString::fromLocal8Bit("�ű���������Ϊ0���߳��˳�..."));
		return;
	}

	//ִ�нű�ʱΪ˳��ִ�У�ÿִ����һ��finishflag�ͱ�ʶΪtrue��������Ժ��������б�ʶ�����з���ѭ��
// 	while (m_hWnd && !m_stopFlag)
// 	{
		//��ȡ���µ���Ϸ���ڴ�С��ͨ����������������������Ա�֤���������С���ܵ����ȷ
		UpdateGameWindowSize();
		int iCurCount = ui->list_tip->count();
		static int iLastCount = iCurCount;
		//�����ʾ�б��С������һ��ֵ���
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
	ui->list_tip->addItem(QString::fromLocal8Bit("��ʼ�ű�����..."));
}

void MainWindow::OnBtnStopClick()
{
	m_stopFlag = true;
	m_timer.stop();
	ui->list_tip->addItem(QString::fromLocal8Bit("ֹͣ�ű�����..."));
}

void MainWindow::OnBtnAddInput()
{
	//���ǰ�ȸ�����Ϸ���ڴ�С
	UpdateGameWindowSize();

	int repeatTime = ui->edt_repeat->text().toShort();

	InputData input;
	GetInputData(input);

	for (int i = 0; i < repeatTime; ++i)
	{
		m_inputVec.push_back(input);
	}

	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("�����").append(std::to_string(repeatTime)).append("��ָ��").c_str()));

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

	ui->list_tip->addItem(QString::fromLocal8Bit("��ɾ����һ��ָ��"));
	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("��ʣ��").append(std::to_string(m_inputVec.size())).append("��ָ��").c_str()));

	RefreshInputVecUIList();
}

void MainWindow::OnBtnDelAllInput()
{
	m_inputVec.clear();
	ui->list_tip->addItem(QString::fromLocal8Bit("��ɾ������ָ��"));

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
	ui->list_tip->addItem(QString::fromLocal8Bit("�Ѹ�������ָ��"));
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

		//�ѵ�ǰinput����ѯ��ʾ��editbox��
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

	//���ն����ƴ洢
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

	//��д������������(����+str)
	int nameLen = (int)ui->edt_wndName->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(ui->edt_wndName->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

	nameLen = (int)ui->edt_wndName2->text().toLocal8Bit().toStdString().length() + 1;
	fwrite(&nameLen, sizeof(int), 1, pFile);
	fwrite(ui->edt_wndName2->text().toLocal8Bit().toStdString().c_str(), 1, nameLen, pFile);

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
	ui->list_tip->addItem(QString::fromLocal8Bit("�����ļ��ɹ�"));
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

	//���ն����ƶ�ȡ
	FILE *pFile = nullptr;
	fopen_s(&pFile, strFilePath.c_str(), "rb");
	if (nullptr == pFile)
	{
		RefreshInputVecUIList();
		ui->list_tip->addItem(QString::fromLocal8Bit(std::string("��ȡ����ģ��[").append(strFilePath).append("]ʧ��").c_str()));
		return;
	}

	//��д������������(����+str)
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
	ui->edt_wndWidth->setText(std::to_string(m_wndWidth).c_str());
	ui->edt_wndHeight->setText(std::to_string(m_wndHeight).c_str());

	fclose(pFile);

	//���±����ļ���ʾ�����ƣ��������ױ��渲�Ǵ�����Ϊ������ģ����ת����
	auto findPos = strFilePath.find_last_of("/");
	if (std::string::npos != findPos)
	{
		strFilePath = strFilePath.substr(findPos + 1);
	}

	ui->edt_saveName->setText(strFilePath.c_str());
	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("��ȡģ��[").append(strFilePath).append("]�ɹ�������ȡ����").append(std::to_string(size)).append("��").c_str()));

	//����������³�ʼ�����ڣ���Ϊ���ڿ����Ѿ��䶯
	InitGameWindow();
	//��Ϊ�����ʱ����ܱ����˸Ķ��ĳ�ʼflag�����Լ���ģ��ʱ�������б�Ƕ�����һ�£���ȷ������ʹ��
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
		//move��ʱ��Ĭ������������
		PostMessage(m_hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(m_gameWndSize.x * input.xRate, m_gameWndSize.y * input.yRate));
	}
		break;
	default:
		ui->list_tip->addItem(QString::fromLocal8Bit("����δ�����������..."));
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
		ui->list_tip->addItem(QString::fromLocal8Bit("����δ����ļ��̲���..."));
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

// 	ui->list_tip->addItem(QString::fromLocal8Bit("��ʼ��ʼ����Ϸ����..."));
// 	ui->list_tip->addItem(QString::fromLocal8Bit(std::string("�������ƣ�").append(ui->edt_wndName->text().toLocal8Bit().toStdString()).c_str()));

	m_hWnd = FindWindowA(nullptr, m_gameWndParentName.toLocal8Bit().toStdString().c_str());
	if (nullptr == m_hWnd)
	{
		ui->list_tip->addItem(QString::fromLocal8Bit("���Ҵ��ھ��ʧ�ܣ���ʼ����Ϸ����ʧ��"));
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
				ui->list_tip->addItem(QString::fromLocal8Bit("�����Ӵ���ʧ��"));
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
			ui->list_tip->addItem(QString::fromLocal8Bit("��ʼ����Ϸ���ڳɹ�"));
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