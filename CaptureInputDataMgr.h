#pragma once
#include "QPoint"
#include "QList"

class MainWindow;
class CaptureInputDataMgr
{
private:
	CaptureInputDataMgr(MainWindow *mainWnd);

public:
	~CaptureInputDataMgr();
	static CaptureInputDataMgr& Singleton(MainWindow *mainWnd = nullptr);
	static CaptureInputDataMgr* SingletonPtr(MainWindow *mainWnd = nullptr);

	void BeginCapture();
	void StopCapture();
	void CaptureThreadUpdate();
	void CaptureAndInsertPicRect();
	void CaptureContinuousClickList();
	void CaptureContinuousDragList();

private:
	static CaptureInputDataMgr						*m_singleton;
	bool											m_bStopFlag;
	MainWindow										*m_mainWnd;

	//capture coordinate
	int												m_x1;
	int												m_y1;
	int												m_x2;
	int												m_y2;
	//capture continuous route points
	QList<QPoint>									m_pointList;
	QList<QPoint>									m_dragPointList;
	//capture flags
	bool											m_bLMouseDown;//for pic/route cap

	bool											m_bCtrlDown;
	bool											m_bLShiftDown;//for continuous route click point insert
	bool											m_bLAltDown;//for continuous route drag point insert
};
