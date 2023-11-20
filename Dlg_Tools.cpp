// Dlg_Tools.cpp : implementation file
//

#include "stdafx.h"
#include "RoboticBiopsy.h"
#include "Dlg_Tools.h"
#include "afxdialogex.h"
#include <iostream>

//***************Create Link to MainFrame (Start)***********
#include "MainFrm.h"
//***************Create Link to MainFrame (End)*************

// Dlg_Tools dialog

IMPLEMENT_DYNAMIC(Dlg_Tools, CDialog)

Dlg_Tools::Dlg_Tools(CWnd* pParent /*=NULL*/)
	: CDialog(Dlg_Tools::IDD, pParent)
	, m_pump_steps(500)
{
	
}

Dlg_Tools::~Dlg_Tools()
{

}

void Dlg_Tools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_LEFT_X_LEFT, m_btn_left_x_left);
	DDX_Control(pDX, IDC_BTN_LEFT_X_RIGHT, m_btn_left_x_right);
	DDX_Control(pDX, IDC_BTN_LEFT_Y_UP, m_btn_left_y_up);
	DDX_Control(pDX, IDC_BTN_LEFT_Y_DOWN, m_btn_left_y_down);
	DDX_Control(pDX, IDC_BTN_LEFT_Z_UP, m_btn_left_z_up);
	DDX_Control(pDX, IDC_BTN_LEFT_Z_DOWN, m_btn_left_z_down);
	DDX_Control(pDX, IDC_COMBO1, m_combo_left_step_size);
	DDX_Control(pDX, IDC_BTN_RIGHT_X_LEFT, m_btn_right_x_left);
	DDX_Control(pDX, IDC_BTN_RIGHT_X_RIGHT, m_btn_right_x_right);
	DDX_Control(pDX, IDC_BTN_RIGHT_Y_UP, m_btn_right_y_up);
	DDX_Control(pDX, IDC_BTN_RIGHT_Y_DOWN, m_btn_right_y_down);
	DDX_Control(pDX, IDC_BTN_RIGHT_Z_UP, m_btn_right_z_up);
	DDX_Control(pDX, IDC_BTN_RIGHT_Z_DOWN, m_btn_right_z_down);
	DDX_Control(pDX, IDC_COMBO2, m_combo_right_step_size);
	DDX_Control(pDX, IDC_BTN_RECORD_VIDEO, m_btn_record_video);
	DDX_Control(pDX, IDC_BTN_SAVE_IMAGE, m_btn_save_image);
	DDX_Control(pDX, IDC_BTN_ASPIRATE, m_btn_aspirate);
	DDX_Control(pDX, IDC_BTN_INFUSE, m_btn_infuse);
	DDX_Control(pDX, IDC_RADIO_POSITION, m_pump_position_mode);
	DDX_Text(pDX, IDC_EDIT1, m_pump_steps);
	DDX_Control(pDX, IDC_EDIT_HELP_STRING, m_help_string);
	DDX_Control(pDX, IDC_EDIT2, m_helpStringCellNo);
	DDX_Control(pDX, IDC_COMBO3, m_combo_focus_step_size);
}

