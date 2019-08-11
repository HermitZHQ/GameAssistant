#include "PlayerUI.h"
#include "ui_player.h"
#include "mainwindow.h"
#include "PreDef.h"

PlayerUI::PlayerUI(MainWindow *wnd) :
	m_ui(new Ui::PlayerUI)
	, m_mainWnd(wnd)
	, m_specificLevelScriptMap({
		{Nefud_1E, QString(DEFAULT_PATH).append("zz_map_nefud-1e")},
		{WOLT, QString(DEFAULT_PATH).append("zz_map_fb_wolt")},
		{Evelyn_50, QString(DEFAULT_PATH).append("zz_map_fb_evelyn_50")},
		})

	, m_specificDelegateScriptMap({
		{Slowest, QString(DEFAULT_PATH).append("delegate_slowest")},
		})

	, m_specificDevScriptMap({
		{Limit, QString(DEFAULT_PATH).append("dev_limit")},
		{Normal10, QString(DEFAULT_PATH).append("dev_normal_10")},
		{Normal20, QString(DEFAULT_PATH).append("dev_normal_20")},
		})
{
	m_ui->setupUi(this);
}

PlayerUI::~PlayerUI()
{
	delete m_ui;
}

void PlayerUI::StopScritp()
{
	m_mainWnd->OnBtnStopClick();
	//设置指定的模拟器类型
	int index = m_ui->cmb_sim->currentIndex();
	m_mainWnd->SetSimWndType((SimWndType)index);
}

void PlayerUI::StartScript()
{
	m_mainWnd->OnBtnStartClick();
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
		m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_a4_reward").c_str());
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
	m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("zz_battle_findIcon_daily").c_str());
	StartScript();
}

void PlayerUI::OnBtnNextStep()
{
	StopScritp();
	m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("g").c_str());
	StartScript();
}

void PlayerUI::OnBtnDelegate()
{
	StopScritp();

	int index = m_ui->cmb_delegate->currentIndex();
	auto it = m_specificDelegateScriptMap.find((ZZ_Delegate)index);
	if (it != m_specificDelegateScriptMap.end())
	{
		m_mainWnd->LoadInputModuleFile(it->second.toStdString().c_str());
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
		m_mainWnd->LoadInputModuleFile(it->second.toStdString().c_str());
		StartScript();
	}
}

void PlayerUI::OnBtnRecruit()
{
	StopScritp();
	m_mainWnd->LoadInputModuleFile(std::string(DEFAULT_PATH).append("recruit").c_str());
	StartScript();
}

void PlayerUI::OnBtnSepcific()
{
	StopScritp();

	int index = m_ui->cmb_specific->currentIndex();
	auto it = m_specificLevelScriptMap.find((ZZ_Specific)index);
	if (it != m_specificLevelScriptMap.end())
	{
		m_mainWnd->LoadInputModuleFile(it->second.toStdString().c_str());
		StartScript();
	}
}
