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

