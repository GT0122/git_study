// GTPPainterView.cpp: CGTPPainterView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "GTPPainter.h"
#endif

#include "GTPPainterDoc.h"
#include "GTPPainterView.h"
#include "PointF64.h"
#include "MainFrm.h"
#include "vector"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGTPPainterView

IMPLEMENT_DYNCREATE(CGTPPainterView, CView)

BEGIN_MESSAGE_MAP(CGTPPainterView, CView)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_MESSAGE(GTPPainter_SAVING, &CGTPPainterView::OnSaving)
	ON_MESSAGE(GTPPainter_NEWDRAW, &CGTPPainterView::OnNewdraw)
	ON_MESSAGE(GTPPainter_THICKNESS, &CGTPPainterView::OnThickness)
	ON_MESSAGE(GTPPainter_COLOR, &CGTPPainterView::OnColor)
	ON_MESSAGE(GTPPainter_POPUP, &CGTPPainterView::OnPopup)
	ON_MESSAGE(GTPPainter_LOADING, &CGTPPainterView::OnLoading)
END_MESSAGE_MAP()

// CGTPPainterView 생성/소멸

CGTPPainterView::CGTPPainterView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다
	m_bTitleChange = false;
	m_i32WindowCount = 0;
	m_f64CanvasCurX = 0;
	m_f64CanvasCurY = 0;
	m_i32PrevWidth = 0;
	m_i32PrevHeight = 0;
	m_f64Scale = 1.0;
	m_f64HalfX = 0;
	m_f64HalfY = 0;
	m_i32Thickness = 1;
	m_sLineColor = RGB(0, 0, 0);
	m_vct2PtReals.push_back(m_vctPtReals);
	m_bDrawn = false;
	m_bSaved = false;
	m_i32Count = 1;
}

CGTPPainterView::~CGTPPainterView()
{
}

BOOL CGTPPainterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CGTPPainterView 그리기

void CGTPPainterView::OnDraw(CDC* pDC)
{
	CGTPPainterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CDC dcMemory;

	dcMemory.CreateCompatibleDC(pDC);

	CRect rViewRect;
	CBitmap bmpNewBitmap;

	GetClientRect(&rViewRect);

	bmpNewBitmap.CreateCompatibleBitmap(pDC, rViewRect.Width(), rViewRect.Height());

	CBitmap* pBmpOldmap = nullptr;
	pBmpOldmap = dcMemory.SelectObject(&bmpNewBitmap);

	dcMemory.PatBlt(0, 0, rViewRect.Width(), rViewRect.Height(), WHITENESS);

	std::vector<std::vector<CPointF64>> vct2PtdTemp;
	std::vector<std::vector<CPointF64>>::iterator vct2PtdIter;

	for(m_vct2PtIter = m_vct2PtReals.begin(); m_vct2PtIter < m_vct2PtReals.end(); m_vct2PtIter++)
	{
		if(GetLineInView(&*m_vct2PtIter, &vct2PtdTemp))
		{
			for(vct2PtdIter = vct2PtdTemp.begin(); vct2PtdIter < vct2PtdTemp.end(); vct2PtdIter++)
			{
				m_vctPtIter = vct2PtdIter->begin();
				m_ptdTemp = ConvertRealToCanvas(m_vctPtIter[0]);

				CPen penNew(PS_SOLID, m_vctPtIter[0].m_i32LineThickness, m_vctPtIter[0].m_sLineColor);
				dcMemory.SelectObject(&penNew);

				dcMemory.SetPixelV((int32_t)m_ptdTemp.m_f64X, (int32_t)m_ptdTemp.m_f64Y, m_ptdTemp.m_sLineColor);
				dcMemory.MoveTo((int32_t)m_ptdTemp.m_f64X, (int32_t)m_ptdTemp.m_f64Y);

				m_vctPtIter++;

				for(m_vctPtIter; m_vctPtIter < vct2PtdIter->end(); m_vctPtIter++)
				{
					m_ptdTemp = ConvertRealToCanvas(*m_vctPtIter);

					CPen penNew(PS_SOLID, m_ptdTemp.m_i32LineThickness, m_ptdTemp.m_sLineColor);
					dcMemory.SelectObject(&penNew);

					dcMemory.LineTo((int32_t)m_ptdTemp.m_f64X, (int32_t)m_ptdTemp.m_f64Y);
					dcMemory.MoveTo((int32_t)m_ptdTemp.m_f64X, (int32_t)m_ptdTemp.m_f64Y);
				}
			}
		}
	}

	vct2PtdTemp.clear();

	if(!m_bTitleChange && m_bDrawn)
	{
		CString strTitle, strApp;
		strTitle = GetDocument()->GetTitle();
		int32_t i32Temp = strApp.LoadStringW(AFX_IDS_APP_TITLE);

		GetParent()->SetWindowTextW('*' + strTitle + " - " + strApp);

		m_bTitleChange = true;

		GetParent()->PostMessage(GTPPainter_DRAWING, true, true);
	}

	pDC->BitBlt(0, 0, rViewRect.Width(), rViewRect.Height(), &dcMemory, 0, 0, SRCCOPY);
	dcMemory.SelectObject(pBmpOldmap);
	dcMemory.DeleteDC();
	bmpNewBitmap.DeleteObject();
}

