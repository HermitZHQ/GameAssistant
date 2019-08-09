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
	m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_a4_reward").c_str());
	m_mainWnd->OnBtnStartClick();
}
