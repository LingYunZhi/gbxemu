#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
// ms-help://MS.VSCC.v90/MS.MSDNQTR.v90.en/winprog/winprog/using_the_windows_headers.htm

#include <sdkddkver.h>

// New versioning macro
#ifndef NTDDI_VERSION
#define NTDDI_WIN2K
#endif

// Older versioning macro
#ifndef _WIN32_WINNT
#define _WIN32_WINNT  _WIN32_WINNT_WIN2K
#endif

// Set WINVER to the same value as _WIN32_WINNT
#ifndef WINVER
#define WINVER  _WIN32_WINNT
#endif

// Set _WIN32_WINDOWS to the same value as _WIN32_WINNT
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS  _WIN32_WINNT
#endif

// We do not care about Internet Explorer
#ifndef _WIN32_IE
#define _WIN32_IE  _WIN32_IE_IE20
#endif
