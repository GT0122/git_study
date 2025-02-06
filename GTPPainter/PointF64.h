#pragma once

class CPointF64 : public CPoint
{
public:
	CPointF64();
	CPointF64(const CPointF64& ptRefPoint);
	CPointF64(double f64X, double f64Y);
	CPointF64(double f64X, double f64Y, int32_t i32LineThickness, COLORREF sLineColor);
	virtual ~CPointF64();

public:
	const CPointF64& operator=(const CPointF64& ptdPosition);
	const CPointF64& operator=(const CPoint& ptPosition);
	operator CPoint() const;

public:
	double m_f64X;
	double m_f64Y;
	int32_t m_i32LineThickness;
	COLORREF m_sLineColor;
};