/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge, Teri Lenard
*/

#include "RuleFileProcessor.h"
#include "ExpressionTree.h"
#include <string.h>
#include <stdio.h>

RuleFileProcessor::RuleFileProcessor() :
	m_stGlobal(GLOBAL_NONE),
	m_continueProcessing(true)
{
	m_currentRule.pExpTree = NULL;
}

RuleFileProcessor::~RuleFileProcessor()
{
	list<RULE>::iterator it;
	for (it = m_lstRules.begin(); it != m_lstRules.end(); ++it) {
		delete (*it).pExpTree;
	}

	map<int, RULE_CHAIN>::iterator it1;
	for (it1 = m_mapRuleChains.begin(); it1 != m_mapRuleChains.end(); ++it1) {
		delete (*it1).second.plstRules;
	}

	map<string, RULE_CHAIN>::iterator it2;
	for (it2 = m_mapStateChains.begin(); it2 != m_mapStateChains.end(); ++it2) {
		delete (*it2).second.plstRules;
	}
	
}

void RuleFileProcessor::onStartElement(const XML_Char* pName, const XML_Char** pAttrs)
{
	// Process according to global state
	switch (m_stGlobal) {
		case GLOBAL_NONE: {
			if (!strcmp(pName, "firewall-definition")) {
				break;
			}
			if (!strcmp(pName, "networks")) {
				m_stGlobal = GLOBAL_NETWORKS;
			}
			else if (!strcmp(pName, "rules")) {
				m_stGlobal = GLOBAL_RULES;
			}
			else if (!strcmp(pName, "rule-chains")) {
				m_stGlobal = GLOBAL_RULE_CHAINS;
			}
			else if (!strcmp(pName, "state-chains")) {
				m_stGlobal = GLOBAL_STATE_CHAINS;
			}
			else {
				printf("Unexpected start element while processing global state: %s\n", pName);
				m_continueProcessing = false;
			}
			break;
		}
		case GLOBAL_NETWORKS: {
			if (!processNetworks(pName, pAttrs)) {
				printf("Unexpected element '%s' while processing networks state\n", pName);
				m_continueProcessing = false;
			}
			break;
		}
		case GLOBAL_RULES: {
			if (!processRules(pName, pAttrs)) {
				printf("Unexpected element '%s' while processing rule state\n", pName);
				m_continueProcessing = false;
			}
			break;
		}
		case GLOBAL_RULE_CHAINS: {
			if (!processRuleChains(pName, pAttrs)) {
				printf("Unexpected element '%s' while processing rule-chain state\n", pName);
				m_continueProcessing = false;
			}
			break;
		}
		case GLOBAL_STATE_CHAINS: {
			if (!processStateChains(pName, pAttrs)) {
				printf("Unexpected element '%s' while processing state-chain state\n", pName);
				m_continueProcessing = false;
			}
			break;
		}
	}
}

void RuleFileProcessor::onEndElement(const XML_Char* pName)
{
	if (GLOBAL_RULES == m_stGlobal) {
		if (!strcmp(pName, "operator")) {
			m_currentRule.pExpTree->operatorEnd();
			return;
		}
		if (!strcmp(pName, "rule")) {
			m_lstRules.push_back(m_currentRule);
			m_currentRule.pExpTree = NULL;
			return;
		}
	}
	
	if (GLOBAL_RULE_CHAINS == m_stGlobal) {
		if (!strcmp(pName, "chain")) {
			m_mapRuleChains.insert(std::pair<int, RULE_CHAIN>(m_currentChain.cid, m_currentChain));
			m_currentChain.cid = -1;
			m_currentChain.id = "";
			m_currentChain.plstRules = NULL;
			return;
		}
	}

	if (GLOBAL_STATE_CHAINS == m_stGlobal) {
		if (!strcmp(pName, "chain")) {
			m_mapStateChains.insert(std::pair<string, RULE_CHAIN>(m_currentChain.id, m_currentChain));
			m_currentChain.cid = -1;
			m_currentChain.id = "";
			m_currentChain.plstRules = NULL;
			return;
		}
	}
	
	if (!strcmp(pName, "networks")) {
		m_stGlobal = GLOBAL_NONE;
		return;
	}
	if (!strcmp(pName, "rules")) {
		m_stGlobal = GLOBAL_NONE;
		return;
	}
	if (!strcmp(pName, "rule-chains")) {
		m_stGlobal = GLOBAL_NONE;
		return;
	}
	if (!strcmp(pName, "state-chains")) {
		m_stGlobal = GLOBAL_NONE;
		return;
	}

}

