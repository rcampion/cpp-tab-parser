/**************************************************************************
 *
 * zCtrlOsInfo.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 **************************************************************************/
/*! \file zCtrlOsInfo.h
*/

#ifndef __ZCTRLOSINFO_H__
#define __ZCTRLOSINFO_H__

//---------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>

class ZCtrlOsVersionInfo
{
	private:
	 	OSVERSIONINFO	osvi;

  public:
  	int IsNt4(void);
    int IsNt5(void);
    int IsWin95(void);
    int IsWin95OSR1(void);
    int IsWin95OSR2(void);
    int IsWin98(void);
    int IsWin98SE(void);

		int IsNt(void);
		int IsWin9x(void);


    int GetBuildNumber(void);
    char *GetOSVersionStr(void);
		char *GetPlatformIdStr(void);

		inline int GetPlatformID(void) { return static_cast<int>(osvi.dwPlatformId); }
    inline int GetMajorVersion(void) { return static_cast<int>(osvi.dwMajorVersion); }
    inline int GetMinorVersion(void) { return static_cast<int>(osvi.dwMinorVersion); }
    inline char *GetExtraVersionStr(void) {return osvi.szCSDVersion; }

    ZCtrlOsVersionInfo(void);
    virtual ~ZCtrlOsVersionInfo(void);

};
#endif
#endif
