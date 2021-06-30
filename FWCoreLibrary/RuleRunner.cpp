#include "RuleRunner.h"
#include "ExpressionTree.h"
#include "FWCore.h"
#include "TPMLogger.h"

RuleRunner::RuleRunner()
{
    m_logger = new TPMLogger();
    m_logger->initialize();
    m_sLogMessage = "";
}
RuleRunner::~RuleRunner()
{
    delete m_logger;
}

int RuleRunner::permitMessage(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz)
{
    // Reset log message
    m_sLogMessage = "N/A";

	// First, look through rule chain
	int retCode = permitRuleChain(iMsgID, pPayload, nPayloadSz);

	// Then, look through the state chain
	int stCode = permitStateChain(iMsgID, pPayload, nPayloadSz);
	if (FWCORE_PROC_CHAINUNDEFINED == retCode) {
		retCode = stCode;
	}
	else if (FWCORE_PROC_DROP == stCode || FWCORE_PROC_DROP_LOG == stCode) {
		retCode = stCode;
	}

	if ((FWCORE_PROC_PERMIT_LOG == retCode) || (FWCORE_PROC_DROP_LOG == retCode)) {
		m_logger->logMessage(m_sLogMessage);
	}
	if (FWCORE_PROC_CHAINUNDEFINED == retCode) {
		// Make sure default action is returned
		retCode = FWCORE_PROC_PERMIT;
	}
	
	return retCode; 
}

int RuleRunner::permitRuleChain(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz)
{
	int retCode = FWCORE_PROC_CHAINUNDEFINED;
	map<int, RULE_CHAIN>::iterator ruleChainIt = m_mapRuleChains.find(iMsgID);
	if (ruleChainIt == m_mapRuleChains.end()) {
		return FWCORE_PROC_CHAINUNDEFINED;
	}
	
	// Step through each rule and search for the first match - or look out for the default value
	// In case no default value is defined, then we PERMIT!!!!
	list<std::pair< RULE*, ACTION_SET >>::iterator it;
	for (it = (*ruleChainIt).second.plstRules->begin(); it != (*ruleChainIt).second.plstRules->end(); ++it) {
		if (NULL == (*it).first) {
			// We have stummbled uppon the default rule
			retCode = decodeAction((*it).second.action);

            // Also store the log message (if there is one)
            if ((*it).second.logMessage.size() > 0) {
                m_sLogMessage = (*it).second.logMessage;
            }
			break;
		}

		if ((*it).first->pExpTree->matchMessage(pPayload, nPayloadSz)) {
			// We have found a match!
			retCode = decodeAction((*it).second.action);

            // Also store the log message (if there is one)
            if ((*it).second.logMessage.size() > 0) {
                m_sLogMessage = (*it).second.logMessage;
            }

			break;
		}
	}
	return retCode;
}

int RuleRunner::permitStateChain(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz)
{
	int retState = FWCORE_PROC_PERMIT; // default

	// Make sure data structures are loaded - only done the first time the method is called
	checkStatefulInit();

	// Verify if the message is part of any state instance
	list<STATEFW_CHINST>::iterator lstIt;
	for (lstIt = m_lstStateInst.begin(); lstIt != m_lstStateInst.end(); ++lstIt) {
		RULE* rule = ((*lstIt).currState)->first;
		if (rule->cid == iMsgID) {
			// In case we find a match in the chain for this message, we save the action and advance the chain pointer
			if (rule->pExpTree->matchMessage(pPayload, nPayloadSz)) {
				retState = decodeAction(((*lstIt).currState)->second.action);

                // Also store the log message (if there is one)
                if (((*lstIt).currState)->second.logMessage.size() > 0) {
                    m_sLogMessage = ((*lstIt).currState)->second.logMessage;
                }

				++(*lstIt).currState;
				printf("State chain '%s' progressed to the next element!\n", (*lstIt).chId.c_str());

				// Verify if we reached the end of the chain, and reset the chain
				if ((*lstIt).currState == (*lstIt).plstRules->end()) {
					(*lstIt).currState = (*lstIt).plstRules->begin();
					printf("State chain '%s' reached end and was reset!\n", (*lstIt).chId.c_str());
				}
			}
		}
	}

	return retState;
}

int RuleRunner::decodeAction(const CHAIN_ACTION action)
{
	if (action == ACTION_PERMIT)
		return FWCORE_PROC_PERMIT;
	if (action == ACTION_PERMIT_LOG)
		return FWCORE_PROC_PERMIT_LOG;
	if (action == ACTION_DROP)
		return FWCORE_PROC_DROP;
	return FWCORE_PROC_DROP_LOG;
}

void RuleRunner::checkStatefulInit(void)
{
	// Check if stateful structures are already set up
	if (m_lstStateInst.size() > 0) {
		return;
	}

	// Otherwise, prepare the data structures for the stateful firewall
	map<string, RULE_CHAIN>::iterator it;
	for (it = m_mapStateChains.begin(); it != m_mapStateChains.end(); ++it) {
		STATEFW_CHINST inst;
		inst.chId = (*it).first;
		inst.plstRules = (*it).second.plstRules;
		inst.currState = inst.plstRules->begin();

		m_lstStateInst.push_back(inst);
	}
}