#ifdef _DEBUG
void CGTPPainterView::AssertValid() const
{
	CView::AssertValid();
}

void CGTPPainterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGTPPainterDoc* CGTPPainterView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGTPPainterDoc)));
	return (CGTPPainterDoc*)m_pDocument;
}
#endif //_DEBUG


// CGTPPainterView 메시지 처리기
void CGTPPainterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ptdTemp = CPointF64(point.x, point.y, m_i32Thickness, m_sLineColor);

	m_vct2PtReals[m_vct2PtReals.size() - 1].push_back(ConvertCanvasToReal(m_ptdTemp));

	m_bDrawn = true;

	if(m_bSaved)
	{
		m_bTitleChange = false;
		m_bSaved = false;
	}

	SetCapture();

	Invalidate(FALSE);
}

void CGTPPainterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_vctPtReals.clear();

	m_vct2PtReals.push_back(m_vctPtReals);

	m_bDrawn = false;

	ReleaseCapture();
}

void CGTPPainterView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nFlags & MK_MBUTTON)
	{
		m_ptdOffset.m_f64X = (point.x - m_f64CanvasCurX) / m_f64Scale;
		m_ptdOffset.m_f64Y = (point.y - m_f64CanvasCurY) / m_f64Scale;

		CRect rView;

		GetClientRect(&rView);

		m_ptdViewZero = ConvertCanvasToReal(CPointF64(0, 0));
		m_ptdViewXY = ConvertCanvasToReal(CPointF64(rView.Width(), rView.Height()));

		Invalidate(FALSE);
	}
	else if(nFlags & MK_LBUTTON && m_bDrawn)
	{
		if(m_i32Count == 0)
			m_bTitleChange = false;

		m_ptdTemp = CPointF64(point.x, point.y, m_i32Thickness, m_sLineColor);

		m_vct2PtReals[m_vct2PtReals.size() - 1].push_back(ConvertCanvasToReal(m_ptdTemp));

		m_i32Count++;

		Invalidate(FALSE);
	}
}

void CGTPPainterView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_ptdCursor = ConvertCanvasToReal(CPointF64(point.x, point.y));
	m_f64CanvasCurX = point.x - m_ptdOffset.m_f64X * m_f64Scale;
	m_f64CanvasCurY = point.y - m_ptdOffset.m_f64Y * m_f64Scale;

	SetCapture();

	Invalidate(FALSE);
}

void CGTPPainterView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(!(nFlags & MK_LBUTTON))
		ReleaseCapture();
}

CPointF64 CGTPPainterView::ConvertCanvasToReal(CPointF64 ptdInput)
{
	ptdInput.m_f64X = (ptdInput.m_f64X - m_f64CanvasCurX) / m_f64Scale + m_ptdCursor.m_f64X - m_ptdOffset.m_f64X;
	ptdInput.m_f64Y = (ptdInput.m_f64Y - m_f64CanvasCurY) / m_f64Scale + m_ptdCursor.m_f64Y - m_ptdOffset.m_f64Y;

	return ptdInput;
}

CPointF64 CGTPPainterView::ConvertRealToCanvas(CPointF64 ptdInput)
{
	ptdInput.m_f64X = (ptdInput.m_f64X - m_ptdCursor.m_f64X + m_ptdOffset.m_f64X) * m_f64Scale + m_f64CanvasCurX;
	ptdInput.m_f64Y = (ptdInput.m_f64Y - m_ptdCursor.m_f64Y + m_ptdOffset.m_f64Y) * m_f64Scale + m_f64CanvasCurY;

	return ptdInput;
}

