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

//ģ�������������Ϣ�������ҵ��ڲ㴰��
enum SimWndType
{
	Thunder,//�׵�
	MuMu,//ľľ

	None,
};
//Ŀǰ֧��3��Ƕ��
#define MAX_LAYER (3)
struct SimWndInfo
{

	QString				layerWndName[MAX_LAYER];
	bool				bUseLayerNameFlag[MAX_LAYER];
	std::vector<bool>	bSetSizeFlagVec;//��Щ���ڲ���Ҫ�������������Զ���������㣬����������㷴��������Աߵĺڱ�
	HWND				layerWnd[MAX_LAYER];
	HWND				gameWnd;//����ָ������Ϸ���ڣ�������������ó�ͳһ�Ĵ�С���ű��Ϳ�������������
	HWND				parentWnd;//����㴰�ڣ���������bottom???
	RECT				rt[MAX_LAYER];
	unsigned short		curLayer;
	unsigned short		gameWndLayer;//ָ���ڼ�������Ϸ��Ϣ����
	unsigned short		totalFindLayer;//һ��Ҫ�Ҽ��㣬����Ŀǰ��mumu���׵綼��2��

	//���¸�ΪĬ�ϲ�֧��2���ˣ�����Ҫ�ҵ����������ȷд�������Ϊ�����׵��mumu�Ĵ������ô�С��������
	//mumu���õڶ����С������ȫ�ﵽָ����С�����Ҳ��ᷴ������mumu���ú����һ��ڱߣ����·ֱ�����С������
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
				layerWndName[i] = "....xxxx";//����ʹ�ÿգ����ҵ�����ľ��
				bUseLayerNameFlag[i] = false;
			}

			layerWnd[i] = nullptr;
			ZeroMemory(rt, sizeof(rt));
		}
	}
	//total����ʵ������curLayer�������±꣬���Լ�һ
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

	//Ӧ���ڲ�����Ϻ���ʹ�ô˺���
	void SetGameWndSize(int width, int height);

	//�����Ϸ���崰���Ƿ��Ѿ����õ�ָ����С��û�еĻ���������ѭ���е���
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
	//��Ҫ���ڸ���xy����İٷֱ�
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

	//������������ı�ʶ
	void ResetAllInputFinishFlag(std::vector<InputData> &inputVec);
	//������ת��������ת����֮��ı�ʶ���������֮ǰ�ı�ʶ����
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
