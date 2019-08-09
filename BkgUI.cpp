#include "BkgUI.h"
#include "ui_bkg.h"

BkgUI::BkgUI()
	:m_ui(new Ui::BkgUI)
{
}

BkgUI::~BkgUI()
{
	delete m_ui;
}
