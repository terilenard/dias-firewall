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
#include <string>

#ifndef WIN32
#include <poll.h>
#endif
using namespace std;

class CANHandler
{
public:
    CANHandler(const char* pszCfgFile);
    virtual ~CANHandler();

    bool initialize(void(*callback)(int idx, unsigned char* payload, void* arg, int dlc,unsigned long long timestamp), void* arg);
    bool runHandler(void);

private:

#ifndef WIN32
    struct pollfd m_poll;
#endif

    void (*m_callback)(int idx, unsigned char* payload, void* arg, int dlc, unsigned long long timestamp);
    void* m_arg;
    string m_sCfgFile;
};

