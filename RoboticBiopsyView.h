
// RoboticBiopsyView.h : interface of the CRoboticBiopsyView class
//

#pragma once

#include "Dlg_Tools.h"
#include "ParameterDefinition.h"
#include <opencv2/core.hpp>

class CRoboticBiopsyView : public CView
{
protected: // create from serialization only
	CRoboticBiopsyView();
	DECLARE_DYNCREATE(CRoboticBiopsyView)

// Attributes
public:
	CRoboticBiopsyDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	// Implementation
public:

	Dlg_Tools m_dlg_tools;
	bool m_is_first_time_load_dlg;
	bool m_is_displaying_image;

	bool m_view_is_recording_video;
	CFont m_customFont;

	CRect CameraDisplayArea;

	virtual ~CRoboticBiopsyView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	cv::Point m_clicked_pos;
};

#ifndef _DEBUG  // debug version in RoboticBiopsyView.cpp
inline CRoboticBiopsyDoc* CRoboticBiopsyView::GetDocument() const
   { return reinterpret_cast<CRoboticBiopsyDoc*>(m_pDocument); }
#endif

