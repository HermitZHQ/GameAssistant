#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTimer"
#include <windows.h>
#include <thread>
#include <vector>
#include "PreDef.h"
#include "Input.h"

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

private slots:
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
	void RefreshInputVecUIList();
	void OnBtnClearTipInfo();

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

	void InitGameWindow(const char *name = "");
	void UpdateGameWindowSize();

private:
    Ui::MainWindow *ui;
	HWND						m_hWnd;
	HWND						m_hChildWnd;
	bool						m_stopFlag;
	std::vector<InputData>		m_inputVec;
	CSize						m_gameWndSize;
	QString						m_gameWndParentName;
	QString						m_gameWndChildName;
	PicCompareStrategy			*m_picCompareStrategy;
	QTimer						m_timer;
};

#endif // MAINWINDOW_H
