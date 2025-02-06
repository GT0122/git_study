
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "GTPPainter.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_COLOR_CUSTOM, &CMainFrame::OnColorCustom)
	ON_COMMAND(ID_BLACK, &CMainFrame::OnBlack)
	ON_COMMAND(ID_LRED, &CMainFrame::OnLred)
	ON_COMMAND(ID_LIME, &CMainFrame::OnLime)
	ON_COMMAND(ID_BLUE, &CMainFrame::OnBlue)
	ON_UPDATE_COMMAND_UI(ID_BLACK, &CMainFrame::OnUpdateBlack)
	ON_UPDATE_COMMAND_UI(ID_LRED, &CMainFrame::OnUpdateLred)
	ON_UPDATE_COMMAND_UI(ID_LIME, &CMainFrame::OnUpdateLime)
	ON_UPDATE_COMMAND_UI(ID_BLUE, &CMainFrame::OnUpdateBlue)
	ON_UPDATE_COMMAND_UI(ID_COLOR_CUSTOM, &CMainFrame::OnUpdateColorCustom)
	ON_COMMAND(ID_THICK_1, &CMainFrame::OnThick1)
	ON_COMMAND(ID_THICK_3, &CMainFrame::OnThick3)
	ON_COMMAND(ID_THICK_5, &CMainFrame::OnThick5)
	ON_UPDATE_COMMAND_UI(ID_THICK_1, &CMainFrame::OnUpdateThick1)
	ON_UPDATE_COMMAND_UI(ID_THICK_3, &CMainFrame::OnUpdateThick3)
	ON_UPDATE_COMMAND_UI(ID_THICK_5, &CMainFrame::OnUpdateThick5)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_NEW, &CMainFrame::OnFileSaveNew)
	ON_COMMAND(ID_FILE_SAVE_AS_NEW, &CMainFrame::OnFileSaveAsNew)
	ON_COMMAND(ID_FILE_CLOSE_NEW, &CMainFrame::OnCloseNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_NEW, &CMainFrame::OnUpdateFileSaveNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_NEW, &CMainFrame::OnUpdateFileSaveAsNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_NEW, &CMainFrame::OnUpdateClose)
	ON_MESSAGE(GTPPainter_DRAWING, &CMainFrame::OnDrawing)
	ON_WM_CONTEXTMENU()
	ON_WM_ACTIVATEAPP()
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_EXIT, &CMainFrame::OnClose)
	ON_CBN_SELCHANGE(ID_THICKNESS, &CMainFrame::OnCbThicknessChange)
	ON_CBN_SELCHANGE(ID_COLOR, &CMainFrame::OnCbColorChange)
	ON_MESSAGE(GTPPainter_CLOSING, &CMainFrame::OnClosing)
	ON_MESSAGE(GTPPainter_OPENING, &CMainFrame::OnOpening)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_bAutoMenuEnable = FALSE;
	m_i32Thick = 1;
	m_sLineColor = RGB(0, 0, 0);
	m_bModify = false;
	m_bDrawing = false;
	int32_t i32Code = m_strTitle.LoadStringW(AFX_IDS_APP_TITLE);
	m_strFileName = "";
	m_i32ColorIndex = 0;
	m_strPreName = "";
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	   !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if(!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	CImageList imgToolBar;
	CBitmap bmpToolBar;

	m_wndToolBar.SetHeight(23);
	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(50, 500);
	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	bmpToolBar.LoadBitmap(IDB_TOOLBAR_NEW);
	imgToolBar.Create(16, 15, ILC_COLOR24 | ILC_MASK, 1, 0);
	imgToolBar.Add(&bmpToolBar, RGB(192, 192, 192));

	m_wndToolBar.GetToolBarCtrl().SetImageList(&imgToolBar);

	imgToolBar.Detach();

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	m_wndToolBar.SetButtons(NULL, 8);

	m_wndToolBar.SetButtonInfo(0, ID_FILE_NEW, TBBS_BUTTON, 0);
	m_wndToolBar.SetButtonInfo(1, NULL, TBBS_SEPARATOR, NULL);
	m_wndToolBar.SetButtonInfo(2, ID_FILE_OPEN, TBSTYLE_BUTTON, 1);
	m_wndToolBar.SetButtonInfo(3, ID_FILE_SAVE_NEW, TBSTYLE_BUTTON, 2);
	m_wndToolBar.SetButtonInfo(4, ID_FILE_CLOSE_NEW, TBSTYLE_BUTTON, 3);
	m_wndToolBar.SetButtonInfo(5, NULL, TBBS_SEPARATOR, NULL);

	CRect rButton;

	m_wndToolBar.GetItemRect(6, &rButton);
	rButton.left += 0;
	rButton.top += 0;
	rButton.right += 50;
	rButton.bottom += 100;

	m_cbThick.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE, rButton, &m_wndToolBar, ID_THICKNESS);
	m_cbThick.AddString(_T("1 px"));
	m_cbThick.AddString(_T("3 px"));
	m_cbThick.AddString(_T("5 px"));
	m_cbThick.SetCurSel(0);

	m_cbThick.SetItemData(0, 1);
	m_cbThick.SetItemData(1, 3);
	m_cbThick.SetItemData(2, 5);

	m_wndToolBar.GetItemRect(7, &rButton);
	rButton.left += 50;
	rButton.top += 0;
	rButton.right += 150;
	rButton.bottom += 100;

	m_cbColor.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE, rButton, &m_wndToolBar, ID_COLOR);
	m_cbColor.AddString(_T("BLACK"));
	m_cbColor.AddString(_T("LIGHTRED"));
	m_cbColor.AddString(_T("LIME"));
	m_cbColor.AddString(_T("BLUE"));
	m_cbColor.AddString(_T("User defined"));
	m_cbColor.SetCurSel(0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnColorCustom()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CColorDialog dlgColor(m_sLineColor);

	if(dlgColor.DoModal() == IDOK)
		m_sLineColor = dlgColor.GetColor();

	if(m_sLineColor == RGB(0, 0, 0))
	{
		m_cbColor.SetCurSel(0);
		m_i32ColorIndex = 0;
	}
	else if(m_sLineColor == RGB(255, 0, 0))
	{
		m_cbColor.SetCurSel(1);
		m_i32ColorIndex = 1;
	}
	else if(m_sLineColor == RGB(0, 255, 0))
	{
		m_cbColor.SetCurSel(2);
		m_i32ColorIndex = 2;
	}
	else if(m_sLineColor == RGB(0, 0, 255))
	{
		m_cbColor.SetCurSel(3);
		m_i32ColorIndex = 3;
	}
	else
	{
		m_cbColor.SetCurSel(4); 
		m_i32ColorIndex = 4;
	}

	GetActiveView()->PostMessage(GTPPainter_COLOR, m_sLineColor);
}