BOOL CGTPPainterView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ScreenToClient(&pt);

	CRect rView;

	GetClientRect(&rView);

	if(zDelta > 0)
	{
		if(m_f64Scale < pow(10, 5))
		{
			m_ptdCursor = ConvertCanvasToReal(CPointF64(pt.x, pt.y));
			m_f64CanvasCurX = pt.x;
			m_f64CanvasCurY = pt.y;
			m_ptdOffset.m_f64X = 0;
			m_ptdOffset.m_f64Y = 0;

			if(m_bDrawn)
			{
				m_ptdTemp = ConvertCanvasToReal(CPointF64(pt.x, pt.y, m_i32Thickness, m_sLineColor));
				m_vct2PtReals[m_vct2PtReals.size() - 1].push_back(m_ptdTemp);
			}

			if(m_f64Scale * 1.5 < pow(10, 5))
				m_f64Scale *= 1.5;
			else
				m_f64Scale = pow(10, 5);

			m_ptdViewZero = ConvertCanvasToReal(CPointF64(0, 0));
			m_ptdViewXY = ConvertCanvasToReal(CPointF64(rView.Width(), rView.Height()));

			Invalidate(FALSE);
		}
	}
	else
	{
		if(m_f64Scale > pow(10, -5))
		{
			m_ptdCursor = ConvertCanvasToReal(CPointF64(pt.x, pt.y));
			m_f64CanvasCurX = pt.x;
			m_f64CanvasCurY = pt.y;
			m_ptdOffset.m_f64X = 0;
			m_ptdOffset.m_f64Y = 0;

			if(m_bDrawn)
			{
				m_ptdTemp = ConvertCanvasToReal(CPointF64(pt.x, pt.y, m_i32Thickness, m_sLineColor));
				m_vct2PtReals[m_vct2PtReals.size() - 1].push_back(m_ptdTemp);
			}

			if(m_f64Scale * 0.7 > pow(10, -5))
				m_f64Scale *= 0.7;
			else
				m_f64Scale = pow(10, -5);

			m_ptdViewZero = ConvertCanvasToReal(CPointF64(0, 0));
			m_ptdViewXY = ConvertCanvasToReal(CPointF64(rView.Width(), rView.Height()));

			Invalidate(FALSE);
		}
	}

	return true;
}

void CGTPPainterView::OnSize(UINT nType, int cx, int cy)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_i32WindowCount < 3)
	{
		m_f64HalfX = 0;
		m_f64HalfY = 0;
		m_i32PrevWidth = cx;
		m_i32PrevHeight = cy;
		m_ptdViewZero = CPointF64(0, 0);
		m_ptdViewXY = CPointF64(cx, cy);
		m_i32WindowCount++;
	}
	else
	{
		m_f64HalfX = (double)(cx - m_i32PrevWidth) / 2;
		m_f64HalfY = (double)(cy - m_i32PrevHeight) / 2;
		m_ptdOffset.m_f64X += m_f64HalfX / m_f64Scale;
		m_ptdOffset.m_f64Y += m_f64HalfY / m_f64Scale;
		m_ptdViewZero.m_f64X -= m_f64HalfX / m_f64Scale;
		m_ptdViewZero.m_f64Y -= m_f64HalfY / m_f64Scale;
		m_ptdViewXY.m_f64X += m_f64HalfX / m_f64Scale;
		m_ptdViewXY.m_f64Y += m_f64HalfY / m_f64Scale;
		m_i32PrevWidth = cx;
		m_i32PrevHeight = cy;
	}
}

bool CGTPPainterView::IsInView(CPointF64 ptdInput)
{
	bool bInner = false;

	do
	{
		if(ptdInput.m_f64X < m_ptdViewZero.m_f64X || ptdInput.m_f64X > m_ptdViewXY.m_f64X)
			break;

		if(ptdInput.m_f64Y < m_ptdViewZero.m_f64Y || ptdInput.m_f64Y > m_ptdViewXY.m_f64Y)
			break;

		bInner = true;

	} while(false);

	return bInner;
}

