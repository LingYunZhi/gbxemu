#pragma once

#include "stdafx.h"
#include "Hyperlink.h"

class AboutDialog : public CDialog
{
public:
  AboutDialog(CWnd* pParent);

private:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  CString   m_version;
  CString   m_date;
  Hyperlink m_link;
  Hyperlink m_link_gpl;
};