void CMainFrame::OnBlack()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_sLineColor = RGB(0, 0, 0);
	m_cbColor.SetCurSel(0);

	GetActiveView()->PostMessage(GTPPainter_COLOR, m_sLineColor);
}

void CMainFrame::OnLred()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_sLineColor = RGB(255, 0, 0);
	m_cbColor.SetCurSel(1);

	GetActiveView()->PostMessage(GTPPainter_COLOR, m_sLineColor);
}

void CMainFrame::OnLime()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_sLineColor = RGB(0, 255, 0);
	m_cbColor.SetCurSel(2);

	GetActiveView()->PostMessage(GTPPainter_COLOR, m_sLineColor);
}

void CMainFrame::OnBlue()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_sLineColor = RGB(0, 0, 255);
	m_cbColor.SetCurSel(3);

	GetActiveView()->PostMessage(GTPPainter_COLOR, m_sLineColor);
}

void CMainFrame::OnUpdateBlack(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_sLineColor == RGB(0, 0, 0))
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnUpdateLred(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_sLineColor == RGB(255, 0, 0))
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnUpdateLime(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_sLineColor == RGB(0, 255, 0))
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnUpdateBlue(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_sLineColor == RGB(0, 0, 255))
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnUpdateColorCustom(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.

	if(m_sLineColor != RGB(0, 0, 0) && m_sLineColor != RGB(255, 0, 0) && m_sLineColor != RGB(0, 255, 0) && m_sLineColor != RGB(0, 0, 255))
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnThick1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_i32Thick = 1;
	m_cbThick.SetCurSel(0);

	GetActiveView()->PostMessage(GTPPainter_THICKNESS, m_i32Thick);
}

void CMainFrame::OnThick3()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_i32Thick = 3;
	m_cbThick.SetCurSel(1);

	GetActiveView()->PostMessage(GTPPainter_THICKNESS, m_i32Thick);
}

