#pragma once
#include "BaseLogger.h"

class TPMLogger : public BaseLogger
{
public:
	TPMLogger();
	virtual ~TPMLogger();

    virtual bool initialize(void);
	virtual bool logMessage(const string& msg);

private:
    int m_fd;
};
