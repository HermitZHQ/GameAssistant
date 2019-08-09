#pragma once
#include "QWidget"

namespace Ui {
	class PlayerUI;
}

class MainWindow;
class PlayerUI : public QWidget
{
	Q_OBJECT
public:
	PlayerUI(MainWindow *wnd);
	~PlayerUI();

private slots:
	void OnBtnStop();
	void OnBtnA4Reward();

private:
	Ui::PlayerUI						*m_ui;
	MainWindow							*m_mainWnd;
};

