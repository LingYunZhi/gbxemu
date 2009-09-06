#pragma once

class RomInfoGBA : public CDialog
{
 public:
  RomInfoGBA(u8 *rom, CWnd* pParent = NULL);

  // Dialog Data
  //{{AFX_DATA(RomInfoGBA)
  enum { IDD = IDD_GBA_ROM_INFO };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA
  u8 *rom;


  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(RomInfoGBA)
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
 protected:

  // Generated message map functions
  //{{AFX_MSG(RomInfoGBA)
  afx_msg void OnOk();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
    };
    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

