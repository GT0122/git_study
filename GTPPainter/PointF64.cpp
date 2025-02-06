#include "pch.h"
#include "PointF64.h"
#include <cstdint>
#include <windef.h>
#include <wingdi.h>
#include <atltypes.h>
#include <afxstr.h>

CPointF64::CPointF64()
{
	m_f64X = 0;
	m_f64Y = 0;
	m_i32LineThickness = 1;
	m_sLineColor = RGB(0, 0, 0);
}

CPointF64::CPointF64(double f64X, double f64Y)
{
	m_f64X = f64X;
	m_f64Y = f64Y;
	m_i32LineThickness = 1;
	m_sLineColor = RGB(0, 0, 0);
}

CPointF64::CPointF64(const CPointF64& ptdRefPoint)
{
	m_f64X = ptdRefPoint.m_f64X;
	m_f64Y = ptdRefPoint.m_f64Y;
	m_i32LineThickness = ptdRefPoint.m_i32LineThickness;
	m_sLineColor = ptdRefPoint.m_sLineColor;
}

CPointF64::CPointF64(double f64X, double f64Y, int32_t i32LineThickness, COLORREF sLineColor)
{
	m_f64X = f64X;
	m_f64Y = f64Y;
	m_i32LineThickness = i32LineThickness;
	m_sLineColor = sLineColor;
}

CPointF64::~CPointF64()
{
}

const CPointF64& CPointF64::operator=(const CPointF64& ptdPosition)
{
	m_f64X = ptdPosition.m_f64X;
	m_f64Y = ptdPosition.m_f64Y;
	m_i32LineThickness = ptdPosition.m_i32LineThickness;
	m_sLineColor = ptdPosition.m_sLineColor;

	return *this;
}

const CPointF64& CPointF64::operator=(const CPoint& ptPosition)
{
	m_f64X = (double)ptPosition.x;
	m_f64Y = (double)ptPosition.y;

	return *this;
}

CPointF64::operator CPoint() const
{
	CPoint ptTemp = CPoint();

	return ptTemp;
}