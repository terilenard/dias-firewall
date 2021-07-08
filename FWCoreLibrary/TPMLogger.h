#pragma once
#include "BaseLogger.h"
#include <string>


class TPMLogger : public BaseLogger
{
public:
	TPMLogger(const char* pszCfgFile);
	virtual ~TPMLogger();

    virtual bool initialize(void);
	virtual bool logMessage(const string& msg);

private:
    int m_fd;
    string m_sCfgFile;
};