void CMainFrame::OnThick5()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_i32Thick = 5;
	m_cbThick.SetCurSel(2);

	GetActiveView()->PostMessage(GTPPainter_THICKNESS, m_i32Thick);
}

void CMainFrame::OnUpdateThick1(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_i32Thick == 1)
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnUpdateThick3(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_i32Thick == 3)
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnUpdateThick5(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_i32Thick == 5)
		pCmdUI->SetCheck(MF_CHECKED);
	else
		pCmdUI->SetCheck(MF_UNCHECKED);
}

void CMainFrame::OnFileNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(m_bDrawing)
	{
		if(!m_bModify)
		{
			m_strFileName = "";

			GetActiveDocument()->SetPathName(m_strDefaultPath);
			GetActiveDocument()->SetTitle(m_strDefaultName);
			SetWindowText(m_strDefaultName + " - " + m_strTitle);

			GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
		}
		else
		{
			int32_t i32Result = MessageBox(_T("There is an unsaved document. Do you want to save it?"), _T("New File"), MB_YESNOCANCEL);

			if(i32Result == IDYES)
			{
				if(m_strFileName == "")
				{
					CFileDialog dlgFile(FALSE, _T(".flp"), m_strDefaultName, OFN_OVERWRITEPROMPT, _T("Fourth Logic Paint(*.flp)|*.flp|모든 파일(*.*)|*.*|"), this);
					i32Result = (int32_t)dlgFile.DoModal();

					if(i32Result == IDOK)
					{
						m_strFileName = dlgFile.GetFileName();
						m_strFileName = m_strFileName.Left(m_strFileName.GetLength() - 4);

						m_strPath = dlgFile.GetPathName();
						m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));

						GetActiveDocument()->SetPathName(m_strPath);
						GetActiveDocument()->SetTitle(m_strFileName);
						GetActiveView()->SendMessage(GTPPainter_SAVING);

						SetWindowText(m_strDefaultName + " - " + m_strTitle);
						
						m_strFileName = "";
						GetActiveDocument()->SetPathName(m_strDefaultPath);
						GetActiveDocument()->SetTitle(m_strDefaultName);
						GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
					}
				}
				else
				{
					GetActiveView()->SendMessage(GTPPainter_SAVING);

					SetWindowText(m_strDefaultName + " - " + m_strTitle);

					m_strFileName = "";
					GetActiveDocument()->SetPathName(m_strDefaultPath);
					GetActiveDocument()->SetTitle(m_strDefaultName);
					GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
				}

			}
			else if(i32Result == IDNO)
			{
				m_strFileName = "";

				GetActiveDocument()->SetPathName(m_strDefaultPath);
				GetActiveDocument()->SetTitle(m_strDefaultName);
				SetWindowText(m_strDefaultName + " - " + m_strTitle);

				GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
			}
		}
	}
}

void CMainFrame::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	bool bOpening = true;

	if(m_bModify)
	{
		int32_t i32Result = MessageBox(_T("There is an unsaved document. Do you want to save it?"), _T("File save"), MB_YESNOCANCEL);

		if(i32Result == IDYES)
		{
			if(m_strFileName == "")
			{
				CFileDialog dlgSave(FALSE, _T(".flp"), m_strDefaultName, OFN_OVERWRITEPROMPT, _T("Fourth Logic Paint(*.flp)|*.flp|모든 파일(*.*)|*.*|"), this);
				i32Result = (int32_t)dlgSave.DoModal();

				if(i32Result == IDOK)
				{
					m_strFileName = dlgSave.GetFileName();
					m_strFileName = m_strFileName.Left(m_strFileName.GetLength() - 4);

					m_strPath = dlgSave.GetPathName();
					m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));

					GetActiveDocument()->SetPathName(m_strPath);
					GetActiveDocument()->SetTitle(m_strFileName);

					GetActiveView()->SendMessage(GTPPainter_SAVING);
				}
				else if(i32Result == IDCANCEL)
					bOpening = false;
			}
			else
				GetActiveView()->SendMessage(GTPPainter_SAVING);
		}
		else if(i32Result == IDCANCEL)
			bOpening = false;
	}

	if(bOpening)
	{
		CFileDialog dlgLoad(TRUE, _T(".flp"), m_strDefaultName, OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST, _T("Fourth Logic Paint(*.flp)|*.flp|모든 파일(*.*)|*.*|"), this);
		int32_t i32Result = (int32_t)dlgLoad.DoModal();

		if(i32Result == IDOK)
		{
			m_strFileName = dlgLoad.GetFileName();
			m_strFileName = m_strFileName.Left(m_strFileName.GetLength() - 4);

			m_strPath = dlgLoad.GetPathName();
			m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));

			m_strPreName = GetActiveDocument()->GetTitle();

			GetActiveDocument()->SetPathName(m_strPath);
			GetActiveDocument()->SetTitle(m_strPreName);

			CString* pstrString = new CString(m_strFileName);
			GetActiveView()->SendMessage(GTPPainter_LOADING, NULL, (LPARAM)pstrString);

			delete pstrString;
			pstrString = nullptr;
		}
	}
}

