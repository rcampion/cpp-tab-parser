/**************************************************************************
 *
 * zCtrlOsInfo.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 **************************************************************************/
/*! \file zCtrlOsInfo.cpp
*/

//---------------------------------------------------------------------------
#ifdef WIN32
#pragma hdrstop

#include <stdio.h>					// sprintf()
#include "zCtrlOsInfo.h"

//---------------------------------------------------------------------------
ZCtrlOsVersionInfo::ZCtrlOsVersionInfo(void)
{
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ZCtrlOsVersionInfo::~ZCtrlOsVersionInfo(void)
{
		//
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsNt(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
					(osvi.dwMajorVersion >= 4)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsNt4(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
					(osvi.dwMajorVersion == 4)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsNt5(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
					(osvi.dwMajorVersion == 5)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsWin9x(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
					(osvi.dwMajorVersion == 4)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsWin95(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
					(osvi.dwMajorVersion == 4) &&
          (osvi.dwMinorVersion != 10)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsWin95OSR1(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
					(osvi.dwMajorVersion == 4) &&
          (osvi.dwMinorVersion != 10) &&
          ((osvi.dwBuildNumber & 0x0000FFFF) <= 1000)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsWin95OSR2(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
					(osvi.dwMajorVersion == 4) &&
          (osvi.dwMinorVersion != 10) &&
          ((osvi.dwBuildNumber & 0x0000FFFF) > 1000)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsWin98(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
					(osvi.dwMajorVersion == 4) &&
          (osvi.dwMinorVersion == 10)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::IsWin98SE(void)
{
 	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
					(osvi.dwMajorVersion == 4) &&
          (osvi.dwMinorVersion == 10)
         );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ZCtrlOsVersionInfo::GetBuildNumber(void)
{
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
   	return static_cast<int>(osvi.dwBuildNumber & 0x0000FFFF);
  }
  else
  {
   	return static_cast<int>(osvi.dwBuildNumber);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
char* ZCtrlOsVersionInfo::GetOSVersionStr(void)
{
  char	*versionStr = new char[128];

  if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
  {
		sprintf(versionStr,"Win %d.%d.%d (Win 32s)",
    				osvi.dwMajorVersion,
            osvi.dwMinorVersion,
            osvi.dwBuildNumber);
  }
  else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
  	if (osvi.dwMinorVersion == 10)
    {
    	if ((osvi.dwBuildNumber & 0x0000FFFF) >= 0)
      {
				sprintf(versionStr,"Windows 98 (%d.%d.%d)",
  	    				osvi.dwMajorVersion,
    	          osvi.dwMinorVersion,
      	        (osvi.dwBuildNumber & 0x0000FFFF));
      }
      else
      {
				sprintf(versionStr,"Windows 98 SE (%d.%d.%d)",
  	    				osvi.dwMajorVersion,
    	          osvi.dwMinorVersion,
      	        (osvi.dwBuildNumber & 0x0000FFFF));
      }
    }
    else
    {
    	if ((osvi.dwBuildNumber & 0x0000FFFF) <= 1000)
      {
				sprintf(versionStr,"Windows 95 (%d.%d.%d)",
        				osvi.dwMajorVersion,
                osvi.dwMinorVersion,
                (osvi.dwBuildNumber & 0x0000FFFF));
      }
      else
      {
				sprintf(versionStr,"Windows 95 OSR 2 (%d.%d.%d)",
        				osvi.dwMajorVersion,
                osvi.dwMinorVersion,
                (osvi.dwBuildNumber & 0x0000FFFF));
      }
    }
  }
  else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
  	if (osvi.dwMajorVersion == 5)
    {
			sprintf(versionStr,"Windows 2000 (%d.%d.%d) %s",
      				osvi.dwMajorVersion,
              osvi.dwMinorVersion,
              osvi.dwBuildNumber,
              osvi.szCSDVersion);
    }
    else
    {
			sprintf(versionStr,"Windows NT (%d.%d.%d) %s",
      				osvi.dwMajorVersion,
              osvi.dwMinorVersion,
              osvi.dwBuildNumber,
              osvi.szCSDVersion);
    }
  }
  else
  {
		sprintf(versionStr,"Unknown Windows Version (%d.%d.%d) %s",
    				osvi.dwMajorVersion,
            osvi.dwMinorVersion,
            osvi.dwBuildNumber,
            osvi.szCSDVersion);
  }

  return versionStr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
char* ZCtrlOsVersionInfo::GetPlatformIdStr(void)
{
  if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
  {
		return "Win 32s";
  }
  else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
		return "Win 32 Windows";
  }
  else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
		return "Win 32 NT";
  }
  else
  {
		return "Unknown Win 32";
  }
}


#endif
