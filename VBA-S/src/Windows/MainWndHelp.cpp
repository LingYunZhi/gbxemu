#include "stdafx.h"
#include "MainWnd.h"

#include "AboutDialog.h"

extern int emulating;

void MainWnd::OnHelpAbout()
{
  AboutDialog dlg(this);
  dlg.DoModal();
}
