/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge
*/

#ifdef WIN32
#include <windows.h>
#endif
#include <map>
#include "FWCore.h"
#include "FirewallManager.h"

using namespace std;

#ifdef WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

map<int, FirewallManager* > g_mapFWInstances;
int g_lastInstIdx = 100; // This should be greater than the last code defined in FWCore
int g_lastError = FWCORE_RESULT_OK;

FWCORE_API int createFWInstance(const char* pszCfgFile) {
	FirewallManager* pmngr = new FirewallManager(pszCfgFile);
	if (!pmngr->initialize()) {
		delete pmngr;
		g_lastError = FWCORE_ERROR_INITFAILED;
		return FWCORE_ERROR;
	}
	g_mapFWInstances.insert(std::pair<int, FirewallManager*>(++g_lastInstIdx, pmngr));
	g_lastError = FWCORE_RESULT_OK;
	return g_lastInstIdx;
}

FWCORE_API int destroyFWInstance(int iFwInstance) {
	std::map<int, FirewallManager*>::iterator fwInst = g_mapFWInstances.find(iFwInstance);
	if (fwInst == g_mapFWInstances.end()) {
		g_lastError = FWCORE_ERROR_INSTNOTFOUND;
		return FWCORE_ERROR;
	}
	delete (*fwInst).second;
	g_mapFWInstances.erase(iFwInstance);

	g_lastError = FWCORE_RESULT_OK;
	return FWCORE_RESULT_OK;
}

FWCORE_API int processMessage(int iFwInstance, const int iMsgIdx, const unsigned char* pPayload, const int nPayloadSz, const unsigned long long timestamp)
{
	std::map<int, FirewallManager*>::iterator fwInst = g_mapFWInstances.find(iFwInstance);
	if (fwInst == g_mapFWInstances.end()) {
		g_lastError = FWCORE_ERROR_INSTNOTFOUND;
		return FWCORE_ERROR;
	}

	g_lastError = FWCORE_RESULT_OK;
	return (*fwInst).second->permitMessage(iMsgIdx, pPayload, nPayloadSz, timestamp);
}

FWCORE_API int getLastError(void)
{
	return g_lastError;
}

