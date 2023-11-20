#pragma once
#include "afxwin.h"

// Dlg_Tools dialog


class Dlg_Tools : public CDialog
{
	DECLARE_DYNAMIC(Dlg_Tools)

public:
	Dlg_Tools(CWnd* pParent = NULL);	// standard constructor
	virtual ~Dlg_Tools();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
// Dialog Data
	enum { IDD = IDD_TOOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
DECLARE_MESSAGE_MAP()

public:

	CEdit m_help_string;

	CButton m_btn_left_x_left;
	CButton m_btn_left_x_right;
	CButton m_btn_left_y_up;
	CButton m_btn_left_y_down;
	CButton m_btn_left_z_up;
	CButton m_btn_left_z_down;
	CComboBox m_combo_left_step_size;
	CButton m_btn_right_x_left;
	CButton m_btn_right_x_right;
	CButton m_btn_right_y_up;
	CButton m_btn_right_y_down;
	CButton m_btn_right_z_up;
	CButton m_btn_right_z_down;
	CComboBox m_combo_right_step_size;

	afx_msg void OnBnClickedBtnRecordVideo();
	afx_msg void OnBnClickedBtnSaveImage();

	afx_msg void OnUpdateBnClickedManipLeft(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedBtnLeftXLeft();
	afx_msg void OnBnClickedBtnLeftXRight();
	afx_msg void OnBnClickedBtnLeftYUp();
	afx_msg void OnBnClickedBtnLeftYDown();
	afx_msg void OnBnClickedBtnLeftZUp();
	afx_msg void OnBnClickedBtnLeftZDown();

	afx_msg void OnUpdateBnClickedManipRight(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedBtnRightXLeft();
	afx_msg void OnBnClickedBtnRightXRight();
	afx_msg void OnBnClickedBtnRightYUp();
	afx_msg void OnBnClickedBtnRightYDown();
	afx_msg void OnBnClickedBtnRightZUp();
	afx_msg void OnBnClickedBtnRightZDown();
	CComboBox m_combo_focus_step_size;

	afx_msg void OnUpdateImageAccquired(CCmdUI* pCmdUI);
	CButton m_btn_record_video;
	CButton m_btn_save_image;

	CButton m_btn_aspirate;
	CButton m_btn_infuse;
	__int64 m_pump_steps;
	CButton m_pump_position_mode;
	bool m_pump_is_aspirate;
	afx_msg void OnBnClickedBtnAspirate();
	afx_msg void OnBnClickedBtnInfuse();
	afx_msg void OnUpdatePumpMotorControl(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedBtnPumpStop();


	afx_msg void OnBnClickedBtnPumpOn();
	

	CEdit m_helpStringCellNo;

	//afx_msg void OnEnChangeEdit1();
	//afx_msg void OnEnChangeEditHelpString();
	afx_msg void OnBnClickedBtnContactDetection();
	afx_msg void OnUpdateContactDetection(CCmdUI* pCmdUI);


	afx_msg void OnBnClickedButton1();
};