void RuleFileProcessor::onValue(const XML_Char* pName, const int nLen)
{

}

bool RuleFileProcessor::isSemanticElementValid(void)
{
	return m_continueProcessing;
}

bool RuleFileProcessor::processNetworks(const XML_Char* pName, const XML_Char** pAttrs)
{
	// Check for unexpected element
	if (strcmp(pName, "network")) {
		return false;
	}

	string idValue;
	if (!readAttrValue(pAttrs, "id", idValue)) {
		return false;
	}
	m_networks.push_back(idValue);
	return true;
}

bool RuleFileProcessor::processRules(const XML_Char* pName, const XML_Char** pAttrs)
{
	if (!strcmp(pName, "rule")) {
		string cid = "";
		if (!readAttrValue(pAttrs, "id", m_currentRule.id) || !readAttrValue(pAttrs, "cid", cid)) {
			return false;
		}
		m_currentRule.cid = atoi(cid.c_str());

		// Reset all other parameters
		m_currentRule.src = "";
		m_currentRule.dest = "";
		if (NULL != m_currentRule.pExpTree) { // This should not happen, but to avoid memory leaks test anyway
			delete m_currentRule.pExpTree;
			m_currentRule.pExpTree = NULL;
		}
		m_currentRule.pExpTree = new ExpressionTree();
		return true;
	}
	if (!strcmp(pName, "source")) {
		return readAttrValue(pAttrs, "network", m_currentRule.src);
	}
	if (!strcmp(pName, "destination")) {
		return readAttrValue(pAttrs, "network", m_currentRule.dest);
	}
	if (!strcmp(pName, "payload")) {
		return true;
	}
	if (!strcmp(pName, "expression")) {
		if (NULL != m_currentRule.pExpTree) { // This should not happen, but to avoid memory leaks test anyway
			delete m_currentRule.pExpTree;
			m_currentRule.pExpTree = NULL;
		}
		m_currentRule.pExpTree = new ExpressionTree();
		return true;
	}
	if (!strcmp(pName, "byte")) {
		if (NULL == m_currentRule.pExpTree) {
			return false;
		}
		string index;
		string value;
		if (!readAttrValue(pAttrs, "index", index)) {
			return false;
		}
		if (!readAttrValue(pAttrs, "value", value)) {
			if (!readAttrValue(pAttrs, "value-range", value)) {
				return false;
			}
			m_currentRule.pExpTree->addBoundValues(index, value);
		}
		else {
			m_currentRule.pExpTree->addSingleValue(index, value);
		}
		return true;
	}
	if (!strcmp(pName, "operator")) {
		if (NULL == m_currentRule.pExpTree) {
			return false;
		}
		string type;
		if (!readAttrValue(pAttrs, "type", type)) {
			return false;
		}
		return m_currentRule.pExpTree->operatorStart(type);
	}
	return false; // Unexpected value
}

bool RuleFileProcessor::processRuleChains(const XML_Char* pName, const XML_Char** pAttrs)
{
	if (!strcmp(pName, "chain")) {
		string cid;
		if (!readAttrValue(pAttrs, "cid", cid) || !readAttrValue(pAttrs, "id", m_currentChain.id)) {
			return false;
		}
		m_currentChain.cid = atoi(cid.c_str());
		m_currentChain.plstRules = new list<std::pair< RULE*, ACTION_SET >>();
		return true;
	}
	
	return processChain(pName, pAttrs);
}

bool RuleFileProcessor::processStateChains(const XML_Char* pName, const XML_Char** pAttrs)
{
	if (!strcmp(pName, "chain")) {
		if (!readAttrValue(pAttrs, "id", m_currentChain.id)) {
			return false;
		}
		m_currentChain.plstRules = new list<std::pair< RULE*, ACTION_SET >>();
		return true;
	}

	return processChain(pName, pAttrs);
}