BOOL Dlg_Tools::OnInitDialog()
{
	CDialog::OnInitDialog();

	/***********************Set arrow icons (start) *****************/
	HICON hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 48, 48, LR_SHARED);
	m_btn_left_x_left.SetIcon(hIcon);
	m_btn_right_x_left.SetIcon(hIcon);

	hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 48, 48, LR_SHARED);
	m_btn_left_y_up.SetIcon(hIcon);
	m_btn_left_z_up.SetIcon(hIcon);
	m_btn_right_y_up.SetIcon(hIcon);
	m_btn_right_z_up.SetIcon(hIcon);

	hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON3), IMAGE_ICON, 48, 48, LR_SHARED);
	m_btn_left_y_down.SetIcon(hIcon);
	m_btn_left_z_down.SetIcon(hIcon);
	m_btn_right_y_down.SetIcon(hIcon);
	m_btn_right_z_down.SetIcon(hIcon);

	hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON4), IMAGE_ICON, 48, 48, LR_SHARED);
	m_btn_left_x_right.SetIcon(hIcon);
	m_btn_right_x_right.SetIcon(hIcon);
	/***********************Set arrow icons (end) *****************/


	/***********************Set default manip step size (start) *****************/
	m_combo_left_step_size.AddString("0.5");
	m_combo_left_step_size.AddString("1");
	m_combo_left_step_size.AddString("2");
	m_combo_left_step_size.AddString("5");
	m_combo_left_step_size.AddString("10");
	m_combo_left_step_size.AddString("20");
	m_combo_left_step_size.AddString("25");
	m_combo_left_step_size.AddString("50");
	m_combo_left_step_size.AddString("100");
	m_combo_left_step_size.AddString("200");
	m_combo_left_step_size.AddString("500");
	m_combo_left_step_size.AddString("1000");
	m_combo_left_step_size.SetWindowTextA("100");

	m_combo_right_step_size.AddString("0.5");
	m_combo_right_step_size.AddString("1");
	m_combo_right_step_size.AddString("2");
	m_combo_right_step_size.AddString("5");
	m_combo_right_step_size.AddString("10");
	m_combo_right_step_size.AddString("20");
	m_combo_right_step_size.AddString("25");
	m_combo_right_step_size.AddString("50");
	m_combo_right_step_size.AddString("100");
	m_combo_right_step_size.AddString("200");
	m_combo_right_step_size.AddString("500");
	m_combo_right_step_size.AddString("1000");
	m_combo_right_step_size.SetWindowTextA("100");
	/***********************Set default manip step size (end) *****************/
	
	/***********************Set default focus step size (start) *****************/
	m_combo_focus_step_size.AddString("1");
	m_combo_focus_step_size.AddString("2");
	m_combo_focus_step_size.AddString("5");
	m_combo_focus_step_size.AddString("10");
	m_combo_focus_step_size.AddString("20");
	m_combo_focus_step_size.AddString("50");
	m_combo_focus_step_size.AddString("100");
	m_combo_focus_step_size.AddString("200");
	m_combo_focus_step_size.AddString("500");
	m_combo_focus_step_size.SetWindowTextA("1");
	/***********************Set default focus step size (end) *****************/
	//objective switch
	//m_combo_objective_switch.AddString("1");
	//m_combo_objective_switch.AddString("2");
	//m_combo_objective_switch.AddString("3");
	//m_combo_objective_switch.AddString("4");
	//m_combo_objective_switch.AddString("5");
	//m_combo_objective_switch.AddString("6");
	//m_combo_objective_switch.SetWindowTextA("5");
	////condenser switch
	//m_combo_condenser.AddString("A");
	//m_combo_condenser.AddString("NAMC2");
	//m_combo_condenser.AddString("NAMC3");
	//m_combo_condenser.AddString("DIC N1");
	//m_combo_condenser.AddString("DIC N2");
	//m_combo_condenser.SetWindowTextA("A");

	//////////////////////////////////////////////////////////////////////////

	m_pump_position_mode.SetCheck(TRUE);
	m_pump_is_aspirate = true;

	UpdateData(FALSE);

	return TRUE;
}