bool CGTPPainterView::GetLineInView(std::vector<CPointF64>* pVctPtdLine, std::vector<std::vector<CPointF64>>* pVct2PtdNew)
{
	bool bInner = false;

	do
	{
		if(pVctPtdLine->size() == 0)
			break;

		bool bPrev, bTemp;
		double f64Diff, f64InterY, f64X, f64Y;
		CPointF64 ptdPrev, ptdTemp;
		std::vector<CPointF64> vctPtdNew;

		ptdPrev = *(pVctPtdLine->begin());
		std::vector<CPointF64>::iterator vctPtdIter = pVctPtdLine->begin();
		bPrev = IsInView(ptdPrev);
		vctPtdIter++;

		if(bPrev)
			vctPtdNew.push_back(ptdPrev);

		for(vctPtdIter; vctPtdIter < pVctPtdLine->end(); vctPtdIter++)
		{
			ptdTemp = *vctPtdIter;
			bTemp = IsInView(ptdTemp);

			if(bTemp)
			{
				if(!bPrev)
				{
					if(ptdPrev.m_f64X == ptdTemp.m_f64X)
					{
						if(ptdPrev.m_f64Y < m_ptdViewZero.m_f64Y)
							vctPtdNew.push_back(CPointF64(ptdPrev.m_f64X, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
						else
							vctPtdNew.push_back(CPointF64(ptdPrev.m_f64X, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
					}
					else
					{
						f64Diff = (ptdTemp.m_f64Y - ptdPrev.m_f64Y) / (ptdTemp.m_f64X - ptdPrev.m_f64X);
						f64InterY = ptdPrev.m_f64Y - ptdPrev.m_f64X * f64Diff;

						if(ptdPrev.m_f64Y < m_ptdViewZero.m_f64Y)
						{
							f64X = (m_ptdViewZero.m_f64Y - f64InterY) / f64Diff;

							if(f64X >= m_ptdViewZero.m_f64X && f64X <= m_ptdViewXY.m_f64X)
								vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							else if(f64X < m_ptdViewZero.m_f64X)
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							}
							else
							{
								f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							}
						}
						else if(ptdPrev.m_f64Y > m_ptdViewXY.m_f64Y)
						{
							f64X = (m_ptdViewXY.m_f64Y - f64InterY) / f64Diff;

							if(f64X >= m_ptdViewZero.m_f64X && f64X <= m_ptdViewXY.m_f64X)
								vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							else if(f64X < m_ptdViewZero.m_f64X)
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							}
							else
							{
								f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							}
						}
						else
						{
							if(ptdPrev.m_f64X < m_ptdViewZero.m_f64X)
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							}
							else
							{
								f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
							}
						}
					}
				}

				vctPtdNew.push_back(ptdTemp);
			}
			else
			{
				if(bPrev)
				{
					if(ptdPrev.m_f64X == ptdTemp.m_f64X)
					{
						if(ptdTemp.m_f64Y < m_ptdViewZero.m_f64Y)
							vctPtdNew.push_back(CPointF64(ptdPrev.m_f64X, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
						else
							vctPtdNew.push_back(CPointF64(ptdPrev.m_f64X, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
					}
					else
					{
						f64Diff = (ptdTemp.m_f64Y - ptdPrev.m_f64Y) / (ptdTemp.m_f64X - ptdPrev.m_f64X);
						f64InterY = ptdTemp.m_f64Y - ptdTemp.m_f64X * f64Diff;

						if(ptdTemp.m_f64Y < m_ptdViewZero.m_f64Y)
						{
							f64X = (m_ptdViewZero.m_f64Y - f64InterY) / f64Diff;

							if(f64X >= m_ptdViewZero.m_f64X && f64X <= m_ptdViewXY.m_f64X)
								vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							else if(f64X < m_ptdViewZero.m_f64X)
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
							else
							{
								f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
						}
						else if(ptdTemp.m_f64Y > m_ptdViewXY.m_f64Y)
						{
							f64X = (m_ptdViewXY.m_f64Y - f64InterY) / f64Diff;

							if(f64X >= m_ptdViewZero.m_f64X && f64X <= m_ptdViewXY.m_f64X)
								vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							else if(f64X < m_ptdViewZero.m_f64X)
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
							else
							{
								f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
						}
						else
						{
							if(ptdTemp.m_f64X < m_ptdViewZero.m_f64X)
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
							else
							{
								f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;
								vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
						}
					}
				}
				else
				{
					if(ptdTemp.m_f64X == ptdPrev.m_f64X)
					{
						if((ptdPrev.m_f64X >= m_ptdViewZero.m_f64X && ptdTemp.m_f64X <= m_ptdViewXY.m_f64X) || (ptdTemp.m_f64X >= m_ptdViewZero.m_f64X && ptdPrev.m_f64X <= m_ptdViewXY.m_f64X))
						{
							if(ptdPrev.m_f64Y < m_ptdViewZero.m_f64Y && ptdTemp.m_f64Y > m_ptdViewXY.m_f64Y)
							{
								vctPtdNew.push_back(CPointF64(ptdPrev.m_f64X, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
								vctPtdNew.push_back(CPointF64(ptdTemp.m_f64X, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
							else if(ptdPrev.m_f64Y > m_ptdViewXY.m_f64Y && ptdTemp.m_f64Y < m_ptdViewZero.m_f64Y)
							{
								vctPtdNew.push_back(CPointF64(ptdPrev.m_f64X, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
								vctPtdNew.push_back(CPointF64(ptdTemp.m_f64X, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
							}
						}
					}
					else
					{
						f64Diff = (ptdTemp.m_f64Y - ptdPrev.m_f64Y) / (ptdTemp.m_f64X - ptdPrev.m_f64X);
						f64InterY = ptdPrev.m_f64Y - ptdPrev.m_f64X * f64Diff;

						if(f64Diff > 0)
						{
							if(!(ptdPrev.m_f64X < m_ptdViewZero.m_f64X && ptdTemp.m_f64X < m_ptdViewZero.m_f64X) && !(ptdPrev.m_f64X > m_ptdViewXY.m_f64X && ptdTemp.m_f64X > m_ptdViewXY.m_f64X) && !(ptdPrev.m_f64Y < m_ptdViewZero.m_f64Y && ptdTemp.m_f64Y < m_ptdViewZero.m_f64Y) && !(ptdPrev.m_f64Y > m_ptdViewXY.m_f64Y && ptdTemp.m_f64Y > m_ptdViewXY.m_f64Y))
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;

								if(f64Y == m_ptdViewXY.m_f64Y)
									vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
								else if(f64Y >= m_ptdViewZero.m_f64Y && f64Y < m_ptdViewXY.m_f64Y)
								{
									f64X = (m_ptdViewXY.m_f64Y - f64InterY) / f64Diff;

									if(f64X < m_ptdViewXY.m_f64X)
									{
										if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
										{
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
										else
										{
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
									}
									else
									{
										double f64YTemp = m_ptdViewXY.m_f64X * f64Diff + f64InterY;

										if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
										{
											vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64YTemp, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
										else
										{
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64YTemp, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
									}
								}
								else if(f64Y < m_ptdViewZero.m_f64Y)
								{
									f64X = (m_ptdViewZero.m_f64Y - f64InterY) / f64Diff;
									double f64XTemp = (m_ptdViewXY.m_f64Y - f64InterY) / f64Diff;

									if(f64XTemp <= m_ptdViewXY.m_f64X)
									{
										if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
										{
											vctPtdNew.push_back(CPointF64(f64XTemp, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
										else
										{
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(f64XTemp, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
									}
									else
									{
										f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;

										if(f64Y > m_ptdViewZero.m_f64Y)
										{
											if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
											{
												vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
												vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
											}
											else
											{
												vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
												vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
											}
										}
										else if(f64Y == m_ptdViewZero.m_f64Y)
											vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
									}
								}
							}
						}
						else if(f64Diff < 0)
						{
							if(!(ptdPrev.m_f64X < m_ptdViewZero.m_f64X && ptdTemp.m_f64X < m_ptdViewZero.m_f64X) && !(ptdPrev.m_f64X > m_ptdViewXY.m_f64X && ptdTemp.m_f64X > m_ptdViewXY.m_f64X) && !(ptdPrev.m_f64Y < m_ptdViewZero.m_f64Y && ptdTemp.m_f64Y < m_ptdViewZero.m_f64Y) && !(ptdPrev.m_f64Y > m_ptdViewXY.m_f64Y && ptdTemp.m_f64Y > m_ptdViewXY.m_f64Y))
							{
								f64Y = m_ptdViewZero.m_f64X * f64Diff + f64InterY;

								if(f64Y == m_ptdViewZero.m_f64Y)
									vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
								else if(f64Y > m_ptdViewZero.m_f64Y && f64Y <= m_ptdViewXY.m_f64Y)
								{
									f64X = (m_ptdViewZero.m_f64Y - f64InterY) / f64Diff;

									if(f64X < m_ptdViewXY.m_f64X)
									{
										if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
										{
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
										else
										{
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
									}
									else
									{
										double f64YTemp = m_ptdViewXY.m_f64X * f64Diff + f64InterY;

										if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
										{
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64YTemp, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
										else
										{
											vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64YTemp, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
									}
								}
								else if(f64Y > m_ptdViewXY.m_f64Y)
								{
									f64X = (m_ptdViewXY.m_f64Y - f64InterY) / f64Diff;
									double f64XTemp = (m_ptdViewZero.m_f64Y - f64InterY) / f64Diff;

									if(f64XTemp <= m_ptdViewXY.m_f64X)
									{
										if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
										{
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(f64XTemp, m_ptdViewZero.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
										else
										{
											vctPtdNew.push_back(CPointF64(f64XTemp, m_ptdViewZero.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
											vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
										}
									}
									else if(f64XTemp > m_ptdViewXY.m_f64X)
									{
										f64Y = m_ptdViewXY.m_f64X * f64Diff + f64InterY;

										if(f64Y < m_ptdViewXY.m_f64Y)
										{
											if(ptdPrev.m_f64Y > ptdTemp.m_f64Y)
											{
												vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
												vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
											}
											else
											{
												vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
												vctPtdNew.push_back(CPointF64(f64X, m_ptdViewXY.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
											}
										}
										else if(f64Y == m_ptdViewXY.m_f64Y)
											vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
									}
								}
							}
						}
						else
						{
							if((ptdPrev.m_f64Y >= m_ptdViewZero.m_f64Y && ptdTemp.m_f64Y <= m_ptdViewXY.m_f64Y) || (ptdTemp.m_f64Y >= m_ptdViewZero.m_f64Y && ptdPrev.m_f64Y <= m_ptdViewXY.m_f64Y))
							{
								if(ptdPrev.m_f64X < m_ptdViewZero.m_f64X && ptdTemp.m_f64X > m_ptdViewXY.m_f64X)
								{
									vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, ptdPrev.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
									vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, ptdPrev.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
								}
								else if(ptdPrev.m_f64X > m_ptdViewXY.m_f64X && ptdTemp.m_f64X < m_ptdViewZero.m_f64X)
								{
									vctPtdNew.push_back(CPointF64(m_ptdViewXY.m_f64X, ptdPrev.m_f64Y, ptdPrev.m_i32LineThickness, ptdPrev.m_sLineColor));
									vctPtdNew.push_back(CPointF64(m_ptdViewZero.m_f64X, ptdPrev.m_f64Y, ptdTemp.m_i32LineThickness, ptdTemp.m_sLineColor));
								}
							}
						}
					}
				}

				if(vctPtdNew.size() > 0)
				{
					pVct2PtdNew->push_back(vctPtdNew);
					vctPtdNew.clear();
				}
			}

			ptdPrev = ptdTemp;
			bPrev = bTemp;
		}

		if(vctPtdNew.size() > 0)
		{
			pVct2PtdNew->push_back(vctPtdNew);
			vctPtdNew.clear();
		}

		bInner = true;

	}
	while(false);

	return bInner;
}

uint32_t CGTPPainterView::ConvertToLittleEndian(int32_t i32Value, int8_t* pI8Char)
{
	uint32_t u32One, u32Two, u32Three, u32Four;
	u32One = i32Value & 255;
	u32Two = (i32Value >> 8) & 255;
	u32Three = (i32Value >> 16) & 255;
	u32Four = (i32Value >> 24) & 255;

	pI8Char[0] = u32One;
	pI8Char[1] = u32Two;
	pI8Char[2] = u32Three;
	pI8Char[3] = u32Four;

	return ((uint8_t)u32One << 24) + ((uint8_t)u32Two << 16) + ((uint8_t)u32Three << 8) + u32Four;
}

uint32_t CGTPPainterView::ConvertToLittleEndian(COLORREF sValue, int8_t* pI8Char)
{
	uint32_t u32One, u32Two, u32Three, u32Four;
	u32One = GetRValue(sValue);
	u32Two = GetGValue(sValue);
	u32Three = GetBValue(sValue);
	u32Four = 0;

	pI8Char[0] = u32One;
	pI8Char[1] = u32Two;
	pI8Char[2] = u32Three;
	pI8Char[3] = u32Four;

	return ((int32_t)u32One << 24) + ((int32_t)u32Two << 16) + ((int32_t)u32Three << 8) + u32Four;
}

COLORREF CGTPPainterView::ConvertToColor(int8_t* pI8Char)
{
	uint32_t u32One, u32Two, u32Three, u32Four;
	u32One = pI8Char[0] & 255;
	u32Two = pI8Char[1] & 255;
	u32Three = pI8Char[2] & 255;
	u32Four = pI8Char[3] & 255;

	return RGB(u32One, u32Two, u32Three);
}

uint32_t CGTPPainterView::ConvertToInt(int8_t* pI8Char)
{
	uint32_t u32One, u32Two, u32Three, u32Four;
	u32One = pI8Char[0] & 255;
	u32Two = pI8Char[1] & 255;
	u32Three = pI8Char[2] & 255;
	u32Four = pI8Char[3] & 255;

	return ((int32_t)u32Four << 24) + ((int32_t)u32Three << 16) + ((int32_t)u32Two << 8) + u32One;
}

double CGTPPainterView::ConvertToEndian(double f64Value, int8_t* pI8Char)
{
	int8_t* pI8Temp = reinterpret_cast<int8_t*>(&f64Value);

	for(size_t i = 0; i < sizeof(double); i++)
		pI8Char[i] = pI8Temp[i];

	return f64Value;
}

double CGTPPainterView::ConvertToDouble(double f64Value, int8_t* pI8Char)
{
	int8_t* pI8Temp = reinterpret_cast<int8_t*>(&f64Value);

	for(size_t i = 0; i < sizeof(double); i++)
		pI8Temp[i] = pI8Char[i];

	return f64Value;
}

afx_msg LRESULT CGTPPainterView::OnSaving(WPARAM wParam, LPARAM lParam)
{
	m_bSaved = true;
	m_i32Count = -1;

	int8_t* arrI8FL = nullptr;
	arrI8FL = (int8_t*)"FourthLogic";
	FILE* pFSave = nullptr;
	int32_t i32Size = sizeof(double) * 2 + sizeof(uint32_t) * 2;
	uint32_t u32Temp;
	double f64Temp;
	int8_t arrI8Int[sizeof(uint32_t)] = {0};
	int8_t arrI8Double[sizeof(double)] = {0};

	uint32_t u32Crc32val = 0;
	u32Crc32val ^= 0xFFFFFFFF;

	CString strName = GetDocument()->GetTitle();
	CString strPath = GetDocument()->GetPathName();
	fopen_s(&pFSave, strPath + '/' + strName + ".flp", "wb");

	if(pFSave)
	{
		CalcCrc32(&u32Crc32val, arrI8FL, sizeof(int8_t) * 11);
		fwrite(arrI8FL, sizeof(int8_t), 11, pFSave);
		
		u32Temp = ConvertToLittleEndian(i32Size, arrI8Int);
		CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
		fwrite(arrI8Int, sizeof(uint32_t), 1, pFSave);
		
		u32Temp = ConvertToLittleEndian((int32_t)m_vct2PtReals.size(), arrI8Int);
		CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
		fwrite(arrI8Int, sizeof(uint32_t), 1, pFSave);
		
		for(m_vct2PtIter = m_vct2PtReals.begin(); m_vct2PtIter < m_vct2PtReals.end(); m_vct2PtIter++)
		{
			u32Temp = ConvertToLittleEndian((int32_t)m_vct2PtIter->size(), arrI8Int);
			CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
			fwrite(&arrI8Int, sizeof(uint32_t), 1, pFSave);
			
			for(m_vctPtIter = m_vct2PtIter->begin(); m_vctPtIter < m_vct2PtIter->end(); m_vctPtIter++)
			{
				f64Temp = ConvertToEndian(m_vctPtIter->m_f64X, arrI8Double);
				CalcCrc32(&u32Crc32val, arrI8Double, sizeof(double));
				fwrite(&arrI8Double, sizeof(double), 1, pFSave);

				f64Temp = ConvertToEndian(m_vctPtIter->m_f64Y, arrI8Double);
				CalcCrc32(&u32Crc32val, arrI8Double, sizeof(double));
				fwrite(&arrI8Double, sizeof(double), 1, pFSave);

				u32Temp = ConvertToLittleEndian(m_vctPtIter->m_i32LineThickness, arrI8Int);
				CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
				fwrite(&arrI8Int, sizeof(uint32_t), 1, pFSave);

				u32Temp = ConvertToLittleEndian(m_vctPtIter->m_sLineColor, arrI8Int);
				CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
				fwrite(&arrI8Int, sizeof(uint32_t), 1, pFSave);
			}
		}

		u32Crc32val = u32Crc32val ^ 0xFFFFFFFF;
		fwrite(&u32Crc32val, sizeof(uint32_t), 1, pFSave);

		fclose(pFSave);

		if(m_vct2PtReals.size() == 1 && m_vct2PtReals[m_vct2PtReals.size() - 1].size() == 0)
			GetParent()->PostMessage(GTPPainter_DRAWING, false, false);
		else
			GetParent()->PostMessage(GTPPainter_DRAWING, true, false);

		if((int32_t)wParam == -1)
			GetParent()->PostMessage(GTPPainter_CLOSING);
	}
	else
	{
		MessageBoxW(_T("Failed to save the file."), MB_OK);
		GetParent()->PostMessage(GTPPainter_DRAWING, true, true);
	}

	return 0;
}

afx_msg LRESULT CGTPPainterView::OnNewdraw(WPARAM wParam, LPARAM lParam)
{
	m_vctPtReals.clear();
	m_vct2PtReals.clear();

	m_bTitleChange = false;
	m_f64CanvasCurX = 0;
	m_f64CanvasCurY = 0;
	m_f64Scale = 1.0;
	m_f64HalfX = 0;
	m_f64HalfY = 0;
	m_vct2PtReals.push_back(m_vctPtReals);
	m_bDrawn = false;
	m_bSaved = false;
	m_i32Count = 1;

	CRect rView;
	GetClientRect(&rView);

	m_ptdViewZero.m_f64X = 0;
	m_ptdViewZero.m_f64Y = 0;
	m_ptdViewXY.m_f64X = rView.Width();
	m_ptdViewXY.m_f64Y = rView.Height();
	m_i32PrevWidth = rView.Width();
	m_i32PrevHeight = rView.Height();

	m_ptdOffset.m_f64X = 0;
	m_ptdOffset.m_f64Y = 0;
	m_ptdCursor.m_f64X = 0;
	m_ptdCursor.m_f64Y = 0;

	Invalidate(FALSE);

	GetParent()->PostMessage(GTPPainter_DRAWING, false, false);

	return 0;
}

afx_msg LRESULT CGTPPainterView::OnThickness(WPARAM wParam, LPARAM lParam)
{
	m_i32Thickness = (int32_t)wParam;

	return 0;
}

afx_msg LRESULT CGTPPainterView::OnColor(WPARAM wParam, LPARAM lParam)
{
	m_sLineColor = (COLORREF)wParam;

	return 0;
}

afx_msg LRESULT CGTPPainterView::OnPopup(WPARAM wParam, LPARAM lParam)
{
	m_vctPtReals.clear();

	m_vct2PtReals.push_back(m_vctPtReals);

	m_bDrawn = false;

	ReleaseCapture();

	return 0;
}

afx_msg LRESULT CGTPPainterView::OnLoading(WPARAM wParam, LPARAM lParam)
{
	int8_t arrI8FL[11];
	FILE* pFLoad = nullptr;
	
	uint32_t u32Total, u32Line, u32Thick;
	double f64X = 0, f64Y = 0;
	COLORREF sColorTemp;
	
	int8_t arrI8Int[sizeof(uint32_t)] = { 0 };
	int8_t arrI8Double[sizeof(double)] = { 0 };
	uint32_t u32Crc32save;
	uint32_t u32Crc32val = 0;
	uint32_t u32Crc32header = 0;
	u32Crc32val ^= 0xFFFFFFFF;
	u32Crc32header ^= 0xFFFFFFFF;

	CString* pstrName = nullptr;
	pstrName = (CString*)lParam;
	CString strName = pstrName->GetString();
	CString strPath = GetDocument()->GetPathName();
	fopen_s(&pFLoad, strPath + '/' + strName + ".flp", "rb");

	std::vector<std::vector<CPointF64>> vct2PtTemp;
	std::vector<CPointF64> vctPtTemp;

	if(pFLoad)
	{
		fread(&arrI8FL, sizeof(int8_t), 11, pFLoad);
		CalcCrc32(&u32Crc32val, arrI8FL, sizeof(int8_t) * 11);
		CalcCrc32(&u32Crc32header, (int8_t*)"FourthLogic", sizeof(int8_t) * 11);

		if(u32Crc32val == u32Crc32header)
		{
			fread(&arrI8Int, sizeof(uint32_t), 1, pFLoad);
			CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));

			fread(&arrI8Int, sizeof(uint32_t), 1, pFLoad);
			CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
			u32Total = ConvertToInt(arrI8Int);

			for(uint32_t i = 0; i < u32Total; i++)
			{
				fread(&arrI8Int, sizeof(uint32_t), 1, pFLoad);
				CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
				u32Line = ConvertToInt(arrI8Int);

				for(uint32_t j = 0; j < u32Line; j++)
				{
					fread(&arrI8Double, sizeof(double), 1, pFLoad);
					CalcCrc32(&u32Crc32val, arrI8Double, sizeof(double));
					f64X = ConvertToDouble(f64X, arrI8Double);

					fread(&arrI8Double, sizeof(double), 1, pFLoad);
					CalcCrc32(&u32Crc32val, arrI8Double, sizeof(double));
					f64Y = ConvertToDouble(f64Y, arrI8Double);

					fread(&arrI8Int, sizeof(uint32_t), 1, pFLoad);
					CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
					u32Thick = ConvertToInt(arrI8Int);

					fread(&arrI8Int, sizeof(uint32_t), 1, pFLoad);
					CalcCrc32(&u32Crc32val, arrI8Int, sizeof(uint32_t));
					sColorTemp = ConvertToColor(arrI8Int);

					vctPtTemp.push_back(CPointF64(f64X, f64Y, u32Thick, sColorTemp));
				}

				vct2PtTemp.push_back(vctPtTemp);
				vctPtTemp.clear();
			}

			fread(&u32Crc32save, sizeof(uint32_t), 1, pFLoad);
			u32Crc32val = u32Crc32val ^ 0xFFFFFFFF;

			fclose(pFLoad);

			if(u32Crc32save == u32Crc32val)
			{
				SendMessage(GTPPainter_NEWDRAW);

				GetDocument()->SetTitle(strName);

				m_vct2PtReals = vct2PtTemp;
				vct2PtTemp.clear();

				GetParent()->PostMessage(GTPPainter_DRAWING, true, false);
				GetParent()->PostMessage(GTPPainter_OPENING, true);
			}
			else
			{
				MessageBoxW(_T("Failed to load file."), MB_OK);
				GetParent()->PostMessage(GTPPainter_OPENING, false);
			}

			Invalidate(FALSE);
		}
		else
		{
			fclose(pFLoad);

			MessageBoxW(_T("Failed to load file."), MB_OK);
			GetParent()->PostMessage(GTPPainter_OPENING, false);
		}
	}
	else
	{
		MessageBoxW(_T("Failed to load file."), MB_OK);
		GetParent()->PostMessage(GTPPainter_OPENING, false);
	}

	return 0;
}

static const uint32_t g_u32Crc32Tab[] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

void CGTPPainterView::CalcCrc32(uint32_t* pU32Crc32val, int8_t* pI8Char, const int32_t i32Size)
{
	for(int32_t i = 0; i < i32Size; i++)
		*pU32Crc32val = g_u32Crc32Tab[(*pU32Crc32val ^ pI8Char[i]) & 0xFF] ^ ((*pU32Crc32val >> 8) & 0x00FFFFFF);
}