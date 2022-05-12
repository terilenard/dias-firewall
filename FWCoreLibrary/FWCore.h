/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge, Roland Bolboaca
*/

#pragma once

#ifdef WIN32
	#ifdef FWCORELIBRARY_EXPORTS
	#define FWCORE_API __declspec(dllexport)
	#else
	#define FWCORE_API __declspec(dllimport)
	#endif
#else
	#define FWCORE_API __attribute__ ((__visibility__("default")))
#endif

#define FWCORE_RESULT_OK 0

// FW Core error codes
#define FWCORE_ERROR -1
#define FWCORE_ERROR_INITFAILED -2
#define FWCORE_ERROR_INSTNOTFOUND -3

// FW Core processing result codes
#define FWCORE_PROC_PERMIT 1
#define FWCORE_PROC_DROP 2
#define FWCORE_PROC_PERMIT_LOG 3
#define FWCORE_PROC_DROP_LOG 4
#define FWCORE_PROC_CHAINUNDEFINED 5


#define FWCORE_FREQ_OK 6
#define FWCORE_FREQ_BAD 7
#define FWCORE_FREQ_ERROR 8

// FW Core API
FWCORE_API int createFWInstance(const char* pszCfgFile);
FWCORE_API int destroyFWInstance(int iFwInstance);
FWCORE_API int processMessage(int iFwInstance, const int iMsgIdx, const unsigned char* pPayload, const int nPayloadSz, const unsigned long long timestamp);
FWCORE_API int getLastError(void);