void CMainFrame::OnFileSaveNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(m_bModify)
	{
		if(m_strFileName != "")
		{
			SetWindowText(m_strFileName + " - " + m_strTitle);

			GetActiveView()->SendMessage(GTPPainter_SAVING);
		}
		else
			OnFileSaveAsNew();
	}
}

void CMainFrame::OnFileSaveAsNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다
	CFileDialog dlgFile(FALSE, _T(".flp"), m_strFileName == "" ? m_strDefaultName : m_strFileName, OFN_OVERWRITEPROMPT, _T("Fourth Logic Paint(*.flp)|*.flp|모든 파일(*.*)|*.*|"), this);
	int32_t i32Result = (int32_t)dlgFile.DoModal();

	if(i32Result == IDOK)
	{
		m_strFileName = dlgFile.GetFileName();
		m_strFileName = m_strFileName.Left(m_strFileName.GetLength() - 4);

		m_strPath = dlgFile.GetPathName();
		m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));

		GetActiveDocument()->SetPathName(m_strPath);
		GetActiveDocument()->SetTitle(m_strFileName);

		SetWindowText(m_strFileName + " - " + m_strTitle);

		GetActiveView()->SendMessage(GTPPainter_SAVING);
	}

	GetActiveView()->PostMessage(GTPPainter_POPUP);
}

void CMainFrame::OnCloseNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(m_bDrawing)
	{
		if(!m_bModify)
		{
			m_strFileName = "";

			GetActiveDocument()->SetPathName(m_strDefaultPath);
			GetActiveDocument()->SetTitle(m_strDefaultName);
			SetWindowText(m_strDefaultName + " - " + m_strTitle);

			GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
		}
		else
		{
			int32_t i32Result = MessageBox(_T("There is an unsaved document. Do you want to save it?"), _T("Close"), MB_YESNOCANCEL);

			if(i32Result == IDYES)
			{
				if(m_strFileName == "")
				{
					CFileDialog dlgFile(FALSE, _T(".flp"), m_strDefaultName, OFN_OVERWRITEPROMPT, _T("Fourth Logic Paint(*.flp)|*.flp|모든 파일(*.*)|*.*|"), this);
					i32Result = (int32_t)dlgFile.DoModal();

					if(i32Result == IDOK)
					{
						m_strFileName = dlgFile.GetFileName();
						m_strFileName = m_strFileName.Left(m_strFileName.GetLength() - 4);

						m_strPath = dlgFile.GetPathName();
						m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));

						GetActiveDocument()->SetPathName(m_strPath);
						GetActiveDocument()->SetTitle(m_strFileName);
						GetActiveView()->SendMessage(GTPPainter_SAVING);

						SetWindowText(m_strDefaultName + " - " + m_strTitle);

						m_strFileName = "";
						GetActiveDocument()->SetPathName(m_strDefaultPath);
						GetActiveDocument()->SetTitle(m_strDefaultName);
						GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
					}
				}
				else
				{
					GetActiveView()->SendMessage(GTPPainter_SAVING);

					SetWindowText(m_strDefaultName + " - " + m_strTitle);

					m_strFileName = "";
					GetActiveDocument()->SetPathName(m_strDefaultPath);
					GetActiveDocument()->SetTitle(m_strDefaultName);
					GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
				}
			}
			else if(i32Result == IDNO)
			{
				m_strFileName = "";

				GetActiveDocument()->SetPathName(m_strDefaultPath);
				GetActiveDocument()->SetTitle(m_strDefaultName);
				SetWindowText(m_strDefaultName + " - " + m_strTitle);

				GetActiveView()->PostMessage(GTPPainter_NEWDRAW);
			}
		}
	}
}

