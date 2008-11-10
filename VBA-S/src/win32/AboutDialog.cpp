#include "AboutDialog.h"
#include "../AutoBuild.h"
#include "resource.h"

AboutDialog::AboutDialog(CWnd* pParent)
  : CDialog(IDD_ABOUT, pParent)
{
  m_version = _T(VERSION);
  m_date = _T(__DATE__);
}

void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_VERSION, m_version);
  DDX_Text(pDX, IDC_DATE, m_date);
  DDX_Control(pDX, IDC_URL, m_link);
  DDX_Control(pDX, IDC_URL_GPL, m_link_gpl);
}
