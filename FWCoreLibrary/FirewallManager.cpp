#include "FirewallManager.h"
#include "RuleRunner.h"
#include "FWCore.h"
#include <libconfig.h>
#include <string>
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

	m_pRuleRunner = new RuleRunner();

	// Next, process the firewall rules
	if (!m_pRuleRunner->initialise()) {
		delete m_pRuleRunner;
		m_pRuleRunner = NULL;
		return false;
	}

	if (!m_pRuleRunner->parseFile(m_fwConfig.ruleFile.c_str())) {
		m_pRuleRunner->destroy();
		delete m_pRuleRunner;
		m_pRuleRunner = NULL;
		return false;
	}

	m_pRuleRunner->printRules();
	
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
	config_t cfg;
	const char* str;

	/*Initialization */
	config_init(&cfg);

	/* Read the file. If there is an error, report it and exit. */
	if (!config_read_file(&cfg, m_sCfgFile.c_str()))
	{
		printf("\n%s:%d - %s", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return false;
	}

	/* Get the configuration file name. */
	if (!config_lookup_string(&cfg, "ruleFile", &str)) {
		printf("Unable to find 'ruleFile' in configuration file\n");
		config_destroy(&cfg);
		return false;
	}

	// Save the string and then destroy the context (also destroys allocated buffers!)
	m_fwConfig.ruleFile = str;

	config_destroy(&cfg);

	printf("Found ruleFile = %s\n", m_fwConfig.ruleFile.c_str());

	return true;
}

int FirewallManager::permitMessage(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz)
{
	return m_pRuleRunner->permitMessage(iMsgID, pPayload, nPayloadSz);
}