void CMainFrame::OnUpdateFileSaveNew(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_bModify)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateFileSaveAsNew(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CMainFrame::OnUpdateClose(CCmdUI* pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(m_bDrawing)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

afx_msg LRESULT CMainFrame::OnDrawing(WPARAM wParam, LPARAM lParam)
{
	m_bDrawing = (bool)wParam;
	m_bModify = (bool)lParam;

	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(GetKeyState(VK_CONTROL) & 0x8000)
		{
			if(GetKeyState(VK_SHIFT) & 0x8000)
			{
				if(pMsg->wParam == 49)
					OnBlack();
				else if(pMsg->wParam == 50)
					OnLred();
				else if(pMsg->wParam == 51)
					OnLime();
				else if(pMsg->wParam == 52)
					OnBlue();
			}
			else if(pMsg->wParam == 49)
				OnThick1();
			else if(pMsg->wParam == 50)
				OnThick3();
			else if(pMsg->wParam == 51)
				OnThick5();
			else if(pMsg->wParam == 83)
				OnFileSaveNew();
			else if(pMsg->wParam == 78)
				OnFileNew();
			else if(pMsg->wParam == 79)
				OnFileOpen();
		}
	}

	return false;
}

void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu menuPopup;

	menuPopup.LoadMenuW(IDR_POPUP);
	CMenu* menuSubPopup = menuPopup.GetSubMenu(0);

	menuSubPopup->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());

	GetActiveView()->PostMessage(GTPPainter_POPUP);
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_strDefaultName.GetLength() == 0)
	{
		m_strDefaultName = GetActiveDocument()->GetTitle();

		TCHAR arrChar[256];

		GetModuleFileName(NULL, arrChar, 256);
		m_strDefaultPath = arrChar;

		int32_t i32Index;

		for(int32_t i = 0; i < 3; i++)
		{
			i32Index = m_strDefaultPath.ReverseFind('\\');
			m_strDefaultPath = m_strDefaultPath.Left(i32Index);
		}
	}
}

void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(!m_bModify)
		CFrameWnd::OnClose();
	else
	{
		int32_t i32Result = MessageBox(_T("There is an unsaved document. Do you want to save it?"), _T("Exit"), MB_YESNOCANCEL);

		if(i32Result == IDYES)
		{
			if(m_strFileName == "")
			{
				CFileDialog dlgFile(FALSE, _T(".flp"), m_strDefaultName, OFN_OVERWRITEPROMPT, _T("Fourth Logic Paint(*.flp)|*.flp|모든 파일(*.*)|*.*|"), this);
				i32Result = (int32_t)dlgFile.DoModal();

				if(i32Result == IDOK)
				{
					m_strFileName = dlgFile.GetFileName();
					m_strFileName = m_strFileName.Left(m_strFileName.GetLength() - 4);

					m_strPath = dlgFile.GetPathName();
					m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));

					GetActiveDocument()->SetPathName(m_strPath);
					GetActiveDocument()->SetTitle(m_strFileName);
					GetActiveView()->PostMessage(GTPPainter_SAVING, -1);
				}
			}
			else
				GetActiveView()->PostMessage(GTPPainter_SAVING, -1);
		}
		else if(i32Result == IDNO)
			CFrameWnd::OnClose();
	}
}

void CMainFrame::OnCbThicknessChange()
{
	int32_t i32Thick = (int32_t)m_cbThick.GetItemData(m_cbThick.GetCurSel());

	if(i32Thick == 1)
		OnThick1();
	else if(i32Thick == 3)
		OnThick3();
	else
		OnThick5();
}

void CMainFrame::OnCbColorChange()
{
	int32_t i32Color = m_cbColor.GetCurSel();

	if(i32Color == 0)
		OnBlack();
	else if(i32Color == 1)
		OnLred();
	else if(i32Color == 2)
		OnLime();
	else if(i32Color == 3)
		OnBlue();
	else if(i32Color == 4)
		OnColorCustom();
}

afx_msg LRESULT CMainFrame::OnClosing(WPARAM wParam, LPARAM lParam)
{
	OnClose();

	return 0;
}


afx_msg LRESULT CMainFrame::OnOpening(WPARAM wParam, LPARAM lParam)
{
	if((bool)wParam)
		GetActiveDocument()->SetTitle(m_strFileName);
	else
	{
		if(m_strPreName == "")
			GetActiveDocument()->SetTitle(m_strDefaultName);
		else
			GetActiveDocument()->SetTitle(m_strPreName);

		m_strFileName = m_strPreName;
	}

	return 0;
}