BEGIN_MESSAGE_MAP(Dlg_Tools, CDialog)
	ON_BN_CLICKED(IDC_BTN_RECORD_VIDEO, &Dlg_Tools::OnBnClickedBtnRecordVideo)
	ON_BN_CLICKED(IDC_BTN_SAVE_IMAGE, &Dlg_Tools::OnBnClickedBtnSaveImage)
	ON_UPDATE_COMMAND_UI(IDC_BTN_SAVE_IMAGE, &Dlg_Tools::OnUpdateImageAccquired)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RECORD_VIDEO, &Dlg_Tools::OnUpdateImageAccquired)

	ON_BN_CLICKED(IDC_BTN_LEFT_X_LEFT, &Dlg_Tools::OnBnClickedBtnLeftXLeft)
	ON_UPDATE_COMMAND_UI(IDC_BTN_LEFT_X_LEFT, &Dlg_Tools::OnUpdateBnClickedManipLeft)
	ON_BN_CLICKED(IDC_BTN_LEFT_X_RIGHT, &Dlg_Tools::OnBnClickedBtnLeftXRight)
	ON_UPDATE_COMMAND_UI(IDC_BTN_LEFT_X_RIGHT, &Dlg_Tools::OnUpdateBnClickedManipLeft)
	ON_BN_CLICKED(IDC_BTN_LEFT_Y_UP, &Dlg_Tools::OnBnClickedBtnLeftYUp)
	ON_UPDATE_COMMAND_UI(IDC_BTN_LEFT_Y_UP, &Dlg_Tools::OnUpdateBnClickedManipLeft)
	ON_BN_CLICKED(IDC_BTN_LEFT_Y_DOWN, &Dlg_Tools::OnBnClickedBtnLeftYDown)
	ON_UPDATE_COMMAND_UI(IDC_BTN_LEFT_Y_DOWN, &Dlg_Tools::OnUpdateBnClickedManipLeft)
	ON_BN_CLICKED(IDC_BTN_LEFT_Z_UP, &Dlg_Tools::OnBnClickedBtnLeftZUp)
	ON_UPDATE_COMMAND_UI(IDC_BTN_LEFT_Z_UP, &Dlg_Tools::OnUpdateBnClickedManipLeft)
	ON_BN_CLICKED(IDC_BTN_LEFT_Z_DOWN, &Dlg_Tools::OnBnClickedBtnLeftZDown)
	ON_UPDATE_COMMAND_UI(IDC_BTN_LEFT_Z_DOWN, &Dlg_Tools::OnUpdateBnClickedManipLeft)

	ON_BN_CLICKED(IDC_BTN_RIGHT_X_LEFT, &Dlg_Tools::OnBnClickedBtnRightXLeft)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RIGHT_X_LEFT, &Dlg_Tools::OnUpdateBnClickedManipRight)
	ON_BN_CLICKED(IDC_BTN_RIGHT_X_RIGHT, &Dlg_Tools::OnBnClickedBtnRightXRight)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RIGHT_X_RIGHT, &Dlg_Tools::OnUpdateBnClickedManipRight)
	ON_BN_CLICKED(IDC_BTN_RIGHT_Y_UP, &Dlg_Tools::OnBnClickedBtnRightYUp)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RIGHT_Y_UP, &Dlg_Tools::OnUpdateBnClickedManipRight)
	ON_BN_CLICKED(IDC_BTN_RIGHT_Y_DOWN, &Dlg_Tools::OnBnClickedBtnRightYDown)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RIGHT_Y_DOWN, &Dlg_Tools::OnUpdateBnClickedManipRight)
	ON_BN_CLICKED(IDC_BTN_RIGHT_Z_UP, &Dlg_Tools::OnBnClickedBtnRightZUp)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RIGHT_Z_UP, &Dlg_Tools::OnUpdateBnClickedManipRight)
	ON_BN_CLICKED(IDC_BTN_RIGHT_Z_DOWN, &Dlg_Tools::OnBnClickedBtnRightZDown)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RIGHT_Z_DOWN, &Dlg_Tools::OnUpdateBnClickedManipRight)
	
	ON_BN_CLICKED(IDC_BTN_ASPIRATE, &Dlg_Tools::OnBnClickedBtnAspirate)
	ON_UPDATE_COMMAND_UI(IDC_BTN_ASPIRATE, &Dlg_Tools::OnUpdatePumpMotorControl)
	ON_BN_CLICKED(IDC_BTN_INFUSE, &Dlg_Tools::OnBnClickedBtnInfuse)
	ON_UPDATE_COMMAND_UI(IDC_BTN_INFUSE, &Dlg_Tools::OnUpdatePumpMotorControl)
	ON_BN_CLICKED(IDC_BTN_PUMP_STOP, &Dlg_Tools::OnBnClickedBtnPumpStop)
	ON_UPDATE_COMMAND_UI(IDC_BTN_PUMP_STOP, &Dlg_Tools::OnUpdatePumpMotorControl)
	

	

	ON_BN_CLICKED(IDC_BTN_PUMP_ON, &Dlg_Tools::OnBnClickedBtnPumpOn)



	
	ON_BN_CLICKED(IDC_BTN_CONTACT_DETECTION, &Dlg_Tools::OnBnClickedBtnContactDetection)
	ON_UPDATE_COMMAND_UI(IDC_BTN_CONTACT_DETECTION, &Dlg_Tools::OnUpdateContactDetection)


	ON_BN_CLICKED(IDC_BUTTON1, &Dlg_Tools::OnBnClickedButton1)
