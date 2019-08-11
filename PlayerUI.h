#pragma once
#include "QWidget"
#include "QString"
#include "PreDef.h"

namespace Ui {
	class PlayerUI;
}

enum ZZ_Reward
{
	A4
};

enum ZZ_Delegate
{
	Slowest
};

enum ZZ_Specific
{
	Nefud_1E,
	WOLT,//薇欧蕾特副本
	Evelyn_50,//伊芙琳50级副本
};

enum ZZ_Dev
{
	Limit,
	Normal10,
	Normal20,
};

class MainWindow;
class PlayerUI : public QWidget
{
	Q_OBJECT
public:
	PlayerUI(MainWindow *wnd);
	~PlayerUI();

	const inline Ui::PlayerUI* GetUI() { return m_ui; }

protected:
	void StopScritp();
	void StartScript();

private slots:
	void OnBtnStop();
	void OnBtnA4Reward();
	void OnBtnDaily();
	void OnBtnNextStep();
	void OnBtnDelegate();
	void OnBtnDev();
	void OnBtnRecruit();
	void OnBtnSepcific();

private:
	Ui::PlayerUI									*m_ui;
	MainWindow										*m_mainWnd;
	std::unordered_map<ZZ_Specific, QString>		m_specificLevelScriptMap;
	std::unordered_map<ZZ_Delegate, QString>		m_specificDelegateScriptMap;
	std::unordered_map<ZZ_Dev, QString>				m_specificDevScriptMap;
};

