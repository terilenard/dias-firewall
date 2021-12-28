#pragma once
#include <string>
using namespace std;

class RuleRunner;

class FirewallManager
{
private:
	typedef struct  {
		string ruleFile;
	}FWCONFIG;

public:
	FirewallManager(const char* pszCfgFile);
	virtual ~FirewallManager();

public:
	bool initialize(void);
	void destroy(void);
	int permitMessage(const int iMsgID, const unsigned char* pPayload, const int nPayloadSz, const long timestamp);

private:
	bool readConfigFile(void);

private:
	RuleRunner* m_pRuleRunner;
	FWCONFIG m_fwConfig;
	string m_sCfgFile;
	bool secureLog;
	bool usesFreqProc;
	bool usesFreqNotifier;
	int notifierTimer = 0;
};

