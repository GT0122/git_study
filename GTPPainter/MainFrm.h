
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once

#define GTPPainter_SAVING    WM_USER + 120
#define GTPPainter_LOADING   WM_USER + 130
#define GTPPainter_THICKNESS WM_USER + 200
#define GTPPainter_COLOR     WM_USER + 210
#define GTPPainter_DRAWING   WM_USER + 300
#define GTPPainter_NEWDRAW   WM_USER + 310
#define GTPPainter_POPUP     WM_USER + 330
#define GTPPainter_CLOSING   WM_USER + 400
#define GTPPainter_OPENING   WM_USER + 410

class CMainFrame : public CFrameWnd
{

protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	int32_t m_i32Thick, m_i32ColorIndex;
	COLORREF m_sLineColor;
	bool m_bModify, m_bDrawing;
	CString m_strTitle, m_strFileName, m_strDefaultName, m_strPreName; 
	CComboBox m_cbThick;
	CComboBox m_cbColor;
	CString m_strPath, m_strDefaultPath;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif
	void OnCbThicknessChange();
	void OnCbColorChange();


protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnColorCustom();
	afx_msg void OnBlack();
	afx_msg void OnLred();
	afx_msg void OnLime();
	afx_msg void OnBlue();
	afx_msg void OnUpdateBlack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLred(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBlue(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColorCustom(CCmdUI* pCmdUI);
	afx_msg void OnThick1();
	afx_msg void OnThick3();
	afx_msg void OnThick5();
	afx_msg void OnUpdateThick1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThick3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThick5(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveNew();
	afx_msg void OnFileSaveAsNew();
	afx_msg void OnCloseNew();
	afx_msg void OnUpdateFileSaveNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAsNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClose(CCmdUI* pCmdUI);

protected:
	afx_msg LRESULT OnDrawing(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClosing(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpening(WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnClose();
};


