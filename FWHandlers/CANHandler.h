#pragma once

#ifndef WIN32
#include <poll.h>
#endif

class CANHandler
{
public:
    CANHandler();
    virtual ~CANHandler();

    bool initialize(void(*callback)(int idx, unsigned char* payload, void* arg), void* arg);
    bool runHandler(void);

private:

#ifndef WIN32
    struct pollfd m_poll;
#endif

    void (*m_callback)(int idx, unsigned char* payload, void* arg);
    void* m_arg;
};

