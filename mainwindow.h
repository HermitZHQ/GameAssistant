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
#include "QMenu"

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
	std::vector<bool>	bSetSizeFlagVec;//有些窗口不需要设置最外层就能自动调整最外层，设置了最外层反而会造成旁边的黑边
	HWND				layerWnd[MAX_LAYER];
	HWND				gameWnd;//最终指定的游戏窗口，把这个窗口设置成统一的大小，脚本就可以正常工作了
	HWND				parentWnd;//最外层窗口，用于设置bottom???
	RECT				rt[MAX_LAYER];
	unsigned short		curLayer;
	unsigned short		gameWndLayer;//指定第几层是游戏消息窗口
	unsigned short		totalFindLayer;//一共要找几层，比如目前的mumu和雷电都是2层

	//最新改为默认不支持2层了，你需要找到几层必须明确写清楚，因为现在雷电和mumu的窗口设置大小机制问题
	//mumu设置第二层大小可以完全达到指定大小，并且不会反弹，而mumu设置后会有一点黑边，导致分辨率略小的问题
	SimWndInfo(const QString &layer1WndName, std::vector<bool> setSizeFlagVec, unsigned short _totalFindLayer, unsigned short _gameWndLayer)
		:gameWnd(nullptr), curLayer(0), totalFindLayer(_totalFindLayer), gameWndLayer(_gameWndLayer)

	{
		bSetSizeFlagVec = setSizeFlagVec;
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
			gameWnd = layerWnd[gameWndLayer - 1];
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

	static BOOL CALLBACK EnumChildProc(_In_ HWND hwnd, _In_ LPARAM lParam);
	const inline HWND GetGameWnd() {
		return m_hGameWnd;
	}
	int GetTableViewIndex();
	void SetTableViewIndex(int index);
	void CaptureUpdate();
	void InsertComparePicOperation(int x, int y, int x2, int y2);
	void InsertClickOperation(int x, int y);
	void InsertDragOperation(int x, int y, int x2, int y2);

	PlayerUI& GetPlayerUI() {
		return m_playerUI;
	}

public slots:
	void PostMsgThread(int cmpParam = -1);

	void OnBtnStartClick();
	void OnBtnStopClick();

	void OnBtnAddInput();
	void GetInputData(InputData &input);
	//主要用于更新xy坐标的百分比
	void UpdateInputData(InputData &input);

	void OnBtnDelAllInput();
	void OnBtnUpdateAllInput();
	void OnTableViewClick();
	void UpdateInputDataUI(int index);
	void UpdateSelectInputData( int index );
	void OnBtnInsertInputClick();
	void InsertInputData(int index);
	void OnBtnGetBattleTemplate();
	void OnBtnGetBattleTemplate2();
	void OnBtnStartTimeCount();
	void OnBtnEndTimeCount();
	void OnBtnClearTipInfo();

	void ShowMessageBox(const char *content);
	bool ShowConfirmBox(const char *str);
	void AddTipInfo(const char *str, bool bConvertFlag = true);

	//lisence
	void OnBtnLisence();
	bool OnBtnLisenceInfo();
	bool CheckLisence();
	QString GetMAC();

	//table view(model-view-delegate)
	void InitTableView();
	void TableViewUpdateDelay();
	void TableViewCopy();
	void TableViewPaste();
	void TableViewCopyInput();
	void TableViewInsertCopyInput();
	void TableViewInsertCopyInputDown();
	void TableViewPasteOverwriteInput();
	void TableViewDel();
	void TableViewUpdateSingleView();
	void TableViewJump();

	void OnBtnOpenFileDialog();
	void OnBtnOpenFileDialog_PicPath();
	void OnBtnSaveClick();
	void LoadScriptModuleFile(const char *file);
	void LoadScriptModuleFileToSpecificInputVec(const char *file, std::vector<InputData> &inputVec);
	void SetInputDataModel();
	void GetInputDataFromModel(int row, int col);

	//重置所有命令的标识
	void ResetAllInputFinishFlag(std::vector<InputData> &inputVec);
	//命令跳转，重置跳转索引之后的标识，完成索引之前的标识设置
	void JumpInput(int index, std::vector<InputData> &inputVec);

	void HandleMouseInput(InputData &input);
	void HandleKeyboardInput(InputData &input);
	void HandleGameImgCompare(InputData &input);

	void InitGameWindow();
	void UpdateGameWindowSize();
	void CheckGameWndSize();
	void ResetSimWndInfo();
	void SetSimWndType(SimWndType type);
	void OnReturnPressedResolutionRateX();
	void OnReturnPressedResolutionRateY();

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
	QTimer						m_captureUpdateTimer;
	QTimer						m_checkGameWndSizeTimer;
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
	QMenu						m_menu;
	QList<QVariant>				m_copyList;
	std::vector<InputData>		m_copyInputVec;

	DWORD						m_waitTime;
	QTimer						m_timeCountTimer;
};

#endif // MAINWINDOW_H
