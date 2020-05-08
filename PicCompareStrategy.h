#pragma once

#include "Input.h"
#include "PreDef.h"

namespace Ui {
	class MainWindow;
}

class MainWindow;
class PicCompareStrategy
{
public:
	PicCompareStrategy(MainWindow *mainWnd)
		:m_mainWnd(mainWnd), m_debugFlag(false)
	{}
	virtual ~PicCompareStrategy() {}
	virtual double HandlePicCompare(InputData &input, HWND gameWnd, const CSize &s) = 0;
	virtual void MouseClick(HWND gameWnd, int x, int y)
	{
		PostMessage(gameWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
		PostMessage(gameWnd, WM_LBUTTONUP, 0, MAKELONG(x, y));
	}
	inline void SetUi(Ui::MainWindow *ui) { m_ui = ui; }

	void SetDebugFlag(bool bFlag) {
		m_debugFlag = bFlag;
	}

	bool GetDebugFlg()const {
		return m_debugFlag;
	}

protected:
	Ui::MainWindow						*m_ui;
	MainWindow							*m_mainWnd;

private:
	bool								m_debugFlag;
};