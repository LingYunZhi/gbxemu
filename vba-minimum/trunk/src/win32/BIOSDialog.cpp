#include "stdafx.h"
#include "VBA.h"

#include "BIOSDialog.h"


// BIOSDialog dialog

IMPLEMENT_DYNAMIC(BIOSDialog, CDialog)

BIOSDialog::BIOSDialog(CWnd* pParent /*=NULL*/)
	: CDialog(BIOSDialog::IDD, pParent)
	, m_enableBIOS_GBA(FALSE)
	, m_skipLogo(FALSE)
	, m_pathGBA(_T(""))
{
}

BIOSDialog::~BIOSDialog()
{
}

void BIOSDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ENABLE_GBA_BIOS, m_enableBIOS_GBA);
	DDX_Check(pDX, IDC_SKIP_BOOT_LOGO, m_skipLogo);
	DDX_Text(pDX, IDC_GBA_BIOS_PATH, m_pathGBA);
	DDX_Control(pDX, IDC_GBA_BIOS_PATH, m_editGBA);

	if( pDX->m_bSaveAndValidate == TRUE ) {
		// disable BIOS usage when it does not exist
		if( !fileExists( m_pathGBA ) ) {
			m_enableBIOS_GBA = FALSE;
		}
	}
}


BEGIN_MESSAGE_MAP(BIOSDialog, CDialog)
	ON_BN_CLICKED(IDC_SELECT_GBA_BIOS_PATH, &BIOSDialog::OnBnClickedSelectGbaBiosPath)
END_MESSAGE_MAP()


// BIOSDialog message handlers

void BIOSDialog::OnBnClickedSelectGbaBiosPath()
{
	CString current;
	m_editGBA.GetWindowText( current );
	if( !fileExists( current ) ) {
		current = _T("");
	}

	CFileDialog dlg(
		TRUE,
		NULL,
		current,
		OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST,
		_T("BIOS Files (*.bin;*.rom;*.gba)|*.bin;*.rom;*.gba|All Files (*.*)|*.*||"),
		this,
		0 );

	if( IDOK == dlg.DoModal() ) {
		m_editGBA.SetWindowText( dlg.GetPathName() );
	}
}

bool BIOSDialog::fileExists(CString& file)
{
	CFileStatus stat;
	BOOL retVal = CFile::GetStatus( file, stat );
	bool noFile = false;
	if( retVal == TRUE ) {
		noFile |= ( stat.m_attribute & CFile::directory ) != 0;
	}
	return ( retVal == TRUE ) && !noFile;
}
