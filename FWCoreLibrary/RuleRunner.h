#pragma once
#include "RuleFileProcessor.h"
#include <string>

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
	RuleRunner(const bool usesTPM);
	virtual ~RuleRunner();

	int permitMessage(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz);

private:
	int permitRuleChain(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz);
	int permitStateChain(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz);
	int decodeAction(const CHAIN_ACTION action);
	void checkStatefulInit(void);

private:
    list<STATEFW_CHINST> m_lstStateInst;
    BaseLogger* m_logger;
    std::string m_sLogMessage;
    bool useTPM;
};
