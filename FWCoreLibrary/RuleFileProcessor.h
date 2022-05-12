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

#pragma once

#include "ExpatWrapper.h"
#include <string>
#include <list>
#include <map>
using namespace std;
class ExpressionTree;

class RuleFileProcessor : public ExpatWrapper
{
protected:

	typedef enum {
		GLOBAL_NONE,
		GLOBAL_NETWORKS,
		GLOBAL_RULES,
		GLOBAL_RULE_CHAINS,
		GLOBAL_STATE_CHAINS
	}STATE_GLOBAL;

	typedef enum {
		NETWORKS_BEGIN,
		NETWORKS_NETWORK,
		NETWORKS_END
	}STATE_NETWORKS;

	typedef enum {
		ACTION_PERMIT,
		ACTION_PERMIT_LOG,
		ACTION_DROP,
		ACTION_DROP_LOG,
	}CHAIN_ACTION;

	typedef struct {
		CHAIN_ACTION action;
		string logMessage;
	}ACTION_SET;
	
	typedef struct {
		string id;
		int cid;
		string src;
		string dest;
		ExpressionTree* pExpTree;
	}RULE;

	typedef struct {
		string id;
		int cid;
		list<std::pair< RULE*, ACTION_SET >>* plstRules;
	}RULE_CHAIN;

public:
	RuleFileProcessor();
	virtual ~RuleFileProcessor();

	void printRules(void);

protected:
	virtual void onStartElement(const XML_Char* pName, const XML_Char** pAttrs);
	virtual void onEndElement(const XML_Char* pName);
	virtual void onValue(const XML_Char* pName, const int nLen);
	virtual bool isSemanticElementValid(void);

private:
	bool processNetworks(const XML_Char* pName, const XML_Char** pAttrs);
	bool processRules(const XML_Char* pName, const XML_Char** pAttrs);
	bool processRuleChains(const XML_Char* pName, const XML_Char** pAttrs);
	bool processStateChains(const XML_Char* pName, const XML_Char** pAttrs);
	bool processChain(const XML_Char* pName, const XML_Char** pAttrs);
	RULE* getRulePtr(const string& id);
	CHAIN_ACTION decodeAction(const string& action);
	string strDecodeAction(const CHAIN_ACTION action);

protected:
	STATE_GLOBAL m_stGlobal;
	STATE_NETWORKS m_stNetworks;
	bool m_continueProcessing;
	list<string> m_networks;
	RULE m_currentRule;
	list<RULE> m_lstRules;
	map<int, RULE_CHAIN> m_mapRuleChains;
	map<string, RULE_CHAIN> m_mapStateChains;
	RULE_CHAIN m_currentChain;
};

