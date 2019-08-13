#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTimer"
#include "QDateTime"
#include <windows.h>
// #include <thread>
#include <vector>
#include "PreDef.h"
#include "Input.h"
#include "PlayerUI.h"
#include "BkgUI.h"
//model view
#include "InputDataModel.h"
#include "ItemDelegate.h"

namespace Ui {
class MainWindow;
}

//模拟器窗口相关信息，辅助找到内层窗口
enum SimWndType
{
	Thunder,//雷电
	MuMu,//木木

	None,
};
//目前支持3层嵌套
#define MAX_LAYER (3)
struct SimWndInfo
{

	QString				layerWndName[MAX_LAYER];
	bool				bUseLayerNameFlag[MAX_LAYER];
	HWND				layerWnd[MAX_LAYER];
	HWND				gameWnd;//最终指定的游戏窗口，把这个窗口设置成统一的大小，脚本就可以正常工作了
	HWND				parentWnd;//最外层窗口，用于设置bottom???
	RECT				rt[MAX_LAYER];
	unsigned short		curLayer;
	unsigned short		totalFindLayer;//一共要找几层，比如目前的mumu和雷电都是2层

	//默认找2层，且不用查找第二层名字的构造，如果以后还有其他模拟器支持再开发其他构造
	SimWndInfo(const QString &layer1WndName, unsigned short _totalFindLayer = 2)
		:gameWnd(nullptr), curLayer(0), totalFindLayer(_totalFindLayer)
	{
		for (int i = 0; i < MAX_LAYER; ++i)
		{
			if (0 == i)
			{
				layerWndName[i] = layer1WndName;
				bUseLayerNameFlag[i] = true;
			}
			else
			{
				layerWndName[i] = "....xxxx";//不能使用空，会找到桌面的句柄
				bUseLayerNameFlag[i] = false;
			}

			layerWnd[i] = nullptr;
			ZeroMemory(rt, sizeof(rt));
		}
	}
	//total是真实层数，curLayer是数组下标，所以减一
	const inline bool CheckFindFinishe() { 
		bool bRes = (curLayer == (totalFindLayer - 1));
		if (bRes)
		{
			gameWnd = layerWnd[curLayer];
			parentWnd = layerWnd[0];
		}
		return bRes;
	}

	const inline unsigned short AddLayer() {
		return ++curLayer;
	}

	const inline void UpdateRect() {
		for (int i = 0; i < totalFindLayer; ++i)
		{
			GetWindowRect(layerWnd[i], &rt[i]);
		}
	}

	const inline void GetLayerSize(unsigned short layer, int &w, int &h) {
		w = rt[layer].right - rt[layer].left;
		h = rt[layer].bottom - rt[layer].top;
	}

	//应该在查找完毕后，再使用此函数
	void SetGameWndSize(int width, int height);

	//检测游戏本体窗口是否已经设置到指定大小，没有的话，可以在循环中调整
	const inline void CheckGameWndSize(int w, int h) {
		if (nullptr == gameWnd)
			return;

		RECT rt;
		GetWindowRect(gameWnd, &rt);
		if ((w != (rt.right - rt.left))
			|| (h != (rt.bottom - rt.top)))
		{
			SetGameWndSize(w, h);
		}
	}

private:
};

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
	//主要用于更新xy坐标的百分比
	void UpdateInputData(InputData &input);

	void OnBtnDelLastInput();
	void OnBtnDelAllInput();
	void OnBtnUpdateAllInput();
	void OnBtnDelSelectInputClick();
	void OnBtnDel3Inputs();
	void OnBtnInputListClick();
	void OnBtnUpdateSelectInputClick();
	void OnBtnInsertInputClick();
	void OnBtnInsertDrag();
	void RefreshInputVecUIList();
	void RefreshInputModuleVecUIList();
	void OnBtnClearTipInfo();
	void OnBtnOverwrite();
	void OnBtnOverwriteDelay();
	void OnBtnSetOverwriteTargetIndex();

	void ShowMessageBox(const char *content);
	void AddTipInfo(const char *str, bool bConvertFlag = true);

	//list module
	void OnBtnShowHide();
	void OnBtnGetModule();
	void OnBtnInsertModule();
	void OnBtnSaveModule();
	void OnBtnLoadModule();
	void OnBtnDelSelectModuleInput();
	void OnBtnModuleListClick();
	void OnBtnClearModuleInput();
	void OnBtnUpdateSelectModuleInput();

	//lisence
	void OnBtnLisence();
	bool OnBtnLisenceInfo();
	bool CheckLisence();
	QString GetMAC();

	void OnBtnOpenFileDialog();
	void OnBtnOpenFileDialog_PicPath();
	void OnBtnSaveClick();
	void OnBtnLoadClick();
	void LoadScriptModuleFile(const char *file);
	void LoadModuleFile(const char *file);
	void SetInputDataModel();
	void GetInputDataModel();

	//重置所有命令的标识
	void ResetAllInputFinishFlag();
	//命令跳转，重置跳转索引之后的标识，完成索引之前的标识设置
	void JumpInput(int index);

	void HandleMouseInput(InputData &input);
	void HandleKeyboardInput(InputData &input);
	void HandleGameImgCompare(InputData &input);

	void InitGameWindow();
	void UpdateGameWindowSize();
	void ResetSimWndInfo();
	void SetSimWndType(SimWndType type);

private:
	Ui::MainWindow				*m_ui;
	PlayerUI					m_playerUI;
	BkgUI						m_bkgUI;
	typedef std::unordered_map<SimWndType, SimWndInfo> SimWndInfoMap;
	SimWndInfoMap				m_simWndInfoMap;
	SimWndType					m_simWndType;
	HWND						m_hGameWnd;
	HWND						m_hParentWnd;
	SimWndInfo					m_curSimWndInfo;

	bool						m_stopFlag;
	std::vector<InputData>		m_inputVec;
	std::vector<InputData>		m_inputModuleVec;
	CSize						m_gameWndSize;
	int							m_wndWidth;
	int							m_wndHeight;
	PicCompareStrategy			*m_picCompareStrategy;
	QTimer						m_timer;

	QTimer						m_lisenceCheckTimer;
	QTimer						m_delayTimer;
	long						m_year;
	long						m_month;
	long						m_day;
	long						m_hour;
	long						m_minute;
	long						m_second;
	QString						m_macClient;
	QString						m_macLisence;
	QDateTime					m_curDate;
	QDateTime					m_endDate;
	LONGLONG					m_lisenceLeftSecond;

	//list module
	bool						m_bShowHideFlag;

	//model view relevant
	InputDataModel				m_inputDataModel;
	ItemDelegate				m_itemDelegate;
};

#endif // MAINWINDOW_H
