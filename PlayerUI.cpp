#include "PlayerUI.h"
#include "ui_player.h"
#include "mainwindow.h"
#include "PreDef.h"

PlayerUI::PlayerUI(MainWindow *wnd) :
	m_ui(new Ui::PlayerUI)
	, m_mainWnd(wnd)
{
	m_ui->setupUi(this);
}

PlayerUI::~PlayerUI()
{
	delete m_ui;
}

void PlayerUI::OnBtnStop()
{
	m_mainWnd->OnBtnStopClick();
}

void PlayerUI::OnBtnA4Reward()
{
	m_mainWnd->OnBtnStopClick();

	int index = m_ui->cmb_delegate->currentIndex();
	switch (index)
	{
	case A4:
	{
		m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_a4_reward").c_str());
	}
		break;
	default:
		break;
	}

	m_mainWnd->OnBtnStartClick();
}

void PlayerUI::OnBtnDaily()
{
	m_mainWnd->OnBtnStopClick();
	m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_daily").c_str());
	m_mainWnd->OnBtnStartClick();
}

void PlayerUI::OnBtnNextStep()
{
	m_mainWnd->OnBtnStopClick();
	m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("g").c_str());
	m_mainWnd->OnBtnStartClick();
}

void PlayerUI::OnBtnDelegate()
{
	m_mainWnd->OnBtnStopClick();

	int index = m_ui->cmb_delegate->currentIndex();
	switch (index)
	{
	case A4:
	{
		m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("delegate_slowest").c_str());
	}
	break;
	default:
		break;
	}

	m_mainWnd->OnBtnStartClick();
}
