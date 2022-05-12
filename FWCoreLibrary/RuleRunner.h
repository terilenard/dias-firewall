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
#include "FrequencyProcessor.h"
#include "RuleFileProcessor.h"
#include <string>
#include <thread>
#include <mutex>

class BaseLogger;

class RuleRunner : public RuleFileProcessor
{
private:
	typedef struct {
		string chId;
		list<std::pair< RULE*, ACTION_SET >>* plstRules;
		list<std::pair< RULE*, ACTION_SET >>::iterator currState;
	}STATEFW_CHINST;

public:
	RuleRunner();
	RuleRunner(const bool usesTPM, const bool usesFreqProc, const bool usesFreqNotifier, const int notifierTimer);
	virtual ~RuleRunner();

	int permitMessage(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz, const unsigned long long timestamp);

private:
	int permitRuleChain(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz);
	int permitStateChain(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz);
	int decodeAction(const CHAIN_ACTION action);
	void checkStatefulInit(void);

private:
    list<STATEFW_CHINST> m_lstStateInst;
    BaseLogger* m_logger;
    std::string m_sLogMessage;
    bool useTPM, useFreqProc, useFreqNotifier;
    FrequencyProcessor* fp;
	thread th;
    mutex mtx;
};
