/*

	10-Jan-2005			Richard Campion

*/

#ifndef ___VERSION_H___
#define ___VERSION_H___

#define COPYRIGHT "(C) 2020 ZdsLogic Technologies";

#define RELEASE "v0.1.1.0"

//#define GEN_DATE              "(compiled "##__DATE__##" at "##__TIME__##")\n# "##COPYRIGHT
#define GEN_DATE              "##__DATE__####__TIME__##"
#define PROG_NAME             "cpp-tab-parser"
#define PROG_VERSION          RELEASE

#define PROG_VERSION_REL      "##PROG_NAME## "##RELEASE##" (RELEASE Version) (compiled "##__DATE__##" at "##__TIME__##")\n"##COPYRIGHT
#define PROG_VERSION_DBG      "##PROG_NAME## "##RELEASE##" (DEBUG Version) (compiled "##__DATE__##" at "##__TIME__##")\n"##COPYRIGHT

#define __DATE____TIME__COPYRIGHT "##__DATE__####__TIME__####COPYRIGHT"

#endif // ___VERSION_H___ 

