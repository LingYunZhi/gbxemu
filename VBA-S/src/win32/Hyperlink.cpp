#include "stdafx.h"
#include "VBA.h"
#include "Hyperlink.h"

Hyperlink::Hyperlink()
{
  m_over = false;
}

Hyperlink::~Hyperlink()
{
  m_normalFont.DeleteObject();
  m_underlineFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(Hyperlink, CStatic)
  ON_WM_CTLCOLOR_REFLECT()
  ON_WM_ERASEBKGND()
  ON_WM_MOUSEMOVE()
  ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
END_MESSAGE_MAP()

void Hyperlink::PreSubclassWindow()
{  
  ModifyStyle(0, SS_NOTIFY);
  m_cursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_HAND), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

  CFont *font = GetFont();
  LOGFONT lg;
  font->GetLogFont(&lg);
  m_normalFont.CreateFontIndirect(&lg);
  lg.lfUnderline = TRUE;
  m_underlineFont.CreateFontIndirect(&lg);

  CStatic::PreSubclassWindow();
}

void Hyperlink::OnClicked()
{
  CString url;
  GetWindowText(url);
  ::ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
  SetFont(&m_normalFont);
}

HBRUSH Hyperlink::CtlColor(CDC* pDC, UINT nCtlColor)
{
  pDC->SetTextColor(GetSysColor(COLOR_HOTLIGHT));
  return (HBRUSH)GetStockObject(NULL_BRUSH);
}

BOOL Hyperlink::OnEraseBkgnd(CDC* pDC)
{
  CRect rect;
  GetClientRect(rect);
  pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));
  return TRUE;
}

void Hyperlink::OnMouseMove(UINT nFlags, CPoint point)
{
  if(!m_over) {
    m_over = true;
    SetCapture();
    ::SetCursor(m_cursor);
    SetFont(&m_underlineFont);
  } else {
    CRect r;
    GetClientRect(&r);

    if(!r.PtInRect(point)) {
      m_over = false;
      SetFont(&m_normalFont);
      ReleaseCapture();
    }
  }
}
