// GTPPainterView.h: CGTPPainterView 클래스의 인터페이스
//

#pragma once

#include "PointF64.h"
#include <vector>

class CGTPPainterView : public CView
{
protected: // serialization에서만 만들어집니다.
	CGTPPainterView() noexcept;
	DECLARE_DYNCREATE(CGTPPainterView)

// 특성입니다.
public:
	CGTPPainterDoc* GetDocument() const;

// 작업입니다.
protected:
	CPointF64 ConvertCanvasToReal(CPointF64 ptdInput);
	CPointF64 ConvertRealToCanvas(CPointF64 ptdInput);
	bool IsInView(CPointF64 ptdInput);
	bool GetLineInView(std::vector<CPointF64>* pVctPtdLine, std::vector<std::vector<CPointF64>>* pVct2PtdNew);
	uint32_t ConvertToLittleEndian(int32_t i32Value, int8_t* pI8Char);
	uint32_t ConvertToLittleEndian(COLORREF sValue, int8_t* pI8Char);
	COLORREF ConvertToColor(int8_t* pI8Char);
	uint32_t ConvertToInt(int8_t* pI8Char);
	double ConvertToEndian(double f64Value, int8_t* pI8Char);
	double ConvertToDouble(double f64Value, int8_t* pI8Char);
	void CalcCrc32(uint32_t* pU32Crc32val, int8_t* pI8Char, const int32_t i32Size);

protected:
	int32_t m_i32WindowCount, m_i32Thickness, m_i32Count;
	int32_t m_i32PrevWidth, m_i32PrevHeight;
	double m_f64Scale;
	double m_f64HalfX, m_f64HalfY;
	double m_f64CanvasCurX, m_f64CanvasCurY;
	bool m_bTitleChange, m_bDrawn, m_bSaved;
	CPointF64 m_ptdCursor, m_ptdOffset;
	CPointF64 m_ptdTemp;
	CPointF64 m_ptdViewZero, m_ptdViewXY;
	COLORREF m_sLineColor;
	std::vector<std::vector<CPointF64>> m_vct2PtReals;
	std::vector<CPointF64> m_vctPtReals;
	std::vector<std::vector<CPointF64>>::iterator m_vct2PtIter;
	std::vector<CPointF64>::iterator m_vctPtIter;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CGTPPainterView();
	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnSaving(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewdraw(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThickness(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPopup(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoading(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // GTPPainterView.cpp의 디버그 버전
inline CGTPPainterDoc* CGTPPainterView::GetDocument() const
{
	return reinterpret_cast<CGTPPainterDoc*>(m_pDocument);
}
#endif
