#pragma once

class Hyperlink : public CStatic
{
public:
  Hyperlink();
	~Hyperlink();

private:
	bool m_over;
	HCURSOR m_cursor;
	afx_msg void OnClicked();
	CFont m_normalFont;
	CFont m_underlineFont;

  virtual void PreSubclassWindow();

  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

  DECLARE_MESSAGE_MAP()
};