void RuleFileProcessor::printRules(void)
{
	// Print networks
	list<string>::iterator it;
	for (it = m_networks.begin(); it != m_networks.end(); ++it) {
		printf("Network id = %s\n", (*it).c_str());
	}

	// Print rules
	list<RULE>::iterator it1;
	for (it1 = m_lstRules.begin(); it1 != m_lstRules.end(); ++it1) {
		printf("Rule id='%s', cid='%d', src='%s', dest='%s'\n", (*it1).id.c_str(), (*it1).cid, (*it1).src.c_str(), (*it1).dest.c_str());
		(*it1).pExpTree->printTree();
	}

	// Print rule-chains
	map<int, RULE_CHAIN>::iterator it2;
	for (it2 = m_mapRuleChains.begin(); it2 != m_mapRuleChains.end(); ++it2) {
		printf("Rule chain for cid='%d', id='%s'\n", (*it2).first, (*it2).second.id.c_str());
		
		list<std::pair< RULE*, ACTION_SET >>::iterator itlst;
		for (itlst = (*it2).second.plstRules->begin(); itlst != (*it2).second.plstRules->end(); ++itlst) {
			if (NULL != (*itlst).first)
				printf("\tid='%s', action='%s', message='%s'\n", (*itlst).first->id.c_str(), strDecodeAction((*itlst).second.action).c_str(), (*itlst).second.logMessage.c_str());
			else
				printf("\tid=default, action='%s', message='%s'\n", strDecodeAction((*itlst).second.action).c_str(), (*itlst).second.logMessage.c_str());
		}
	}

	// Print state-chains
	map<string, RULE_CHAIN>::iterator it3;
	for (it3 = m_mapStateChains.begin(); it3 != m_mapStateChains.end(); ++it3) {
		printf("State chain with id='%s'\n", (*it3).first.c_str());

		list<std::pair< RULE*, ACTION_SET >>::iterator itlst;
		for (itlst = (*it3).second.plstRules->begin(); itlst != (*it3).second.plstRules->end(); ++itlst) {
			if (NULL != (*itlst).first)
				printf("\tid='%s', action='%s', message='%s'\n", (*itlst).first->id.c_str(), strDecodeAction((*itlst).second.action).c_str(), (*itlst).second.logMessage.c_str());
			else
				printf("\tid=default, action='%s', message='%s'\n", strDecodeAction((*itlst).second.action).c_str(), (*itlst).second.logMessage.c_str());
		}
	}

}

RuleFileProcessor::RULE* RuleFileProcessor::getRulePtr(const string& id)
{
	list<RULE>::iterator it;
	for (it = m_lstRules.begin(); it != m_lstRules.end(); ++it) {
		if (!(*it).id.compare(id)) {
			return &(*it);
		}
	}
	return NULL;
}

bool RuleFileProcessor::processChain(const XML_Char* pName, const XML_Char** pAttrs)
{
	if (!strcmp(pName, "rule")) {
		if (NULL == m_currentChain.plstRules) {
			return false;
		}

		string id;
		string action;
		string message = "";
		if (!readAttrValue(pAttrs, "id", id) || !readAttrValue(pAttrs, "action", action)) {
			return false;
		}
		readAttrValue(pAttrs, "message", message);

		RULE* pRule = getRulePtr(id);
		if (NULL == pRule) {
			return false;
		}

		ACTION_SET actDetails;
		actDetails.action = decodeAction(action);
		actDetails.logMessage = message;

		m_currentChain.plstRules->push_back(std::pair< RULE*, ACTION_SET >(pRule, actDetails));
		return true;
	}
	if (!strcmp(pName, "default")) {
		if (NULL == m_currentChain.plstRules) {
			return false;
		}
		string action;
		string message = "";
		if (!readAttrValue(pAttrs, "action", action)) {
			return false;
		}
		readAttrValue(pAttrs, "message", message);

		ACTION_SET actDetails;
		actDetails.action = decodeAction(action);
		actDetails.logMessage = message;
		m_currentChain.plstRules->push_back(std::pair< RULE*, ACTION_SET >(NULL, actDetails));
		return true;
	}
	return false;
}

RuleFileProcessor::CHAIN_ACTION RuleFileProcessor::decodeAction(const string& action)
{
	CHAIN_ACTION chAction = ACTION_PERMIT;
	if (action == "PERMIT");
	else if (action == "PERMIT-LOG") {
		chAction = ACTION_PERMIT_LOG;
	}
	else if (action == "DROP") {
		chAction = ACTION_DROP;
	}
	else if (action == "DROP-LOG") {
		chAction = ACTION_DROP_LOG;
	}
	return chAction;
}

string RuleFileProcessor::strDecodeAction(const CHAIN_ACTION action)
{
	if (action == ACTION_PERMIT)
		return "PERMIT";
	if (action == ACTION_PERMIT_LOG)
		return "PERMIT-LOG";
	if (action == ACTION_DROP)
		return "DROP";
	if (action == ACTION_DROP_LOG)
		return "DROP-LOG";
	return "UNDEFINED!";
}