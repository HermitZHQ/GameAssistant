#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTimer"
#include <windows.h>
#include <thread>
#include <vector>
#include "PreDef.h"
#include "Input.h"
#include "PlayerUI.h"
#include "BkgUI.h"

#define DEV_VER

namespace Ui {
class MainWindow;
}

class PicCompareStrategy;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	static BOOL CALLBACK EnumChildProc(_In_ HWND hwnd,_In_ LPARAM lParam);

public slots:
	void PostMsgThread();

	void OnBtnStartClick();
	void OnBtnStopClick();

	void OnBtnAddInput();
	void GetInputData(InputData &input);
	//��Ҫ���ڸ���xy����İٷֱ�
	void UpdateInputData(InputData &input);

	void OnBtnDelLastInput();
	void OnBtnDelAllInput();
	void OnBtnUpdateAllInput();
	void OnBtnDelSelectInputClick();
	void OnBtnInputListClick();
	void OnBtnUpdateSelectInputClick();
	void OnBtnInsertInputClick();
	void OnBtnInsertDrag();
	void RefreshInputVecUIList();
	void OnBtnClearTipInfo();
	void OnBtnOverwrite();
	void OnBtnSetOverwriteTargetIndex();

	void ShowMessageBox(const char *content);

	void OnBtnOpenFileDialog();
	void OnBtnOpenFileDialog_PicPath();
	void OnBtnSaveClick();
	void OnBtnLoadClick();
	void LoadInputModuleFile(const char *file);

	//������������ı�ʶ
	void ResetAllInputFinishFlag();
	//������ת��������ת����֮��ı�ʶ���������֮ǰ�ı�ʶ����
	void JumpInput(int index);

	void HandleMouseInput(InputData &input);
	void HandleKeyboardInput(InputData &input);
	void HandleGameImgCompare(InputData &input);

	void InitGameWindow();
	void UpdateGameWindowSize();

private:
	Ui::MainWindow				*m_ui;
	PlayerUI					m_playerUI;
	BkgUI						m_bkgUI;

	HWND						m_hWnd;
	HWND						m_hChildWnd;
	bool						m_stopFlag;
	std::vector<InputData>		m_inputVec;
	CSize						m_gameWndSize;
	QString						m_gameWndParentName;
	QString						m_gameWndChildName;
	int							m_wndWidth;
	int							m_wndHeight;
	PicCompareStrategy			*m_picCompareStrategy;
	QTimer						m_timer;
};

#endif // MAINWINDOW_H
