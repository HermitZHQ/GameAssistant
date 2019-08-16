#include "CaptureInputDataMgr.h"
#include "mainwindow.h"
#include <thread>
#include "PreDef.h"

CaptureInputDataMgr *CaptureInputDataMgr::m_singleton = nullptr;

CaptureInputDataMgr::CaptureInputDataMgr(MainWindow *mainWnd)
	:m_bStopFlag(false)
	, m_mainWnd(mainWnd)
	, m_bLMouseDown(false)
	, m_bLShiftDown(false)
	, m_bLAltDown(false)
	, m_bCtrlDown(false)
{
	m_pointList.clear();
}

CaptureInputDataMgr::~CaptureInputDataMgr()
{

}

CaptureInputDataMgr& CaptureInputDataMgr::Singleton(MainWindow *mainWnd /* = nullptr */)
{
	return (nullptr == m_singleton) ? *(m_singleton = new CaptureInputDataMgr(mainWnd)) : *(m_singleton);
}

CaptureInputDataMgr* CaptureInputDataMgr::SingletonPtr(MainWindow *mainWnd /* = nullptr */)
{
	return (nullptr == m_singleton) ? (m_singleton = new CaptureInputDataMgr(mainWnd)) : (m_singleton);
}

void CaptureInputDataMgr::BeginCapture()
{
// 	m_bStopFlag = false;
// 	std::thread t(&CaptureInputDataMgr::CaptureThreadUpdate, this);
// 	t.detach();
}

void CaptureInputDataMgr::StopCapture()
{
// 	m_bStopFlag = true;
}

void CaptureInputDataMgr::CaptureThreadUpdate()
{
	CaptureAndInsertPicRect();
	CaptureContinuousClickList();
	CaptureContinuousDragList();
}

void CaptureInputDataMgr::CaptureAndInsertPicRect()
{
	if ( GetKeyState( VK_LCONTROL ) & 0x8000 && !m_bCtrlDown )
	{
		m_bCtrlDown = true;
	}
	else if ( m_bCtrlDown && !( GetKeyState( VK_LCONTROL ) & 0x8000 ) )
	{
		m_bCtrlDown = false;
	}

	if ( m_bCtrlDown && !m_bLMouseDown && ( GetKeyState( VK_LBUTTON ) & 0x8000 ) )
	{
		m_bLMouseDown = true;
		HWND gameWnd = m_mainWnd->GetGameWnd();
		if ( nullptr == gameWnd )
		{
			return;
		}

		POINT pt;
		GetCursorPos( &pt );
		::ScreenToClient( gameWnd, &pt );
		m_x1 = pt.x;
		m_y1 = pt.y;
		m_mainWnd->AddTipInfo( std::string( "click x:" ).append( std::to_string( pt.x ) ).append( " y:" ).append( std::to_string( pt.y ) ).c_str() );
	}
	else if ( m_bCtrlDown && m_bLMouseDown )
	{
		if ( !( GetKeyState( VK_LBUTTON ) & 0x8000 ) )
		{
			m_bLMouseDown = false;

			HWND gameWnd = m_mainWnd->GetGameWnd();
			if ( nullptr == gameWnd )
			{
				return;
			}

			POINT pt;
			GetCursorPos( &pt );
			::ScreenToClient( gameWnd, &pt );
			m_x2 = pt.x;
			m_y2 = pt.y;
			m_mainWnd->AddTipInfo( std::string( "release x:" ).append( std::to_string( pt.x ) ).append( " y:" ).append( std::to_string( pt.y ) ).c_str() );

			m_mainWnd->InsertComparePicOperation( m_x1, m_y1, m_x2, m_y2 );
		}
	}
}

void CaptureInputDataMgr::CaptureContinuousClickList()
{
	if (GetKeyState(VK_LSHIFT) & 0x8000 && !m_bLShiftDown)
	{
		m_bLShiftDown = true;
		m_pointList.clear();
	}
	else if (!(GetKeyState(VK_LSHIFT) & 0x8000))
	{
		m_bLShiftDown = false;

		if ( m_pointList.size() )
		{
			//放开shift键后，一次性添加所有路径
			for (auto &pt : m_pointList)
			{
				int index = m_mainWnd->GetTableViewIndex();
				if (-1 == index)
					return;
	
				m_mainWnd->InsertClickOperation(pt.x(), pt.y());
				m_mainWnd->SetTableViewIndex(index + 1);
			}

			m_pointList.clear();
		}
	}

	if (m_bLShiftDown)
	{
		if (!m_bLMouseDown && (GetKeyState( VK_RBUTTON ) & 0x8000))
		{
			m_bLMouseDown = true;
			HWND gameWnd = m_mainWnd->GetGameWnd();

			POINT pt;
			GetCursorPos(&pt);
			::ScreenToClient(gameWnd, &pt);
			m_pointList.push_back(QPoint(pt.x, pt.y));
			m_mainWnd->AddTipInfo(std::string("add continuous(shift) x:").append(std::to_string(pt.x)).append(" y:").append(std::to_string(pt.y)).c_str());
		}
		else if (m_bLMouseDown && !(GetKeyState( VK_RBUTTON ) & 0x8000))
		{
			m_bLMouseDown = false;
		}
	}
}

void CaptureInputDataMgr::CaptureContinuousDragList()
{
	if (GetKeyState(VK_LMENU) & 0x8000 && !m_bLAltDown)
	{
		m_bLAltDown = true;
		m_dragPointList.clear();
	}
	else if (!(GetKeyState(VK_LMENU) & 0x8000))
	{
		m_bLAltDown = false;

		if ( m_dragPointList.size() > 1 )
		{
			//放开alt键后，一次性添加所有路径
			for (int i = 0; i < m_dragPointList.size() - 1; ++i)
			{
				int index = m_mainWnd->GetTableViewIndex();
				if (-1 == index)
					return;
	
				m_mainWnd->InsertDragOperation(m_dragPointList[i].x(), m_dragPointList[i].y(), m_dragPointList[i + 1].x(), m_dragPointList[i + 1].y());
				m_mainWnd->SetTableViewIndex(index + 3);
			}
	
			m_dragPointList.clear();
		}
	}

	if (m_bLAltDown)
	{
		if (!m_bLMouseDown && (GetKeyState( VK_RBUTTON ) & 0x8000))
		{
			m_bLMouseDown = true;
			HWND gameWnd = m_mainWnd->GetGameWnd();

			POINT pt;
			GetCursorPos(&pt);
			::ScreenToClient(gameWnd, &pt);
			m_dragPointList.push_back(QPoint(pt.x, pt.y));
			m_mainWnd->AddTipInfo(std::string("add continuous(alt) x:").append(std::to_string(pt.x)).append(" y:").append(std::to_string(pt.y)).c_str());
		}
		else if (m_bLMouseDown && !(GetKeyState( VK_RBUTTON ) & 0x8000))
		{
			m_bLMouseDown = false;
		}
	}
}
