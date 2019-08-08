#pragma once
#include "QWidget"

namespace Ui {
	class BkgUI;
}

class BkgUI : public QWidget
{
	Q_OBJECT
public:
	BkgUI();
	~BkgUI();

private slots:

private:
	Ui::BkgUI							*m_ui;
};