END_MESSAGE_MAP()


// override pretranslate
//BOOL Dlg_Tools::PreTranslateMessage(MSG* pMsg) 
//{
//	return 0;
//}

// Dlg_Tools message handlers

// Record Video
void Dlg_Tools::OnBnClickedBtnRecordVideo()
{
	// TODO: Add your control notification handler code here

	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->OnRecordVideo();
}

// Save Image
void Dlg_Tools::OnBnClickedBtnSaveImage()
{
	// TODO: Add your control notification handler code here

	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->SaveImage();
}

// Update UI control for 
void Dlg_Tools::OnUpdateImageAccquired(CCmdUI* pCmdUI)
{
	//CRoboticBiopsyView* pView = (CRoboticBiopsyView*)GetActiveView();
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();

	pCmdUI->Enable(pWnd->m_image_is_loaded);
}


// Update UI control for the LEFT manipulator
void Dlg_Tools::OnUpdateBnClickedManipLeft(CCmdUI* pCmdUI)
{
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
	pCmdUI->Enable(pWnd->m_current_state == M_IDLE  && ENABLE_MANIP_LEFT);
}

// Left Manip X Left
void Dlg_Tools::OnBnClickedBtnLeftXLeft()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_left_step_size.GetWindowText(step_size);

	cout << "Sutter Step Size: " << step_size << ".\n" << endl;

	pWnd->OnMoveManipLeft(1, -atof(step_size));
}


// Left Manip X Right
void Dlg_Tools::OnBnClickedBtnLeftXRight()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_left_step_size.GetWindowText(step_size);

	pWnd->OnMoveManipLeft(1, atof(step_size));
}


// Left Manip Y Up
void Dlg_Tools::OnBnClickedBtnLeftYUp()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_left_step_size.GetWindowText(step_size);

	pWnd->OnMoveManipLeft(2, atof(step_size));
}


// Left Manip Y Down
void Dlg_Tools::OnBnClickedBtnLeftYDown()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_left_step_size.GetWindowText(step_size);

	pWnd->OnMoveManipLeft(2, -atof(step_size));

}


// Left Manip Z Up
void Dlg_Tools::OnBnClickedBtnLeftZUp()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_left_step_size.GetWindowText(step_size);

	pWnd->OnMoveManipLeft(3, -atof(step_size));
}


// Left Manip Z Down
void Dlg_Tools::OnBnClickedBtnLeftZDown()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_left_step_size.GetWindowText(step_size);

	pWnd->OnMoveManipLeft(3, atof(step_size));
}



// Update UI control for the RIGHT manipulator
void Dlg_Tools::OnUpdateBnClickedManipRight(CCmdUI* pCmdUI)
{
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
	pCmdUI->Enable( pWnd->m_current_state == M_IDLE && ENABLE_MANIP_RIGHT);
}


