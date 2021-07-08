#include "FirewallManager.h"
#include "RuleRunner.h"
#include "FWCore.h"
#include "Config.h"
#include <string>
#include <cstring>
using namespace std;

FirewallManager::FirewallManager(const char* pszCfgFile):
	m_sCfgFile(pszCfgFile),
	m_pRuleRunner(NULL)
{

}

FirewallManager::~FirewallManager()
{

}

bool FirewallManager::initialize(void)
{
	if (NULL != m_pRuleRunner) {
		return false;
	}
	// Read the configuration file
	if (!readConfigFile()) {
		return false;
	}

	m_pRuleRunner = new RuleRunner(secureLog);

	// Next, process the firewall rules
	if (!m_pRuleRunner->initialise()) {
		delete m_pRuleRunner;
		m_pRuleRunner = NULL;
		printf("Rule Runner: Initialize Error\n");
		return false;
	}

	if (!m_pRuleRunner->parseFile(m_fwConfig.ruleFile.c_str())) {
		m_pRuleRunner->destroy();
		delete m_pRuleRunner;
		m_pRuleRunner = NULL;
		printf("Rule Runner: file parse Error\n");
		return false;
	}

	m_pRuleRunner->printRules();
	printf("Printing Rules\n");
	return true;
}

void FirewallManager::destroy(void)
{
	if (NULL == m_pRuleRunner) {
		return;
	}
	m_pRuleRunner->destroy();
	delete m_pRuleRunner;
	m_pRuleRunner = NULL;
}

bool FirewallManager::readConfigFile(void)
{
	const char* str;
	int ret;

	str = getParameter(m_sCfgFile.c_str(),"ruleFile");
	
	if (!str)
		return false;
	printf("Config file was read successfully\n");

	// Save the string and then destroy the context (also destroys allocated buffers!)
	m_fwConfig.ruleFile = str;

	printf("Found ruleFile = %s\n", m_fwConfig.ruleFile.c_str());

	str = getParameter(m_sCfgFile.c_str(),"secureLog");

	if (!str)
		return false;

	ret = strcmp(str,"true");
	if (ret == 0)
		secureLog = true;
	else
		secureLog = false;

	printf("Secure Log option %s was read successfully\n", str);

	return true;
}

int FirewallManager::permitMessage(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz)
{
	return m_pRuleRunner->permitMessage(iMsgID, pPayload, nPayloadSz);
}