
// RoboticBiopsyView.cpp : implementation of the CRoboticBiopsyView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "RoboticBiopsy.h"
#endif

#include "RoboticBiopsyDoc.h"
#include "RoboticBiopsyView.h"
#include "MainFrm.h"

#pragma comment(lib,"gdiplus.lib")
#include <gdiplus.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CRoboticBiopsyView

IMPLEMENT_DYNCREATE(CRoboticBiopsyView, CView)

BEGIN_MESSAGE_MAP(CRoboticBiopsyView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CRoboticBiopsyView construction/destruction

CRoboticBiopsyView::CRoboticBiopsyView()
{
	// TODO: add construction code here
	m_is_first_time_load_dlg = true;
	m_is_displaying_image = false;
	m_view_is_recording_video = false;

	m_customFont.CreatePointFont(240, ("Arial"));

	CameraDisplayArea.left = ClientLeft;
	CameraDisplayArea.top = ClientTop;
	CameraDisplayArea.right = ClientLeft + nImgWidth;
	CameraDisplayArea.bottom = ClientTop + nImgHeight;
}

CRoboticBiopsyView::~CRoboticBiopsyView()
{
}

BOOL CRoboticBiopsyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CRoboticBiopsyView drawing

void CRoboticBiopsyView::OnDraw(CDC* pDC)
{
	CRoboticBiopsyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nImgWidth, nImgHeight);
	CBitmap* pOldBitmap = memDC.SelectObject(&bmp);

	// Get pointer to mainframe
	CMainFrame* pMainFrm = (CMainFrame*)((AfxGetApp())->m_pMainWnd);

	if (!m_is_displaying_image)     // no image loaded or captured
	{
		CRect rect;
		GetClientRect(&rect);
		memDC.FillRect(&rect, &CBrush(BackGroundColor));
	}
	else
	{
		CRect rect;
		GetClientRect(&rect);
		memDC.FillRect(&rect, &CBrush(BackGroundColor));

		// Display main image
		pMainFrm->m_cs_display_color_mat.Lock();
		cv::Mat display_img = pMainFrm->m_displayColorImg.clone();
		pMainFrm->m_cs_display_color_mat.Unlock();

		if (!display_img.empty())
		{
			cv::Mat display_img_rgb;
			cv::cvtColor(display_img, display_img_rgb, cv::COLOR_BGR2RGB);

			BITMAPINFO bmpInfo;
			memset(&bmpInfo, 0, sizeof(BITMAPINFO));
			bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpInfo.bmiHeader.biWidth = display_img_rgb.cols;
			bmpInfo.bmiHeader.biHeight = -display_img_rgb.rows; // Negative height to flip the image
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = display_img_rgb.channels() * 8;
			bmpInfo.bmiHeader.biCompression = BI_RGB;

			StretchDIBits(memDC.GetSafeHdc(), CameraDisplayArea.left, CameraDisplayArea.top, CameraDisplayArea.Width(), CameraDisplayArea.Height(),
				0, 0, display_img_rgb.cols, display_img_rgb.rows,
				display_img_rgb.data, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
		}

		if (m_view_is_recording_video)
		{
			CRect Help_Video_Area(800, 5, 1180, 50);
			memDC.SelectObject(&m_customFont);
			memDC.SetTextColor(BlackColor);
			memDC.SetBkMode(TRANSPARENT);
			memDC.DrawText("Recording video...", &Help_Video_Area, DT_RIGHT);
		}
	}

	pDC->BitBlt(ClientLeft, ClientTop, nImgWidth, nImgHeight, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);

	if (m_is_first_time_load_dlg)
	{
		m_dlg_tools.Create(IDD_TOOLS, this);
		CRect dlg_rs(nImgWidth, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // Left top (x, y) Right bottom (x, y)
		m_dlg_tools.MoveWindow(&dlg_rs);
		m_dlg_tools.ShowWindow(true);

		m_is_first_time_load_dlg = false;
	}

	m_dlg_tools.UpdateDialogControls(&m_dlg_tools, FALSE);
}






// CRoboticBiopsyView diagnostics

#ifdef _DEBUG
void CRoboticBiopsyView::AssertValid() const
{
	CView::AssertValid();
}

void CRoboticBiopsyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRoboticBiopsyDoc* CRoboticBiopsyView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRoboticBiopsyDoc)));
	return (CRoboticBiopsyDoc*)m_pDocument;
}
#endif //_DEBUG


// CRoboticBiopsyView message handlers

// Erase backgroud to avoid updating backgroud
BOOL CRoboticBiopsyView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CRoboticBiopsyView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/*CRect m_xyPlan(ClientLeft, ClientTop, ClientLeft+nImgWidth, ClientTop+nImgHeight);
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	if (m_xyPlan.PtInRect(point)==1)
	{
		pMainFrm->OnLButtonDblClk(nFlags, point); 
	}*/
	AfxMessageBox(_T("double click happen."));
	m_clicked_pos = cv::Point(point.x, point.y);
	printf("DblClk_pos:(x=%d;y=%d)\n", m_clicked_pos.x, m_clicked_pos.y);
	CView::OnLButtonDblClk(nFlags, point);
}
