#include "stdafx.h"
#include "VBA.h"

static char buffer[2048];
static HKEY vbKey = NULL;
static CString *regVbaPath = NULL;

#define VBA_PREF "preferences"

bool regEnabled = true;

void regInit(const char *path, bool force)
{
	if( regEnabled ) {
  DWORD disp = 0;
  LONG res = RegCreateKeyEx(HKEY_CURRENT_USER,
                            "Software\\Emulators\\VisualBoyAdvance",
                            0,
                            "",
                            REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS,
                            NULL,
                            &vbKey,
                            &disp);
	}
  if( regVbaPath != NULL ) {
	  delete regVbaPath;
	  regVbaPath = NULL;
  }

  // If vba.ini exists in executable's folder, use it. Else create/use one in %appdata% folder.
  regVbaPath = new CString();
  regVbaPath->Format("%s\\vba.ini", path);
  if( !force && !utilFileExists( regVbaPath->GetString() ) ) {
	  TCHAR appdata[MAX_PATH+1];
	  SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata );
	  regVbaPath->Format( "%s\\VBA-M", appdata );
	  SHCreateDirectoryEx( NULL, regVbaPath->GetString(), NULL );
	  regVbaPath->Append( "\\vba.ini" );
  }
}

void regShutdown()
{
  LONG res = RegCloseKey(vbKey);
}

const char *regGetINIPath()
{
  return *regVbaPath;
}

char *regQueryStringValue(const char * key, char *def)
{
  if(regEnabled) {
    DWORD type = 0;
    DWORD size = 2048;

    LONG res = RegQueryValueEx(vbKey,
                               key,
                               NULL,
                               &type,
                               (UCHAR *)buffer,
                               &size);

    if(res == ERROR_SUCCESS && type == REG_SZ)
      return buffer;

    return def;
  }

  DWORD res = GetPrivateProfileString(VBA_PREF,
                                      key,
                                      def,
                                      (LPTSTR)buffer,
                                      2048,
                                      *regVbaPath);

  if(res)
    return buffer;

  return def;
}

DWORD regQueryDwordValue(const char * key, DWORD def, bool force)
{
  if(regEnabled || force) {
    DWORD type = 0;
    DWORD size = sizeof(DWORD);
    DWORD result = 0;

    LONG res = RegQueryValueEx(vbKey,
                               key,
                               NULL,
                               &type,
                               (UCHAR *)&result,
                               &size);

    if(res == ERROR_SUCCESS && type == REG_DWORD)
      return result;

    return def;
  }

  return GetPrivateProfileInt(VBA_PREF,
                              key,
                              def,
                              *regVbaPath);
}

BOOL regQueryBinaryValue(const char * key, char *value, int count)
{
  if(regEnabled) {
    DWORD type = 0;
    DWORD size = count;
    DWORD result = 0;


    LONG res = RegQueryValueEx(vbKey,
                               key,
                               NULL,
                               &type,
                               (UCHAR *)value,
                               &size);

    if(res == ERROR_SUCCESS && type == REG_BINARY)
      return TRUE;

    return FALSE;
  }
  CString k = key;
  k += "Count";
  int size = GetPrivateProfileInt(VBA_PREF,
                                  k,
                                  -1,
                                  *regVbaPath);
  if(size >= 0 && size < count)
    count = size;
  return GetPrivateProfileStruct(VBA_PREF,
                                 key,
                                 value,
                                 count,
                                 *regVbaPath);
}

void regSetStringValue(const char * key, const char * value)
{
  if(regEnabled) {
    LONG res = RegSetValueEx(vbKey,
                             key,
                             NULL,
                             REG_SZ,
                             (const UCHAR *)value,
                             lstrlen(value)+1);
  } else {
    WritePrivateProfileString(VBA_PREF,
                              key,
                              value,
                              *regVbaPath);
  }
}

void regSetDwordValue(const char * key, DWORD value, bool force)
{
  if(regEnabled || force) {
    LONG res = RegSetValueEx(vbKey,
                             key,
                             NULL,
                             REG_DWORD,
                             (const UCHAR *)&value,
                             sizeof(DWORD));
  } else {
    wsprintf(buffer, "%u", value);
    WritePrivateProfileString(VBA_PREF,
                              key,
                              buffer,
                              *regVbaPath);
  }
}

void regSetBinaryValue(const char *key, char *value, int count)
{
  if(regEnabled) {
    LONG res = RegSetValueEx(vbKey,
                             key,
                             NULL,
                             REG_BINARY,
                             (const UCHAR *)value,
                             count);
  } else {
    CString k = key;
    k += "Count";
    wsprintf(buffer, "%u", count);

    WritePrivateProfileString(VBA_PREF,
                              k,
                              buffer,
                              *regVbaPath);

    WritePrivateProfileStruct(VBA_PREF,
                              key,
                              value,
                              count,
                              *regVbaPath);
  }
}

void regDeleteValue(char *key)
{
  if(regEnabled) {
    LONG res = RegDeleteValue(vbKey,
                              key);
  } else {
    WritePrivateProfileString(VBA_PREF,
                              key,
                              NULL,
                              *regVbaPath);
  }
}
