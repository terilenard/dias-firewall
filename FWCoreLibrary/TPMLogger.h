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
