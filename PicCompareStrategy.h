#pragma once

#include "Input.h"
#include "PreDef.h"

namespace Ui {
	class MainWindow;
}

class PicCompareStrategy
{
public:
	PicCompareStrategy() {}
	virtual ~PicCompareStrategy() {}
	virtual double HandlePicCompare(InputData &input, HWND gameWnd, const CSize &s) = 0;
	virtual void MouseClick(HWND gameWnd, int x, int y)
	{
		PostMessage(gameWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
		PostMessage(gameWnd, WM_LBUTTONUP, 0, MAKELONG(x, y));
	}
	inline void SetUi(Ui::MainWindow *ui) { m_ui = ui; }

protected:
	Ui::MainWindow						*m_ui;

private:
};