// Right Manip X Left
void Dlg_Tools::OnBnClickedBtnRightXLeft()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_right_step_size.GetWindowText(step_size);

	//pWnd->OnMoveManipRight(1, -atof(step_size));

}

// Right Manip X Right
void Dlg_Tools::OnBnClickedBtnRightXRight()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_right_step_size.GetWindowText(step_size);

	//pWnd->OnMoveManipRight(1, atof(step_size));
}


// Right Manip Y Up
void Dlg_Tools::OnBnClickedBtnRightYUp()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_right_step_size.GetWindowText(step_size);

	//pWnd->OnMoveManipRight(2, atof(step_size));
}


// Right Manip Y Down
void Dlg_Tools::OnBnClickedBtnRightYDown()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_right_step_size.GetWindowText(step_size);

	//pWnd->OnMoveManipRight(2, -atof(step_size));
}


// Right Manip Z Up
void Dlg_Tools::OnBnClickedBtnRightZUp()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_right_step_size.GetWindowText(step_size);

	//pWnd->OnMoveManipRight(3, atof(step_size));
}


// Right Manip Z Down
void Dlg_Tools::OnBnClickedBtnRightZDown()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();

	CString step_size;
	m_combo_right_step_size.GetWindowText(step_size);

	//pWnd->OnMoveManipRight(3, -atof(step_size));
}

// Eppendorf Pump Power ON
void Dlg_Tools::OnBnClickedBtnPumpOn()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pump_motor.PowerOn();
	printf("Pump is engaged.\n");
}


// Eppendorf Pump Aspiration
void Dlg_Tools::OnBnClickedBtnAspirate()
{
	// TODO: Add your control notification handler code here
	//PostMessage(WM_CLFEEDBACK_ACQUIRED, 0, 0);
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();

	if (!pWnd->m_pump_motor.GetPowerState()) {
		printf("Please press the 'ON' button first to engage the pump!\n");
	}
	else {
		UpdateData(TRUE);
		m_pump_steps = m_pump_steps > 0 ? m_pump_steps : -m_pump_steps;
		m_pump_is_aspirate = true;
		printf("Aspirating...\n");
		pWnd->OnPumpMotorControl();
	}
	
}

// Eppendorf Pump Infusion
void Dlg_Tools::OnBnClickedBtnInfuse()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();

	UpdateData(TRUE);
	m_pump_steps = m_pump_steps > 0 ? m_pump_steps: -m_pump_steps;
	m_pump_is_aspirate = false;

	//pWnd->OnPumpMotorControl();
}

// Eppendorf Pump Stop
void Dlg_Tools::OnBnClickedBtnPumpStop()
{
	// TODO: Add your control notification handler code here
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	if (pWnd->m_current_state = M_ASPIRATE)
	{
		pWnd->m_current_state = M_IDLE;
		pWnd->m_pump_motor.Stop();
		pWnd->m_pump_motor.PowerOff();
		printf("Pump is unengaged.\n");
		//std::cout << "m_current_state:" << pWnd->m_current_state << std::endl;
	}
	else 
	{
		printf("Pump is idle.\n");
	}

}


// Update Pump Motor UI control
void Dlg_Tools::OnUpdatePumpMotorControl(CCmdUI* pCmdUI)
{
	CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
	//pCmdUI->Enable((ENABLE_PUMP_MOTOR || ENABLE_ROTATIONAL_STAGE ) && pWnd-> m_is_pump_motor_start);
}


// Contact Detection
void Dlg_Tools::OnBnClickedBtnContactDetection()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->OnContactDetection();
}

void Dlg_Tools::OnUpdateContactDetection(CCmdUI* pCmdUI)
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pCmdUI->Enable(pWnd->m_current_state == M_IDLE && ENABLE_MANIP_LEFT && pWnd->m_aquiring_images);
}

void Dlg_Tools::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->OnFocusMoving();
}
